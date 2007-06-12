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

#include <stdlib.h>
#include <string.h>

RCSID("$Id: pdcdisp.c,v 1.3 2007/06/12 07:44:13 wmcbrine Exp $");

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

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
	chtype ch;

	PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
		row, col, SP->cursrow, SP->curscol));

	PDC_transform_line(SP->cursrow, SP->curscol, 1, 
		curscr->_y[SP->cursrow] + SP->curscol);

	if (!SP->visibility)
		return;

	ch = curscr->_y[row][col] & A_ATTRIBUTES;

	ch |= (SP->visibility == 1) ? '_' : acs_map['0'];

	PDC_transform_line(row, col, 1, &ch);
}

void _setattr(chtype ch)
{
	SDL_Color attr[2];
	short fg, bg;

	pair_content(PAIR_NUMBER(ch), &fg, &bg);

	fg |= (ch & A_BOLD) ? 8 : 0;
	bg |= (ch & A_BLINK) ? 8 : 0;

	if (ch & A_REVERSE)
	{
		short tmp = fg;
		fg = bg;
		bg = tmp;
	}

	attr[0].r = pdc_color[fg].r;
	attr[0].g = pdc_color[fg].g;
	attr[0].b = pdc_color[fg].b;

	attr[1].r = pdc_color[bg].r;
	attr[1].g = pdc_color[bg].g;
	attr[1].b = pdc_color[bg].b;

	SDL_SetColors(pdc_font, attr, 0, 2);
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
	SDL_Rect src, dest;
	chtype oldch;
	int j;

	PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

	src.w = pdc_fwidth;
	src.h = pdc_fheight;
	dest.y = pdc_fheight * lineno;

	for (j = 0; j < len; j++)
	{
		chtype ch = srcp[j];

		if (!j || ((ch & A_ATTRIBUTES) != oldch))
		{
			_setattr(ch);
			oldch = ch & A_ATTRIBUTES;
		}

#ifdef CHTYPE_LONG
		if (ch & A_ALTCHARSET && !(ch & 0xff80))
			ch = acs_map[ch & 0x7f];
#endif
		dest.x = (x + j) * pdc_fwidth;
		src.x = (ch & 0xff) % 32 * pdc_fwidth;
		src.y = (ch & 0xff) / 32 * pdc_fheight;

		SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
	}

	SDL_UpdateRect(pdc_screen, x * pdc_fwidth, lineno * pdc_fheight,
		len * pdc_fwidth, pdc_fheight);
}
