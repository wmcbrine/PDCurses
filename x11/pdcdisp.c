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

#include "pdcx11.h"

#include <string.h>

RCSID("$Id: pdcdisp.c,v 1.29 2006/07/21 04:07:05 wmcbrine Exp $");

int PDC_display_cursor(int oldrow, int oldcol, int newrow, int newcol,
			   int visibility)
{
	char buf[30];
	int idx, pos;

	PDC_LOG(("%s:PDC_display_cursor() - called: "
		"NEW row %d col %d, vis %d\n",
		XCLOGMSG, newrow, newcol, visibility));

	if (visibility == -1)
	{
		/* Only send the CURSES_DISPLAY_CURSOR message, no data */

		idx = CURSES_DISPLAY_CURSOR;
		memcpy(buf, (char *)&idx, sizeof(int));
		idx = sizeof(int);
	}
	else
	{
		idx = CURSES_CURSOR;
		memcpy(buf, (char *)&idx, sizeof(int));

		idx = sizeof(int);
		pos = oldrow + (oldcol << 8);
		memcpy(buf + idx, (char *)&pos, sizeof(int));

		idx += sizeof(int);
		pos = newrow + (newcol << 8);
		memcpy(buf + idx, (char *)&pos, sizeof(int));

		idx += sizeof(int);
	}

	if (write_socket(display_sock, buf, idx) < 0)
		XCursesExitCursesProcess(1,
			"exiting from PDC_display_cursor");

	return OK;
}

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

  PDC_gotoyx()	- position hardware cursor at (y, x)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the
	screen. We don't optimize here -- on a PC, it takes more time
	to optimize than to do things directly.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_gotoyx(int row, int col);

**man-end****************************************************************/

int PDC_gotoyx(int row, int col)
{
	PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

	PDC_display_cursor(SP->cursrow, SP->curscol, row, col, 
		SP->visibility);

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
	const chtype *dstp;
	int x, endx, len;

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return;

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	dstp = curscr->_y[lineno] + x;
	len = endx - x + 1;

	get_line_lock(lineno);

	memcpy(Xcurscr + XCURSCR_Y_OFF(lineno) + (x * sizeof(chtype)),
		dstp, len * sizeof(chtype));

	*(Xcurscr + XCURSCR_START_OFF + lineno) = x;
	*(Xcurscr + XCURSCR_LENGTH_OFF + lineno) = len;

	release_line_lock(lineno);

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;

	XCursesInstructAndWait(CURSES_REFRESH);

	if (lineno == SP->cursrow)
		PDC_display_cursor(SP->cursrow, SP->curscol,
			SP->cursrow, SP->curscol, 1);
}
