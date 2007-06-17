/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcsdl.h"

RCSID("$Id: pdcdisp.c,v 1.15 2007/06/17 20:37:29 wmcbrine Exp $")

#include <stdlib.h>
#include <string.h>

#ifdef CHTYPE_LONG

# define A(x) ((chtype)x | A_ALTCHARSET)

chtype acs_map[128] =
{
	A(0), A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9),
	A(10), A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18),
	A(19), A(20), A(21), A(22), A(23), A(24), A(25), A(26), A(27),
	A(28), A(29), A(30), A(31), ' ', '!', '"', '#', '$', '%', '&',
	'\'', '(', ')', '*',

	A(0x1a), A(0x1b), A(0x18), A(0x19),

	'/',

	0xdb,

	'1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
	'>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

	A(0x04), 0xb1,

	'b', 'c', 'd', 'e',

	0xf8, 0xf1, 0xb0, A(0x0f), 0xd9, 0xbf, 0xda, 0xc0, 0xc5, 0x2d,
	0x2d, 0xc4, 0x2d, 0x5f, 0xc3, 0xb4, 0xc1, 0xc2, 0xb3, 0xf3,
	0xf2, 0xe3, 0xd8, 0x9c, 0xf9,

	A(127)
};

# undef A

#endif

#define MAXRECT 200

static SDL_Rect uprect[MAXRECT];
static int rectcount = 0;
static short foregr, backgr;

/* do the real updates on a delay */

void PDC_update_rects(void)
{
	if (rectcount)
	{
		SDL_UpdateRects(pdc_screen, rectcount, uprect);
		rectcount = 0;
	}
}

/* set the font colors to match the chtype's attribute */

static void _set_attr(chtype ch)
{
	SDL_Color attr[2];

	pair_content(PAIR_NUMBER(ch), &foregr, &backgr);

	foregr |= (ch & A_BOLD) ? 8 : 0;
	backgr |= (ch & A_BLINK) ? 8 : 0;

	if (ch & A_REVERSE)
	{
		attr[0] = pdc_color[backgr];
		attr[1] = pdc_color[foregr];
	}
	else
	{
		attr[0] = pdc_color[foregr];
		attr[1] = pdc_color[backgr];
	}

	SDL_SetColors(pdc_font, attr, 0, 2);
}

/* draw a cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
	SDL_Rect src, dest;
	chtype ch;

	PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
		row, col, SP->cursrow, SP->curscol));

	/* clear the old cursor */

	PDC_transform_line(SP->cursrow, SP->curscol, 1, 
		curscr->_y[SP->cursrow] + SP->curscol);

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

	src.x = (ch & 0xff) % 32 * pdc_fwidth;
	src.y = (ch & 0xff) / 32 * pdc_fheight + (pdc_fheight - src.h);

	SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
	SDL_UpdateRect(pdc_screen, dest.x, dest.y, src.w, src.h);
}

/* handle the A_*LINE attributes */

static void _highlight(SDL_Rect *src, SDL_Rect *dest, chtype oldch)
{
	short col = SP->line_color;

	if (oldch & A_UNDERLINE)
	{
		if (col != -1)
			SDL_SetColors(pdc_font, pdc_color + col, 0, 1);

		src->x = '_' % 32 * pdc_fwidth;
		src->y = '_' / 32 * pdc_fheight;

		SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 1);
		SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
		SDL_SetColorKey(pdc_font, 0, 0);

		if (col != -1)
			_set_attr(oldch);
	}

	if (oldch & (A_LEFTLINE|A_RIGHTLINE))
	{
		if (col == -1)
			col = (oldch & A_REVERSE) ? backgr : foregr;

		dest->w = 1;

		if (oldch & A_LEFTLINE)
			SDL_FillRect(pdc_screen, dest, pdc_mapped[col]);

		if (oldch & A_RIGHTLINE)
		{
			dest->x += pdc_fwidth - 1;
			SDL_FillRect(pdc_screen, dest, pdc_mapped[col]);
		}
	}
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
	SDL_Rect src, dest, lastrect;
	chtype oldch = 0;
	int j;

	PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

	if (rectcount == MAXRECT)
		PDC_update_rects();

	src.h = pdc_fheight;
	src.w = pdc_fwidth;

	dest.y = pdc_fheight * lineno + pdc_yoffset;
	dest.x = pdc_fwidth * x + pdc_xoffset;
	dest.h = pdc_fheight;
	dest.w = pdc_fwidth * len;

	/* merge rects that are vertically adjacent and have the same
	   width -- mainly useful for full-screen updates */

	if (rectcount)
		lastrect = uprect[rectcount - 1];

	if (rectcount && (lastrect.x == dest.x) &&
	   (lastrect.w == dest.w) && (lastrect.y == dest.y - pdc_fheight))
		uprect[rectcount - 1].h += pdc_fheight;
	else
		uprect[rectcount++] = dest;

	for (j = 0; j < len; j++)
	{
		chtype ch = srcp[j];

		if (!j || ((ch & (A_ATTRIBUTES ^ A_ALTCHARSET)) != oldch))
		{
			_set_attr(ch);
			oldch = ch & (A_ATTRIBUTES ^ A_ALTCHARSET);
		}

#ifdef CHTYPE_LONG
		if (ch & A_ALTCHARSET && !(ch & 0xff80))
			ch = acs_map[ch & 0x7f];
#endif
		src.x = (ch & 0xff) % 32 * pdc_fwidth;
		src.y = (ch & 0xff) / 32 * pdc_fheight;

		SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);

		if (oldch & (A_UNDERLINE|A_LEFTLINE|A_RIGHTLINE))
			_highlight(&src, &dest, oldch);

		dest.x += pdc_fwidth;
	}
}
