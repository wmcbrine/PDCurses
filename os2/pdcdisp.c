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

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>

RCSID("$Id: pdcdisp.c,v 1.27 2006/07/15 20:46:23 wmcbrine Exp $");

extern unsigned char atrtab[MAX_ATRTAB];

/*man-start**************************************************************

  PDC_cursor_on()	- Turns on the hardware cursor.

  PDCurses Description:
	Turns on the hardware cursor; does nothing if it is already on.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  Portability:
	PDCurses  int PDC_cursor_on(void);

**man-end****************************************************************/

int PDC_cursor_on(void)
{
	PDC_LOG(("PDC_cursor_on() - called\n"));

	if (!SP->visible_cursor)
	{
		SP->visible_cursor = TRUE;
		PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
			     (SP->cursor & 0x00ff));
	}

	return OK;
}

/*man-start**************************************************************

  PDC_cursor_off()	- Turns off the hardware cursor.

  PDCurses Description:
	Turns off the hardware cursor; does nothing if it is already off.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
	ERR will be returned if the hardware cursor can not be disabled.

  Portability:
	PDCurses  int PDC_cursor_off(void);

**man-end****************************************************************/

int PDC_cursor_off(void)
{
	PDC_LOG(("PDC_cursor_off() - called\n"));

	if (SP->visible_cursor)
	{
		SP->visible_cursor = FALSE;
		PDC_set_cursor_mode(32, 33);	/* turn it off */
	}

	return OK;
}

/*man-start**************************************************************

  PDC_gotoxy()	- position hardware cursor at (x, y)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the screen. 
	We don't optimize here -- on a PC, it takes more time to 
	optimize than to do things directly.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_gotoxy(int row, int col);

**man-end****************************************************************/

int PDC_gotoxy(int row, int col)
{
	PDC_LOG(("PDC_gotoxy() - called: row %d col %d\n", row, col));

#ifdef EMXVIDEO
	v_gotoxy(col, row);
#else
	VioSetCurPos(row, col, 0);
#endif
	return OK;
}

/*man-start**************************************************************

  PDC_putc()	- Output a character in the current attribute.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs character 'chr' to screen in tty fashion. If a colour
	mode is active, the character is written with colour 'colour'.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_putc(chtype character, chtype color);

**man-end****************************************************************/

int PDC_putc(chtype character, chtype color)
{
	int curRow = 0, curCol = 0;
#ifdef EMXVIDEO
	char Cell[2];
#endif
	PDC_LOG(("PDC_putc() - called: char=%c attrib=0x%x color=0x%x\n",
		character & A_CHARTEXT, character & A_ATTRIBUTES, color));

	PDC_get_cursor_pos(&curRow, &curCol);
#ifdef EMXVIDEO
	Cell[0] = (char)character;
	Cell[1] = (char)color;
	v_putline(Cell, curCol, curRow, 1);
#else
	VioWrtTTY((PCH)&character, 1, 0);
	VioWrtNAttr((PBYTE)&color, 1, (USHORT)curRow, (USHORT)curCol, 0);
	PDC_gotoxy(curRow, curCol);
#endif
	return OK;
}

/*man-start**************************************************************

  PDC_putctty()	- Output a character and attribute in TTY fashion.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs character 'chr' to screen in tty fashion. If a colour
	mode is active, the character is written with colour 'colour'.

	This function moves the physical cursor after writing so the
	screen will scroll if necessary.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_putctty(chtype character, chtype color);

**man-end****************************************************************/

int PDC_putctty(chtype character, chtype color)
{
	int curRow=0, curCol=0;

	PDC_LOG(("PDC_putctty() - called\n"));

	PDC_get_cursor_pos(&curRow, &curCol);
#ifdef EMXVIDEO
	v_attrib(color);
	v_putc(character);
#else
	VioWrtTTY((PCH)&character, 1, 0);
	VioWrtNAttr((PBYTE)&color, 1, (USHORT)curRow, (USHORT)curCol, 0);
#endif
	return OK;
}

/*man-start**************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  Portability:
	PDCurses  void PDC_transform_line(int lineno);

**man-end****************************************************************/

void PDC_transform_line(int lineno)
{
	/* this should be enough for the maximum width of a screen. */

	unsigned short temp_line[256];

	chtype *srcp;
	int j, x, len;

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return;

	x = curscr->_firstch[lineno];
	len = curscr->_lastch[lineno] - x + 1;
	srcp = curscr->_y[lineno] + x;

	/* replace the attribute part of the chtype with the 
	   actual colour value for each chtype in the line */

	for (j = 0; j < len; j++)
		temp_line[j] = chtype_attr(srcp[j]) | (srcp[j] & A_CHARTEXT);

#ifdef EMXVIDEO
	v_putline((char*)temp_line, x, lineno, len);
#else
	VioWrtCellStr((PCH)temp_line, (USHORT)(len * sizeof(unsigned short)),
		(USHORT)lineno, (USHORT)x, 0);
#endif

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;
}
