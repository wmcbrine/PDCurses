/* Public Domain Curses */

#include "pdcsdl.h"

#include <stdlib.h>
#include <string.h>

#ifdef CHTYPE_LONG

#ifdef PDC_WIDE
   #define USE_UNICODE_ACS_CHARS 1
#else
   #define USE_UNICODE_ACS_CHARS 0
#endif

#include "acs_defs.h"

#endif

Uint32 pdc_lastupdate = 0;

#define MAXRECT 200     /* maximum number of rects to queue up before
                           an update is forced; the number was chosen
                           arbitrarily */

static SDL_Rect uprect[MAXRECT];       /* table of rects to update */
static chtype oldch = (chtype)(-1);    /* current attribute */
static int rectcount = 0;              /* index into uprect */
static short foregr = -2, backgr = -2; /* current foreground, background */

/* do the real updates on a delay */

void PDC_update_rects(void)
{
    if (rectcount)
    {
        /* if the maximum number of rects has been reached, we're
           probably better off doing a full screen update */

        if (rectcount == MAXRECT)
            SDL_Flip(pdc_screen);
        else
            SDL_UpdateRects(pdc_screen, rectcount, uprect);

        pdc_lastupdate = SDL_GetTicks();
        rectcount = 0;
    }
}

/* set the font colors to match the chtype's attribute */

static void _set_attr(chtype ch)
{
    ch &= (A_COLOR|A_BOLD|A_BLINK|A_REVERSE);

    if (oldch != ch)
    {
        short newfg, newbg;

        if (SP->mono)
            return;

        PDC_pair_content(PAIR_NUMBER(ch), &newfg, &newbg);

        newfg |= (ch & A_BOLD) ? 8 : 0;
        newbg |= (ch & A_BLINK) ? 8 : 0;

        if (ch & A_REVERSE)
        {
            short tmp = newfg;
            newfg = newbg;
            newbg = tmp;
        }

        if (newfg != foregr)
        {
#ifndef PDC_WIDE
            SDL_SetPalette(pdc_font, SDL_LOGPAL,
                           pdc_color + newfg, pdc_flastc, 1);
#endif
            foregr = newfg;
        }

        if (newbg != backgr)
        {
#ifndef PDC_WIDE
            if (newbg == -1)
                SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 0);
            else
            {
                if (backgr == -1)
                    SDL_SetColorKey(pdc_font, 0, 0);

                SDL_SetPalette(pdc_font, SDL_LOGPAL,
                               pdc_color + newbg, 0, 1);
            }
#endif
            backgr = newbg;
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

    if (!SP->visibility)
        return;

    /* draw a new cursor by overprinting the existing character in
       reverse, either the full cell (when visibility == 2) or the
       lowest quarter of it (when visibility == 1) */

    ch = curscr->_y[row][col] ^ A_REVERSE;

    _set_attr(ch);

#ifdef CHTYPE_LONG
    if (ch & A_ALTCHARSET && !(ch & 0xff80))
        ch = acs_map[ch & 0x7f];
#endif
    src.h = (SP->visibility == 1) ? pdc_fheight >> 2 : pdc_fheight;
    src.w = pdc_fwidth;

    dest.y = (row + 1) * pdc_fheight - src.h + pdc_yoffset;
    dest.x = col * pdc_fwidth + pdc_xoffset;

#ifdef PDC_WIDE
    chstr[0] = ch & A_CHARTEXT;

    pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr, pdc_color[foregr]);
    if (pdc_font)
    {
        dest.h = src.h;
        dest.w = src.w;
        src.x = 0;
        src.y = 0;
        SDL_SetColorKey(pdc_font, 0, 0);
        SDL_SetPalette(pdc_font, SDL_LOGPAL, pdc_color + backgr, 0, 1);
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

static void _highlight(SDL_Rect *src, SDL_Rect *dest, chtype ch)
{
    short col = SP->line_color;
#ifdef PDC_WIDE
    Uint16 chstr[2] = {'_', 0};
#endif

    if (SP->mono)
        return;

    if (ch & (A_UNDERLINE | A_OVERLINE | A_STRIKEOUT))
    {
#ifdef PDC_WIDE
        if (col == -1)
            col = foregr;

        pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr, pdc_color[col]);
        if (pdc_font)
        {
           src->x = 0;
           src->y = 0;

           if (backgr != -1)
               SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 0);

           if( ch & A_UNDERLINE)
                SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
           if( ch & A_OVERLINE)
           {
                dest->y -= pdc_fheight - 1;
                SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
                dest->y += pdc_fheight - 1;
           }
           if( ch & A_STRIKEOUT)
           {
                dest->y -= pdc_fheight / 2;
                SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
                dest->y += pdc_fheight / 2;
           }
           SDL_FreeSurface(pdc_font);
           pdc_font = NULL;
        }
#else
        if (col != -1)
            SDL_SetPalette(pdc_font, SDL_LOGPAL,
                           pdc_color + col, pdc_flastc, 1);

        src->x = '_' % 32 * pdc_fwidth;
        src->y = '_' / 32 * pdc_fheight;

        if (backgr != -1)
            SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 0);

        if( ch & A_UNDERLINE)
            SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
        if( ch & A_OVERLINE)
        {
           dest->y -= pdc_fheight - 1;
           SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
           dest->y += pdc_fheight - 1;
        }
        if( ch & A_STRIKEOUT)
        {
           dest->y -= pdc_fheight / 2;
           SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
           dest->y += pdc_fheight / 2;
        }

        if (backgr != -1)
            SDL_SetColorKey(pdc_font, 0, 0);

        if (col != -1)
            SDL_SetPalette(pdc_font, SDL_LOGPAL,
                           pdc_color + foregr, pdc_flastc, 1);
#endif
    }

    if (ch & (A_LEFTLINE|A_RIGHTLINE))
    {
        if (col == -1)
            col = foregr;

        dest->w = 1;

        if (ch & A_LEFTLINE)
            SDL_FillRect(pdc_screen, dest, pdc_mapped[col]);

        if (ch & A_RIGHTLINE)
        {
            dest->x += pdc_fwidth - 1;
            SDL_FillRect(pdc_screen, dest, pdc_mapped[col]);
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
        if (backgr == -1)
            SDL_LowerBlit(pdc_tileback, &dest, pdc_screen, &dest);

#ifdef PDC_WIDE
        chstr[0] = ch & A_CHARTEXT;
        pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr,
                                           pdc_color[foregr]);

        if (pdc_font)
        {
            if (backgr != -1)
            {
                SDL_SetColorKey(pdc_font, 0, 0);
                SDL_SetPalette(pdc_font, SDL_LOGPAL,
                               pdc_color + backgr, 0, 1);
            }
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
            _highlight(&src, &dest, ch);

        dest.x += pdc_fwidth;
    }
}
