/* Public Domain Curses */

#include "pdcwin.h"

#include "../common/acsuni.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#ifdef WIN32_LEAN_AND_MEAN
   #include <commdlg.h>
#endif

static void PDC_get_rgb_values( const chtype srcp,
            COLORREF *foreground_rgb, COLORREF *background_rgb);

/* For this 'real Windows' version,  we use all Unicode all the time,
including for ACS characters,  and even when PDC_WIDE isn't #defined
(i.e., when running in 'legacy' 8-bit character mode) See 'acs_defs.h'
for details. */


static const unsigned short starting_ascii_to_unicode[32] = {
   0,
   0x263a,        /*   1 smiling face              */
   0x263b,        /*   2 smiling face inverted     */
   0x2665,        /*   3 heart                     */
   0x2666,        /*   4 diamond                   */
   0x2663,        /*   5 club                      */
   0x2660,        /*   6 spade                     */
   0x2024,        /*   7 small bullet              */
   0x25d8,        /*   8 inverted bullet           */
   0x25bc,        /*   9 hollow bullet             */
   0x25d9,        /*  10 inverted hollow bullet    */
   0x2642,        /*  11 male/Mars symbol          */
   0x2640,        /*  12 female/Venus symbol       */
   0x266a,        /*  13 eighth note               */
   0x266c,        /*  14 two sixteenth notes       */
   0x263c,        /*  15 splat                     */
   0x25b6,        /*  16 right-pointing triangle   */
   0x25c0,        /*  17 left-pointing triangle    */
   0x2195,        /*  18 double up/down arrow      */
   0x203c,        /*  19 double exclamation !!     */
   0x00b6,        /*  20 pilcrow                   */
   0xa7,          /*  21                           */
   0x2582,        /*  22 lower 1/3 block           */
   0x280d,        /*  23 double up/down arrow      */
   0x2191,        /*  24 up arrow                  */
   0x2193,        /*  25 down arrow                */
   0x2192,        /*  26 right arrow               */
   0x2190,        /*  27 left arrow                */
   0x2319,        /*  28                           */
   0x280c,        /*  29 left & right arrow        */
   0x25b2,        /*  30 up triangle               */
   0x25bc};       /*  31 down triangle             */

/* Cursors may be added to the 'shapes' array.  A 'shapes' string
defines the cursor as one or more rectangles,  separated by semicolons.
The coordinates of the upper left and lower right corners are given,
usually just as integers from zero to eight.  Thus,  "0488" means a
rectangle running from (0,4),  middle of the left side,  to (8,8),
bottom right corner:  a rectangle filling the bottom half of the
character cell. "0048" would fill the left half of the cell,  and
"0082;6088" would fill the top and bottom quarters of the cell.

   However,  a coordinate may be followed by a + or -,  and then by a
single-digit offset in pixels.  So "08-4" refers to a point on the
left-hand side of the character cell,  four pixels from the bottom. I
admit that the cursor descriptions themselves look a little strange!
But this way of describing cursors is compact and lends itself to some
pretty simple code.

   The first three lines are standard PDCurses cursors:  0=no cursor,
1=four-pixel thick line at bottom of the cell,  2="high-intensity",
i.e., a filled block.  The rest are extended cursors,  not currently
available in other PDCurses flavors. */

#define N_CURSORS 9

static void redraw_cursor_from_index( const HDC hdc, const int idx)
{
    const char *shapes[N_CURSORS] = {
        "",                         /* 0: invisible */
        "08-488",                    /* 1: normal:  four lines at bottom */
        "0088",                       /* 2: full block */
        "0088;0+10+18-18-1",           /* 3: outlined block */
        "28-368;4-10+34+18-3;2060+3",  /* 4: caret */
        "0488",                       /* 5: bottom half block */
        "2266",                      /* 6: central block     */
        "0385;3053;3558",           /* 7: cross */
        "0088;0+10+48-18-4"  };    /* 8: outlined block: heavy top/bottom*/
    const char *sptr = shapes[idx];
    LONG left, top;

    left = SP->curscol * PDC_cxChar;
    top = SP->cursrow * PDC_cyChar;
    while( *sptr)
    {
        int i;
        LONG coords[4];
        RECT rect;

        for( i = 0; i < 4; i++)
        {
            coords[i] = (( i & 1) ?
                         top + (PDC_cyChar * (*sptr - '0') + 4) / 8 :
                         left + (PDC_cxChar * (*sptr - '0') + 4) / 8);
            sptr++;
            if( *sptr == '+' || *sptr == '-')
            {
                if( *sptr == '+')
                   coords[i] += sptr[1] - '0';
                else
                   coords[i] -= sptr[1] - '0';
                sptr += 2;
            }
        }
        rect.left = coords[0];
        rect.top = coords[1];
        rect.right = coords[2];
        rect.bottom = coords[3];
        InvertRect( hdc, &rect);
        if( *sptr == ';')
            sptr++;
    }
}

/* PDC_current_cursor_state( ) determines which cursor,  if any,
is currently shown.  This may depend on the blink state.  Also,
if the window currently lacks the focus,  we show cursor 3 (a hollow
box) in place of any visible cursor.  */

static int PDC_current_cursor_state( void)
{
    const int shift_amount = (PDC_blink_state ? 0 : 8);
    const int cursor_style_for_unfocussed_window =
               PDC_CURSOR( PDC_CURSOR_OUTLINE, PDC_CURSOR_OUTLINE);
    int cursor_style;

            /* for unfocussed windows, show an hollow box: */
    if( SP->visibility && (PDC_hWnd != GetForegroundWindow( )))
        cursor_style = cursor_style_for_unfocussed_window;
    else    /* otherwise,  just show the cursor "normally" */
        cursor_style = SP->visibility;
    return( (cursor_style >> shift_amount) & 0xff);
}

static void redraw_cursor( const HDC hdc)
{
    const int cursor_style = PDC_current_cursor_state( );

    if( cursor_style > 0 && cursor_style < N_CURSORS)
        redraw_cursor_from_index( hdc, cursor_style);
}

/* position "hardware" cursor at (y, x).  We don't have a for-real hardware */
/* cursor in this version,  of course,  but we can fake it.  Note that much */
/* of the logic was borrowed from the SDL version.  In particular,  the     */
/* cursor is moved by first overwriting the "original" location.            */

void PDC_gotoyx(int row, int col)
{
    PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
             row, col, SP->cursrow, SP->curscol));

                /* clear the old cursor,  if it's on-screen: */
    if( SP->cursrow >= 0 && SP->curscol >= 0 &&
         SP->cursrow < SP->lines && SP->curscol < SP->cols)
    {
        const int temp_visibility = SP->visibility;

        SP->visibility = 0;
        PDC_transform_line( SP->cursrow, SP->curscol, 1,
                           curscr->_y[SP->cursrow] + SP->curscol);
        SP->visibility = temp_visibility;
    }

               /* ...then draw the new (assuming it's actually visible).    */
               /* This used to require some logic.  Now the redraw_cursor() */
               /* function figures out what cursor should be drawn, if any. */
    if( SP->visibility)
    {
        HDC hdc = GetDC( PDC_hWnd) ;

        SP->curscol = col;
        SP->cursrow = row;
        redraw_cursor( hdc);
        ReleaseDC( PDC_hWnd, hdc) ;
    }
}

int PDC_font_size = 12;
TCHAR PDC_font_name[80];

static LOGFONT PDC_get_logical_font( const int font_idx)
{
    LOGFONT lf;

    memset(&lf, 0, sizeof(LOGFONT));        /* Clear out structure. */
    lf.lfHeight = -PDC_font_size;
#ifdef PDC_WIDE
    if( !*PDC_font_name)
        wcscpy( PDC_font_name, _T("Courier New"));
    if( font_idx & 4)
        wcscpy( lf.lfFaceName, _T("Unifont"));
    else
        wcscpy( lf.lfFaceName, PDC_font_name );
/*  wprintf( L"New font: %s\n", PDC_font_name); */
#else
    if( !*PDC_font_name)
        strcpy( PDC_font_name, "Courier New");
    if( font_idx & 4)
        strcpy( lf.lfFaceName, "Unifont");
    else
        strcpy( lf.lfFaceName, PDC_font_name);
#endif
/*  lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN; */
    lf.lfPitchAndFamily = FF_MODERN;
    lf.lfWeight = ((font_idx & 1) ? FW_EXTRABOLD : FW_NORMAL);
    lf.lfItalic = ((font_idx & 2) ? TRUE : FALSE);
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfQuality = PROOF_QUALITY;
    lf.lfOutPrecision = OUT_RASTER_PRECIS;
    return( lf);
}

HFONT PDC_get_font_handle( const int font_idx)
{
    LOGFONT lf = PDC_get_logical_font( font_idx);

    return( CreateFontIndirect( &lf));
}

int PDC_choose_a_new_font( void)
{
    LOGFONT lf = PDC_get_logical_font( 0);
    CHOOSEFONT cf;
    int rval;

    lf.lfHeight = -PDC_font_size;
    debug_printf( "In PDC_choose_a_new_font: %d\n", lf.lfHeight);
    memset( &cf, 0, sizeof( CHOOSEFONT));
    cf.lStructSize = sizeof( CHOOSEFONT);
    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
    cf.hwndOwner = PDC_hWnd;
    cf.lpLogFont = &lf;
    cf.rgbColors = RGB( 0, 0, 0);
    rval = ChooseFont( &cf);
    if( rval)
#ifdef PDC_WIDE
        wcscpy( PDC_font_name, lf.lfFaceName);
#else
        strcpy( PDC_font_name, lf.lfFaceName);
#endif
    debug_printf( "rval %d; %ld\n", rval, CommDlgExtendedError( ));
    debug_printf( "output size: %d\n", lf.lfHeight);
    PDC_font_size = -lf.lfHeight;
    return( rval);
}

/* see 'addch.c' for an explanation of how combining chars are handled. */

#ifdef USING_COMBINING_CHARACTER_SCHEME
   int PDC_expand_combined_characters( const cchar_t c, cchar_t *added);  /* addch.c */
#endif

#ifdef PDC_WIDE
const chtype MAX_UNICODE = 0x110000;
#endif

#ifdef USE_FALLBACK_FONT
GLYPHSET *PDC_unicode_range_data = NULL;

/* Note that in the following,  if a character has a Unicode point
greater than 64K (i.e.,  it's not in the Basic Multilingual Plane),
we assume it has a glyph in the font.  The problem is that,  due to
the brain-dead implementation of Unicode in Windoze,  we can't
actually tell if there is a glyph or not;  the "range" data is 16-bit. */

static bool character_is_in_font( chtype ichar)
{
    int i;
    WCRANGE *wptr = PDC_unicode_range_data->ranges;

    if( (ichar & A_ALTCHARSET) && (ichar & A_CHARTEXT) < 0x80)
       ichar = acs_map[ichar & 0x7f];
    ichar &= A_CHARTEXT;
    if( ichar > MAX_UNICODE)  /* assume combining chars won't be */
       return( FALSE);        /* supported;  they rarely are     */
    if( ichar > 0xffff)     /* see above comments */
       return( TRUE);
    for( i = PDC_unicode_range_data->cRanges; i; i--, wptr++)
      if( wptr->wcLow > ichar)
         return( FALSE);
      else if( wptr->wcLow + wptr->cGlyphs > ichar)
         return( TRUE);
               /* Didn't find it in any range;  it must not be in the font */
    return( FALSE);
}
#endif         /* #ifdef USE_FALLBACK_FONT */

/* update the given physical line to look like the corresponding line in
curscr.

   NOTE that if x > 0,  we decrement it and srcp,  and increment the
length.  In other words,  we draw the preceding character,  too.  This
is done because,  at certain font sizes,  characters break out and
overwrite the preceding character.  That results in debris left on
the screen.

   The code also now increments the length only,  drawing one more
character (i.e.,  draws the character following the "text we really
want").  Again,  this helps to avoid debris left on the screen.

   The 'ExtTextOut' function takes an lpDx array that specifies the exact
placement of each character relative to the previous character.  This seems
to help avoid most (but not all) stray pixels from being displayed.  The
problem is that,  at certain font sizes,  letters may be drawn that don't
fit properly in the clip rectangle;  and they aren't actually clipped
correctly,  despite the use of the ETO_CLIPPED flag.  But things do seem
to be much better than was the case back when plain 'TextOut' was used. */

#define N_CACHED_FONTS     8

static HFONT hFonts[N_CACHED_FONTS];

#define BUFFSIZE 50

void PDC_transform_line_given_hdc( const HDC hdc, const int lineno,
                             int x, int len, const chtype *srcp)
{
    HFONT hOldFont = (HFONT)0;
    int i, curr_color = -1;
    attr_t font_attrib = (attr_t)-1;
    int cursor_overwritten = FALSE;
    COLORREF foreground_rgb = 0;
    chtype prev_ch = 0;

    if( !srcp)             /* just freeing up fonts */
    {
        for( i = 0; i < N_CACHED_FONTS; i++)
            if( hFonts[i])
            {
                DeleteObject( hFonts[i]);
                hFonts[i] = NULL;
            }
#ifdef USE_FALLBACK_FONT
        if( PDC_unicode_range_data)
        {
            free( PDC_unicode_range_data);
            PDC_unicode_range_data = NULL;
        }
#endif
        return;
    }
                     /* Seems to me as if the input text to this function */
    if( x < 0)       /* should _never_ be off-screen.  But it sometimes is. */
    {                /* Clipping is therefore necessary. */
        len += x;
        srcp -= x;
        x = 0;
    }
    len++;    /* draw an extra char to avoid leaving garbage on screen */
    if( len > SP->cols - x)
        len = SP->cols - x;
    if( lineno >= SP->lines || len <= 0 || lineno < 0)
        return;
    if( x)           /* back up by one character to avoid */
    {                /* leaving garbage on the screen */
        x--;
        len++;
        srcp--;
    }
    if( lineno == SP->cursrow && SP->curscol >= x && SP->curscol < x + len)
        if( PDC_current_cursor_state( ))
            cursor_overwritten = TRUE;


    while( len)
    {
        const attr_t attrib = (attr_t)( *srcp >> PDC_REAL_ATTR_SHIFT);
        const int color = (int)(( *srcp & A_COLOR) >> PDC_COLOR_SHIFT);
        attr_t new_font_attrib = (*srcp & (A_BOLD | A_ITALIC));
        RECT clip_rect;
        wchar_t buff[BUFFSIZE];
        int lpDx[BUFFSIZE + 1];
        int olen = 0;
#ifdef USE_FALLBACK_FONT
        const bool in_font = character_is_in_font( *srcp);
#endif

        for( i = 0; i < len && olen < BUFFSIZE - 1
#ifdef USE_FALLBACK_FONT
                  && (in_font == character_is_in_font( srcp[i])
                              || (srcp[i] & A_CHARTEXT) == MAX_UNICODE)
#endif
                  && attrib == (attr_t)( srcp[i] >> PDC_REAL_ATTR_SHIFT); i++)
        {
            chtype ch = srcp[i] & A_CHARTEXT;

#ifdef USING_COMBINING_CHARACTER_SCHEME
            if( ch > 0xffff && ch < MAX_UNICODE)  /* use Unicode surrogates to fit */
            {               /* >64K values into 16-bit wchar_t: */
                ch -= 0x10000;
                buff[olen] = (wchar_t)( 0xd800 | (ch >> 10));
                lpDx[olen] = 0;                   /* ^ upper 10 bits */
                olen++;
                ch = (wchar_t)( 0xdc00 | (ch & 0x3ff));  /* lower 10 bits */
            }
            if( ch > MAX_UNICODE)      /* chars & fullwidth supported */
            {
                cchar_t added[10], root = ch;
                int n_combined = 0;

                while( (root = PDC_expand_combined_characters( root,
                                   &added[n_combined])) > MAX_UNICODE)
                {
                    n_combined++;
                }
                buff[olen] = (wchar_t)root;
                lpDx[olen] = 0;
                olen++;
                ch = (wchar_t)added[n_combined];
                while( n_combined)
                {
                    n_combined--;
                    buff[olen] = (wchar_t)added[n_combined];
                    lpDx[olen] = 0;
                    olen++;
                }
            }
#endif
            if( (srcp[i] & A_ALTCHARSET) && ch < 0x80)
                ch = acs_map[ch & 0x7f];
            else if( ch < 32)
               ch = starting_ascii_to_unicode[ch];
#ifndef PDC_WIDE                          /* If we're in Unicode,  assume */
            else if( ch <= 0xff)          /* the incoming text doesn't need */
            {                             /* code-page translation          */
               char c = (char)ch;
               wchar_t z;

               mbtowc( &z, &c, 1);
               ch = (chtype)z;
            }
            assert( "We should never get here");
#endif
            buff[olen] = (wchar_t)ch;
            lpDx[olen] = PDC_cxChar;
#ifdef PDC_WIDE
            if( ch != MAX_UNICODE)
               olen++;
            else if( olen)          /* prev char is double-width */
               lpDx[olen - 1] = 2 * PDC_cxChar;
#else
            olen++;
#endif
        }
        lpDx[olen] = PDC_cxChar;
        if( color != curr_color || ((prev_ch ^ *srcp) & (A_REVERSE | A_BLINK | A_BOLD | A_DIM)))
        {
            COLORREF background_rgb;

            PDC_get_rgb_values( *srcp, &foreground_rgb, &background_rgb);
            curr_color = color;
            SetTextColor( hdc, (COLORREF)foreground_rgb);
            SetBkColor( hdc, (COLORREF)background_rgb);
        }
        if( !(SP->termattrs & A_BLINK) && (*srcp & A_BLINK))
            new_font_attrib &= ~A_BLINK;
#ifdef USE_FALLBACK_FONT
        if( !in_font)                  /* flag to indicate use of */
            new_font_attrib |= 1;      /* fallback font           */
#endif
        if( new_font_attrib != font_attrib)
        {
            HFONT hFont;
            int idx = 0;

            font_attrib = new_font_attrib;
            if( font_attrib & A_BOLD & SP->termattrs)
                idx |= 1;
            if( font_attrib & A_ITALIC)
                idx |= 2;
            if( font_attrib & 1)  /* use Unifont or other fallback font */
                idx |= 4;
            if( !hFonts[idx])
                hFonts[idx] = PDC_get_font_handle( idx);
            hFont = SelectObject( hdc, hFonts[idx]);
            if( !hOldFont)
                hOldFont = hFont;
        }
        prev_ch = *srcp;
        clip_rect.left = x * PDC_cxChar;
        clip_rect.top = lineno * PDC_cyChar;
        clip_rect.right = clip_rect.left + i * PDC_cxChar;
        clip_rect.bottom = clip_rect.top + PDC_cyChar;
        ExtTextOutW( hdc, clip_rect.left, clip_rect.top,
                           ETO_CLIPPED | ETO_OPAQUE, &clip_rect,
                           buff, olen, (olen > 1 ? lpDx : NULL));
#ifdef A_OVERLINE
        if( *srcp & (A_UNDERLINE | A_RIGHTLINE | A_LEFTLINE | A_OVERLINE | A_STRIKEOUT))
#else
        if( *srcp & (A_UNDERLINE | A_RIGHTLINE | A_LEFTLINE))
#endif
        {
            const int y1 = clip_rect.top;
            const int y2 = clip_rect.bottom - 1;
            const int x1 = clip_rect.left;
            const int x2 = clip_rect.right;
            int j;
            const COLORREF rgb = (SP->line_color == -1 ?
                             foreground_rgb : PDC_palette[SP->line_color]);
            const HPEN pen = CreatePen( PS_SOLID, 1, (COLORREF)rgb);
            const HPEN old_pen = SelectObject( hdc, pen);

            if( *srcp & A_UNDERLINE)
            {
                MoveToEx( hdc, x1, y2, NULL);
                LineTo(   hdc, x2, y2);
            }
#ifdef A_OVERLINE
            if( *srcp & A_OVERLINE)
            {
                MoveToEx( hdc, x1, y1, NULL);
                LineTo(   hdc, x2, y1);
            }
            if( *srcp & A_STRIKEOUT)
            {
                MoveToEx( hdc, x1, (y1 + y2) / 2, NULL);
                LineTo(   hdc, x2, (y1 + y2) / 2);
            }
#endif
            if( *srcp & A_RIGHTLINE)
                for( j = 0; j < i; j++)
                {
                    MoveToEx( hdc, x2 - j * PDC_cxChar - 1, y1, NULL);
                    LineTo(   hdc, x2 - j * PDC_cxChar - 1, y2);
                }
            if( *srcp & A_LEFTLINE)
                for( j = 0; j < i; j++)
                {
                    MoveToEx( hdc, x1 + j * PDC_cxChar, y1, NULL);
                    LineTo(   hdc, x1 + j * PDC_cxChar, y2);
                }
            SelectObject( hdc, old_pen);
            DeleteObject( pen);
        }
        len -= i;
        x += i;
        srcp += i;
    }
    SelectObject( hdc, hOldFont);
               /* ...did we step on the cursor?  If so,  redraw it: */
    if( cursor_overwritten)
        redraw_cursor( hdc);
}

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    if( !srcp)    /* just freeing up fonts */
        PDC_transform_line_given_hdc( 0, 0, 0, 0, NULL);
    else
    {
        const HDC hdc = GetDC( PDC_hWnd) ;

        PDC_transform_line_given_hdc( hdc, lineno, x, len, srcp);
        ReleaseDC( PDC_hWnd, hdc);
    }
}

void PDC_doupdate(void)
{
}

    /* This function 'intensifies' a color by shifting it toward white. */
    /* It used to average the input color with white.  Then it did a    */
    /* weighted average:  2/3 of the input color,  1/3 white,   for a   */
    /* lower "intensification" level.                                   */
    /*    Then Mark Hessling suggested that the output level should     */
    /* remap zero to 85 (= 255 / 3, so one-third intensity),  and input */
    /* of 192 or greater should be remapped to 255 (full intensity).    */
    /* Assuming we want a linear response between zero and 192,  that   */
    /* leads to output = 85 + input * (255-85)/192.                     */
    /*    This should lead to proper handling of bold text in legacy    */
    /* apps,  where "bold" means "high intensity".                      */

static COLORREF intensified_color( COLORREF ival)
{
    int rgb, i;
    COLORREF oval = 0;

    for( i = 0; i < 3; i++, ival >>= 8)
    {
        rgb = (int)( ival & 0xff);
        if( rgb >= 192)
            rgb = 255;
        else
            rgb = 85 + rgb * (255 - 85) / 192;
        oval |= ((COLORREF)rgb << (i * 8));
    }
    return( oval);
}

   /* For use in adjusting colors for A_DIMmed characters.  Just */
   /* knocks down the intensity of R, G, and B by 1/3.           */

static COLORREF dimmed_color( COLORREF ival)
{
    unsigned i;
    COLORREF oval = 0;

    for( i = 0; i < 3; i++, ival >>= 8)
    {
        unsigned rgb = (unsigned)( ival & 0xff);

        rgb -= (rgb / 3);
        oval |= ((COLORREF)rgb << (i * 8));
    }
    return( oval);
}

static void PDC_get_rgb_values( const chtype srcp,
            COLORREF *foreground_rgb, COLORREF *background_rgb)
{
    const int color = (int)(( srcp & A_COLOR) >> PDC_COLOR_SHIFT);
    bool reverse_colors = ((srcp & A_REVERSE) ? TRUE : FALSE);
    bool intensify_backgnd = FALSE;
    bool default_foreground = FALSE, default_background = FALSE;

    *foreground_rgb = (COLORREF)-1;
    *background_rgb = (COLORREF)-1;

    {
        short foreground_index, background_index;

        pair_content( color, &foreground_index, &background_index);
        if( foreground_index < 0 && SP->orig_attr)
            default_foreground = TRUE;
        else
            *foreground_rgb = PDC_palette[foreground_index];
        if( background_index < 0 && SP->orig_attr)
            default_background = TRUE;
        else
            *background_rgb = PDC_palette[background_index];
    }

    if( srcp & A_BLINK)
    {
        if( !(SP->termattrs & A_BLINK))   /* convert 'blinking' to 'bold' */
            intensify_backgnd = TRUE;
        else if( PDC_blink_state)
            reverse_colors ^= 1;
    }
    if( reverse_colors)
    {
        const COLORREF temp = *foreground_rgb;

        *foreground_rgb = *background_rgb;
        *background_rgb = temp;
    }

    if( srcp & A_BOLD & ~SP->termattrs)
        *foreground_rgb = intensified_color( *foreground_rgb);
    if( intensify_backgnd)
        *background_rgb = intensified_color( *background_rgb);
    if( srcp & A_DIM)
    {
        *foreground_rgb = dimmed_color( *foreground_rgb);
        *background_rgb = dimmed_color( *background_rgb);
    }
    if( default_foreground)
        *foreground_rgb = (COLORREF)-1;
    if( default_background)
        *background_rgb = (COLORREF)-1;
}
