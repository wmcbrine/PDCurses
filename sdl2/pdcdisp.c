/* Public Domain Curses */

#include "pdcsdl.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef CHTYPE_LONG
# ifdef PDC_WIDE
#  define USE_UNICODE_ACS_CHARS 1
# else
#  define USE_UNICODE_ACS_CHARS 0
# endif
# include "acs_defs.h"
#endif

Uint32 pdc_lastupdate = 0;
int PDC_really_blinking = FALSE;
static bool PDC_blink_state = FALSE;

#define MAXRECT 200     /* maximum number of rects to queue up before
                           an update is forced; the number was chosen
                           arbitrarily */

static SDL_Rect uprect[MAXRECT];       /* table of rects to update */
static chtype oldch = (chtype)(-1);    /* current attribute */
static int rectcount = 0;              /* index into uprect */
static SDL_Color foreground_rgb, background_rgb;  /* current foreground, background */

/* do the real updates on a delay */

void PDC_update_rects(void)
{
    if (rectcount)
    {
        /* if the maximum number of rects has been reached, we're
           probably better off doing a full screen update */

        if (rectcount == MAXRECT)
            SDL_UpdateWindowSurface(pdc_window);
        else
            SDL_UpdateWindowSurfaceRects(pdc_window, uprect, rectcount);

        pdc_lastupdate = SDL_GetTicks();
        rectcount = 0;
    }
}

   /* PDC_get_rgb_values(), extract_packed_rgb(), intensified_component(), */
   /* intensified_color(),  and dimmed_color() each exist in x11/x11.c,    */
   /* win32a/pdcdisp.c,  and sdl2/pdcdisp.c in forms slightly modified for */
   /* each platform.  But they all look pretty much alike.  */

#if defined( CHTYPE_LONG) && CHTYPE_LONG >= 2
            /* PDCurses stores RGBs in fifteen bits,  five bits each */
            /* for red, green, blue.  A Pixel uses eight bits per    */
            /* channel.  Hence the following.                        */
static SDL_Color extract_packed_rgb( const chtype color)
{
    SDL_Color rval;

    rval.r = (Uint8)( (color << 3) & 0xf8);
    rval.g = (Uint8)( (color >> 2) & 0xf8);
    rval.b = (Uint8)( (color >> 7) & 0xf8);
    rval.a = (Uint8)255;
    return rval;
}
#endif

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

static Uint8 intensified_component( Uint8 component)
{
   if( component >= 192)
      component = 255;
   else
      component = 85 + component * (255 - 85) / 192;
   return( component);
}

static SDL_Color intensified_color( SDL_Color ival)
{
    SDL_Color oval;

    oval.r = intensified_component( ival.r);
    oval.g = intensified_component( ival.g);
    oval.b = intensified_component( ival.b);
    return( oval);
}

   /* For use in adjusting colors for A_DIMmed characters.  Just */
   /* knocks down the intensity of R, G, and B by 1/3.           */

static SDL_Color dimmed_color( SDL_Color ival)
{
    ival.r -= ival.r / 3;
    ival.g -= ival.g / 3;
    ival.b -= ival.b / 3;
    return( ival);
}

void PDC_get_rgb_values( const chtype srcp,
            SDL_Color *fore_rgb, SDL_Color *back_rgb)
{
    bool reverse_colors = ((srcp & A_REVERSE) ? TRUE : FALSE);
    bool intensify_backgnd = FALSE;

#if defined( CHTYPE_LONG) && CHTYPE_LONG >= 2
    if( srcp & A_RGB_COLOR)
    {
        /* Extract RGB from 30 bits of the color field */
        *back_rgb = extract_packed_rgb( srcp >> PDC_COLOR_SHIFT);
        *fore_rgb = extract_packed_rgb( srcp >> (PDC_COLOR_SHIFT + 15));
    }
    else
#endif
    {
        short foreground_index, background_index;

        PDC_pair_content( PAIR_NUMBER( srcp), &foreground_index, &background_index);
        *fore_rgb = pdc_color[foreground_index];
        if( background_index >= 0)
           *back_rgb = pdc_color[background_index];
    }

    if( srcp & A_BLINK)
    {
        if( !PDC_really_blinking)   /* convert 'blinking' to 'bold' */
            intensify_backgnd = TRUE;
        else if( PDC_blink_state)
            reverse_colors = !reverse_colors;
    }
    if( reverse_colors)
    {
        const SDL_Color temp = *fore_rgb;

        *fore_rgb = *back_rgb;
        *back_rgb = temp;
    }

    if( srcp & A_BOLD)
        *fore_rgb = intensified_color( *fore_rgb);
    if( intensify_backgnd)
        *back_rgb = intensified_color( *back_rgb);
    if( srcp & A_DIM)
    {
        *fore_rgb = dimmed_color( *fore_rgb);
        *back_rgb = dimmed_color( *back_rgb);
    }
}

#define same_colors( A, B) ((A).r==(B).r && (A).g==(B).g && (A).b==(B).b)

/* set the font colors to match the chtype's attribute */

static void _set_attr(chtype ch)
{
    ch &= (A_COLOR|A_BOLD|A_BLINK|A_REVERSE | A_DIM | A_RGB_COLOR);

    if (oldch != ch)
    {
        SDL_Color fore_rgb, back_rgb;

        if (SP->mono)
            return;
        PDC_get_rgb_values( ch, &fore_rgb, &back_rgb);

        if( !same_colors( fore_rgb, foreground_rgb))
        {
#ifndef PDC_WIDE
            SDL_SetPaletteColors(pdc_font->format->palette,
                                 &fore_rgb, pdc_flastc, 1);
#endif
            foreground_rgb = fore_rgb;
        }

        if( !same_colors( back_rgb, background_rgb))
        {
#ifndef PDC_WIDE
            SDL_SetPaletteColors(pdc_font->format->palette,
                                     &back_rgb, 0, 1);
#endif
            background_rgb = back_rgb;
        }

        oldch = ch;
    }
}

/* draw a cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    SDL_Rect src, dest;
    chtype ch;
    int oldrow, oldcol;
    const int cursor_style =
             (SP->visibility >> (PDC_blink_state ? 8 : 0)) & 0xff;
#ifdef PDC_WIDE
    Uint16 chstr[2] = {0, 0};
#endif


    PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
             row, col, SP->cursrow, SP->curscol));

    if (SP->mono)
        return;

    oldrow = SP->cursrow;
    oldcol = SP->curscol;

    /* clear the old cursor */

    PDC_transform_line(oldrow, oldcol, 1, curscr->_y[oldrow] + oldcol);

    if (!cursor_style)
        return;

    /* draw a new cursor by overprinting the existing character in
       reverse, either the full cell (when cursor_style == 2) or the
       lowest quarter of it (when cursor_style == 1) */

    ch = curscr->_y[row][col] ^ A_REVERSE;

    _set_attr(ch);

#ifdef CHTYPE_LONG
    if (ch & A_ALTCHARSET && !(ch & 0xff80))
        ch = acs_map[ch & 0x7f];
#endif
    src.h = (cursor_style == 1) ? pdc_fheight >> 2 : pdc_fheight;
    src.w = pdc_fwidth;

    dest.y = (row + 1) * pdc_fheight - src.h + pdc_yoffset;
    dest.x = col * pdc_fwidth + pdc_xoffset;

#ifdef PDC_WIDE
    chstr[0] = ch & A_CHARTEXT;

    pdc_font = TTF_RenderUNICODE_Solid( pdc_ttffont, chstr, foreground_rgb);
    if (pdc_font)
    {
        dest.h = src.h;
        dest.w = src.w;
        src.x = 0;
        src.y = 0;
        SDL_SetColorKey( pdc_font, SDL_FALSE, 0);
        SDL_SetPaletteColors(pdc_font->format->palette, &background_rgb, 0, 1);
        SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
        SDL_FreeSurface(pdc_font);
        pdc_font = NULL;
    }
#else

    src.x = (ch & 0xff) % 32 * pdc_fwidth;
    src.y = (ch & 0xff) / 32 * pdc_fheight + (pdc_fheight - src.h);

    SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
#endif

    if (oldrow != row || oldcol != col)
    {
        if (rectcount == MAXRECT)
            PDC_update_rects();

        uprect[rectcount++] = dest;
    }
}

/* handle the A_*LINE attributes */

static void _highlight( SDL_Rect *dest, chtype ch)
{
    SDL_Color col;
    Uint32 pixel_value;

    if (SP->mono)
        return;

    if (SP->line_color == -1)
        col = foreground_rgb;
    else
        col = pdc_color[SP->line_color];
    pixel_value = SDL_MapRGB( pdc_screen->format, col.r, col.g, col.b);
    if (ch & (A_UNDERLINE | A_OVERLINE | A_STRIKEOUT))
    {
        dest->h = 1;

        if (ch & A_OVERLINE)
            SDL_FillRect(pdc_screen, dest, pixel_value);

        if (ch & A_UNDERLINE)
        {
            dest->y += pdc_fheight - 1;
            SDL_FillRect(pdc_screen, dest, pixel_value);
            dest->y -= pdc_fheight - 1;
        }

        if (ch & A_STRIKEOUT)
        {
            dest->y += pdc_fheight / 2;
            SDL_FillRect(pdc_screen, dest, pixel_value);
            dest->y -= pdc_fheight / 2;
        }

        dest->h = pdc_fheight;
    }

    if (ch & (A_LEFTLINE|A_RIGHTLINE))
    {

        dest->w = 1;

        if (ch & A_LEFTLINE)
            SDL_FillRect(pdc_screen, dest, pixel_value);

        if (ch & A_RIGHTLINE)
        {
            dest->x += pdc_fwidth - 1;
            SDL_FillRect(pdc_screen, dest, pixel_value);
            dest->x -= pdc_fwidth - 1;
        }

        dest->w = pdc_fwidth;
    }
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    SDL_Rect src, dest, lastrect;
    int j;
#ifdef PDC_WIDE
    Uint16 chstr[2] = {0, 0};
#endif

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    if (rectcount == MAXRECT)
        PDC_update_rects();

    src.h = pdc_fheight;
    src.w = pdc_fwidth;

    dest.y = pdc_fheight * lineno + pdc_yoffset;
    dest.x = pdc_fwidth * x + pdc_xoffset;
    dest.h = pdc_fheight;
    dest.w = pdc_fwidth * len;

    /* if the previous rect was just above this one, with the same width
       and horizontal position, then merge the new one with it instead
       of adding a new entry */

    if (rectcount)
        lastrect = uprect[rectcount - 1];

    if (rectcount && lastrect.x == dest.x && lastrect.w == dest.w)
    {
        if (lastrect.y + lastrect.h == dest.y)
            uprect[rectcount - 1].h = lastrect.h + pdc_fheight;
        else
            if (lastrect.y != dest.y)
                uprect[rectcount++] = dest;
    }
    else
        uprect[rectcount++] = dest;

    dest.w = pdc_fwidth;

#ifdef PDC_WIDE
    src.x = 0;
    src.y = 0;
#endif

    for (j = 0; j < len; j++)
    {
        chtype ch = srcp[j];

        _set_attr(ch);

#ifdef CHTYPE_LONG
        if (ch & A_ALTCHARSET && !(ch & 0xff80))
            ch = (ch & (A_ATTRIBUTES ^ A_ALTCHARSET)) | acs_map[ch & 0x7f];
#endif

#ifdef PDC_WIDE
        chstr[0] = ch & A_CHARTEXT;
        pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr,
                                           foreground_rgb);

        if (pdc_font)
        {
            SDL_SetColorKey( pdc_font, SDL_FALSE, 0);
            SDL_SetPaletteColors(pdc_font->format->palette,
                                     &background_rgb, 0, 1);
            SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
            SDL_FreeSurface(pdc_font);
            pdc_font = NULL;
        }
#else
        src.x = (ch & 0xff) % 32 * pdc_fwidth;
        src.y = (ch & 0xff) / 32 * pdc_fheight;

        SDL_LowerBlit(pdc_font, &src, pdc_screen, &dest);
#endif

        if (ch & (A_UNDERLINE|A_LEFTLINE|A_RIGHTLINE|A_OVERLINE|A_STRIKEOUT))
            _highlight( &dest, ch);

        dest.x += pdc_fwidth;
    }
}


/* If PDC_really_blinking is 'true',  we need to redraw all text with
the A_BLINK attribute set every 500 milliseconds.  (We also need to redraw
it when blinking is turned off.)  If the line-drawing color changes,  we
need to redraw all under/over/left/right-lined text,  and strikeout text.
This function,  therefore,  redraws all text of a specified attribute. */

static void redraw_text_with_attribute( const chtype attribute)
{
    int i, j, k;

    for( i = 0; i < SP->lines; i++)
    {
        const chtype *srcp = curscr->_y[i];

        for( j = 0; j < SP->cols; j++)
            if( srcp[j] & attribute)
            {
                k = j + 1;
                while( k < SP->cols && (srcp[k] & attribute))
                    k++;
                PDC_transform_line( i, j, k - j, srcp + j);
                j = k;
            }
    }
}

/* Twice a second,  we blink the cursor,  blink text if it's "really"
blinking (or draw it as 'standout' if it was previously blinking),  and
if the line color has changed,  we redraw all the lined text. */

void PDC_twice_a_second( void)
{
    static int previously_really_blinking;
    static int prev_line_color = -1;

    PDC_blink_state = !PDC_blink_state;
    PDC_gotoyx(SP->cursrow, SP->curscol);
    if( PDC_really_blinking || previously_really_blinking)
        redraw_text_with_attribute( A_BLINK);
    previously_really_blinking = PDC_really_blinking;
    if( prev_line_color != SP->line_color)
    {
        redraw_text_with_attribute( A_UNDERLINE | A_OVERLINE | A_STRIKEOUT
                     | A_LEFTLINE | A_RIGHTLINE);
        prev_line_color = SP->line_color;
    }
}
