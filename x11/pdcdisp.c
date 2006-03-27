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

#ifdef PDCDEBUG
const char *rcsid_PDCdisp =
	"$Id: pdcdisp.c,v 1.20 2006/03/27 14:33:00 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  PDC_cursor_on()	- Turns on the hardware cursor.

  PDCurses Description:
	Turns on the hardware curses, it does nothing if it is already on.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  Portability:
	PDCurses  int PDC_cursor_on(void);

**man-end****************************************************************/

int PDC_cursor_on(void)
{
	PDC_LOG(("PDC_cursor_on() - called\n"));

	return OK;
}

/*man-start**************************************************************

  PDC_cursor_off()	- Turns off the hardware cursor.

  PDCurses Description:
	Turns off the hardware curses, it does nothing if it is already off.

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

	return OK;
}

/*man-start**************************************************************

  PDC_gotoxy()	- position hardware cursor at (x, y)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the
	screen. We don't optimize here -- on a PC, it takes more time
	to optimize than to do things directly.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_gotoxy(int row, int col);

**man-end****************************************************************/

int PDC_gotoxy(int row, int col)
{
	PDC_LOG(("PDC_gotoxy() - called: row %d col %d\n", row, col));

	XCurses_display_cursor(SP->cursrow, SP->curscol, row, col, 
		SP->visibility);

	return OK;
}

/*man-start**************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  void PDC_transform_line(int lineno);

**man-end****************************************************************/

void PDC_transform_line(int lineno)
{
	const chtype *dstp;
	int x, endx, len;

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return;

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	dstp = curscr->_y[lineno] + x;
	len = endx - x + 1;

	XCurses_transform_line(dstp, lineno, x, len);

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;
}
