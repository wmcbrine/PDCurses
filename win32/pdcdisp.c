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
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#define CURSES_LIBRARY 1
#define INCLUDE_WINDOWS_H
#include <curses.h>
#include <stdlib.h>
#include <string.h>

extern HANDLE hConOut;
extern unsigned char atrtab[MAX_ATRTAB];

RCSID("$Id: pdcdisp.c,v 1.32 2006/07/30 03:55:46 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_gotoyx() - position hardware cursor at (y, x)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the screen. 
	We don't optimize here -- on a PC, it takes more time to 
	optimize than to do things directly.

  Portability:
	PDCurses  void PDC_gotoyx(int row, int col);

**man-end****************************************************************/

void PDC_gotoyx(int row, int col)
{
	COORD coord;

	PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
		row, col, SP->cursrow, SP->curscol));

	coord.X = col;
	coord.Y = row;

	SetConsoleCursorPosition(hConOut, coord);
}

/*man-start**************************************************************

  PDC_transform_line()  - display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  Portability:
	PDCurses  void PDC_transform_line(int lineno, int x, int len, 
					  const chtype *srcp);

**man-end****************************************************************/

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
	CHAR_INFO ci[512];
	int j;
	COORD bufSize, bufPos;
	SMALL_RECT sr;

	PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

	bufPos.X = bufPos.Y = 0;

	bufSize.X = len;
	bufSize.Y = 1;

	sr.Top = lineno;
	sr.Bottom = lineno;
	sr.Left = x;
	sr.Right = x + len - 1;

	for (j = 0; j < len; j++)
	{
#ifdef UNICODE
		ci[j].Char.UnicodeChar = srcp[j] & A_CHARTEXT;
#else
		ci[j].Char.AsciiChar = srcp[j] & 0xff;
#endif
		ci[j].Attributes = chtype_attr(srcp[j]);
	}

	WriteConsoleOutput(hConOut, ci, bufSize, bufPos, &sr);
}
