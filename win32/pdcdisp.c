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

#ifdef PDCDEBUG
const char *rcsid_PDCdisp =
	"$Id: pdcdisp.c,v 1.15 2006/02/22 06:09:47 wmcbrine Exp $";
#endif

static CHAR_INFO ci[512];

/*man-start*********************************************************************

  PDC_clr_update()   - Updates the screen with a full redraw.

  PDCurses Description:
	Updates the screen by clearing it and then redraw it in its
	entirety. If SP->refrbrk is TRUE, and there is pending
	input characters, the update will be prematurely terminated.

  PDCurses Return Value:
	This routine returns ERR if it is unable to accomplish its task.

	The return value OK is returned if there were no errors.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_clr_update(WINDOW *s);

**man-end**********************************************************************/

int PDC_clr_update(WINDOW *s)
{
	int i, j;
	chtype *srcp;
	COORD bufSize, bufPos;
	SMALL_RECT sr;

	PDC_LOG(("PDC_clr_update() - called\n"));

	if (curscr == (WINDOW *)NULL)
		return ERR;
#if 0
	if (SP->full_redraw)
		PDC_clr_scrn(s);	/* clear physical screen */
#endif
	s->_clear = FALSE;
	bufPos.X = bufPos.Y = 0;
	bufSize.X = COLS;
	bufSize.Y = 1;
	sr.Left = 0;
	sr.Right = COLS - 1;

	for (i = 0; i < LINES; i++)	/* update physical screen */
	{
		if (s != curscr)
			memcpy(curscr->_y[i], s->_y[i], COLS * sizeof(chtype));

		srcp = s->_y[i];

		sr.Top = i;
		sr.Bottom = i;

		for (j = 0; j < COLS; j++)
		{
			ci[j].Char.AsciiChar = srcp[j] & A_CHARTEXT;
			ci[j].Attributes =
				(chtype_attr(srcp[j]) & 0xFF00) >> 8;
		}

		WriteConsoleOutput(hConOut, ci, bufSize, bufPos, &sr);

		if (SP->refrbrk && (SP->cbreak || SP->raw_inp)
		    && PDC_breakout())
			break;
	}

	return OK;
}

/*man-start*********************************************************************

  PDC_cursor_on() - Turns on the hardware cursor.

  PDCurses Description:
	Turns on the hardware cursor; does nothing if it is already on.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  Portability:
	PDCurses  int PDC_cursor_on(void);

**man-end**********************************************************************/

int PDC_cursor_on(void)
{
	CONSOLE_CURSOR_INFO cci;

	PDC_LOG(("PDC_cursor_on() - called\n"));

	if (!SP->visible_cursor)
	{
		SP->visible_cursor = TRUE;
		GetConsoleCursorInfo(hConOut, &cci);
		cci.bVisible = TRUE;
		SetConsoleCursorInfo(hConOut, &cci);
	}

	return OK;
}

/*man-start*********************************************************************

  PDC_cursor_off()   - Turns off the hardware cursor.

  PDCurses Description:
	Turns off the hardware cursor; does nothing if it is already off.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
	ERR will be returned if the hardware cursor can not be disabled.

  Portability:
	PDCurses  int PDC_cursor_off(void);

**man-end**********************************************************************/

int PDC_cursor_off(void)
{
	CONSOLE_CURSOR_INFO cci;

	PDC_LOG(("PDC_cursor_off() - called\n"));

	if (SP->visible_cursor)
	{
		SP->visible_cursor = FALSE;
		GetConsoleCursorInfo(hConOut, &cci);
		cci.bVisible = FALSE;
		SetConsoleCursorInfo(hConOut, &cci);
	}

	return OK;
}

/*man-start*********************************************************************

  PDC_fix_cursor()   - Fix the cursor start and stop scan lines (if necessary)

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will fix the cursor shape for certain video 
	adapters. Normally, the values used are correct, but some 
	adapters choke. The most noticable choke is on a monochrome 
	adapter.  The "correct" scan lines will result in the cursor 
	being set in the middle of the character cell, rather than at 
	the bottom.

	The passed flag indicates whether the cursor is visible or not.

	This only applies to the DOS platform.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_fix_cursor(int flag);

**man-end**********************************************************************/

int PDC_fix_cursor(int flag)
{
	return OK;
}

/*man-start*********************************************************************

  PDC_gotoxy() - position hardware cursor at (x, y)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the screen. 
	We don't optimize here -- on a PC, it takes more time to 
	optimize than to do things directly.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_gotoxy(int row, int col);

**man-end**********************************************************************/

int PDC_gotoxy(int row, int col)
{
	COORD coord;

	PDC_LOG(("PDC_gotoxy() - called: row %d col %d from row %d col %d\n",
		row, col, SP->cursrow, SP->curscol));

	coord.X = col;
	coord.Y = row;

	SetConsoleCursorPosition(hConOut, coord);

	return OK;
}

/*man-start*********************************************************************

  PDC_putc()   - Output a character in the current attribute.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs character 'chr' to screen in tty fashion. If a colour
	mode is active, the character is written with colour 'colour'.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_putc(chtype character, chtype color);

**man-end**********************************************************************/

int PDC_putc(chtype character, chtype color)
{
	int curRow, curCol;
	WORD buffer[2];
	COORD coord;
	DWORD written;

	PDC_LOG(("PDC_putc() - called:char=%c attrib=0x%x color=0x%x\n",
		character & A_CHARTEXT, character & A_ATTRIBUTES, color));

	PDC_get_cursor_pos(&curRow, &curCol);

	coord.X = curCol;
	coord.Y = curRow;
#if 0
	buffer[0] = color;
	WriteConsoleOutputAttribute(hConOut, &buffer, 1, coord, &written);
#endif
	buffer[0] = character;
	WriteConsoleOutputCharacter(hConOut, (char*)&buffer[0], 1, 
		coord, &written);

	return OK;
}

/*man-start*********************************************************************

  PDC_putctty()   - Output a character and attribute in TTY fashion.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs character 'chr' to screen in tty fashion. If a colour
	mode is active, the character is written with colour 'colour'.

	This function moves the physical cursor after writing so the
	screen will scroll if necessary.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_putctty(chtype character, chtype color);

**man-end**********************************************************************/

int PDC_putctty(chtype character, chtype color)
{
	int curRow, curCol;
	WORD buffer;
	COORD coord;
	DWORD written;

	PDC_LOG(("PDC_putctty() - called\n"));

	PDC_get_cursor_pos(&curRow, &curCol);

	coord.X = curCol;
	coord.Y = curRow;
#if 0
	buffer = color;
	WriteConsoleOutputAttribute(hConOut, &buffer, 1, coord, &written);
#endif
	buffer = character;
	WriteConsoleOutputCharacter(hConOut, (char*)&buffer, 1,
		coord, &written);

	return OK;
}

/*man-start*********************************************************************

  PDC_scroll() - low level screen scroll

  PDCurses Description:
	Scrolls a window in the current page up or down. Urow, lcol,
	lrow, rcol are the window coordinates. Lines is the number of
	lines to scroll. If 0, clears the window, if < 0 scrolls down,
	if > 0 scrolls up.  Blanks areas that are left, and sets
	character attributes to attr. If in a colour graphics mode,
	fills them with the colour 'attr' instead.

  PDCurses Return Value:
	The PDC_scroll() function returns OK on success otherwise ERR is 
	returned.

  Portability:
	PDCurses  int PDC_scroll(int urow, int lcol, int rcol,
				 int nlines, chtype attr);

**man-end**********************************************************************/

int PDC_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr)
{
	PDC_LOG(("PDC_scroll() - called: urow %d lcol %d lrow %d "
		"rcol %d nlines %d\n", urow, lcol, lrow, rcol, nlines));

	return OK;
}

/*man-start*********************************************************************

  PDC_transform_line()  - display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  PDCurses Return Value:
	This routine returns TRUE if a premature refresh end
	is allowed, and there is an input character pending.  Otherwise,
	FALSE is returned.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  bool PDC_transform_line(int lineno);

**man-end**********************************************************************/

bool PDC_transform_line(int lineno)
{
	int j, x, endx, len;
	chtype *srcp;
	COORD bufSize, bufPos;
	SMALL_RECT sr;

	PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return FALSE;

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	srcp = curscr->_y[lineno] + x;
	len = endx - x + 1;

	bufPos.X = bufPos.Y = 0;

	bufSize.X = len;
	bufSize.Y = 1;

	sr.Top = lineno;
	sr.Bottom = lineno;
	sr.Left = x;
	sr.Right = endx;

	for (j = 0; j < len; j++)
	{
		ci[j].Char.AsciiChar = srcp[j] & A_CHARTEXT;
		ci[j].Attributes = (chtype_attr(srcp[j]) & 0xFF00) >> 8;
	}

	WriteConsoleOutput(hConOut, ci, bufSize, bufPos, &sr);

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;

	if (SP->refrbrk && (SP->cbreak || SP->raw_inp) && PDC_breakout())
		return TRUE;

	return FALSE;
}

/*man-start*********************************************************************

  PDC_doupdate()  - display updated data in one call (Win32 only)

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical screen to look like _curscr.

  PDCurses Return Value:
	This routine returns nothing.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  void PDC_doupdate(void);

**man-end**********************************************************************/

void PDC_doupdate(void)
{
	int i, j, k;
	int starty = _NO_CHANGE, startx = _NO_CHANGE;
	int size;
	int endy = _NO_CHANGE, endx = _NO_CHANGE;
	chtype *srcp;

	CHAR_INFO *ptr;
	COORD bufSize, bufPos;
	SMALL_RECT sr;

	PDC_LOG(("PDC_doupdate() - called:\n"));

	if (curscr == (WINDOW *)NULL)
		return;

	for (i = 0; i < LINES; i++)
		if (curscr->_firstch[i] != _NO_CHANGE)
		{
			if (starty == _NO_CHANGE)
				starty = i;

			endy = i;

			if (startx == _NO_CHANGE
			    && curscr->_firstch[i] != _NO_CHANGE)
				startx = curscr->_firstch[i];

			if (curscr->_firstch[i] < startx)
				startx = curscr->_firstch[i];

			if (curscr->_lastch[i] > endx)
				endx = curscr->_lastch[i];
		}

	if (starty == _NO_CHANGE)	/* nothing to do... */
		return;

	size = ((endy - starty) + 1) * ((endx - startx) + 1);

	ptr = (CHAR_INFO*)malloc(size * sizeof(CHAR_INFO));
	if (ptr == NULL)
		return;

	bufPos.X = bufPos.Y = 0;
	bufSize.X = endx - startx + 1;
	bufSize.Y = endy - starty + 1;

	sr.Top = starty;
	sr.Bottom = endy;
	sr.Left = startx;
	sr.Right = endx;

	k = 0;

	for (i = starty; i <= endy; i++)
	{
		srcp = curscr->_y[i];
		for (j = startx; j <= endx; j++)
		{
			ptr[k].Char.AsciiChar = srcp[j] & A_CHARTEXT;
			ptr[k].Attributes =
				(chtype_attr(srcp[j]) & 0xFF00) >> 8;
			k++;
		}
		curscr->_firstch[i] = _NO_CHANGE;
		curscr->_lastch[i] = _NO_CHANGE;
	}

	WriteConsoleOutput(hConOut, ptr, bufSize, bufPos, &sr);
	free(ptr);
}
