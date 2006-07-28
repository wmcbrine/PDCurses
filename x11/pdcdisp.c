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

RCSID("$Id: pdcdisp.c,v 1.33 2006/07/28 22:06:53 wmcbrine Exp $");

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

  PDC_gotoyx()	- position hardware cursor at (y, x)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the
	screen. We don't optimize here -- on a PC, it takes more time
	to optimize than to do things directly.

  Portability:
	PDCurses  void PDC_gotoyx(int row, int col);

**man-end****************************************************************/

void PDC_gotoyx(int row, int col)
{
	PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

	PDC_display_cursor(SP->cursrow, SP->curscol, row, col, 
		SP->visibility);
}

/*man-start**************************************************************

  PDC_transform_line()	- display a physical line of the screen

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
	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	get_line_lock(lineno);

	memcpy(Xcurscr + XCURSCR_Y_OFF(lineno) + (x * sizeof(chtype)),
		srcp, len * sizeof(chtype));

	*(Xcurscr + XCURSCR_START_OFF + lineno) = x;
	*(Xcurscr + XCURSCR_LENGTH_OFF + lineno) = len;

	release_line_lock(lineno);

	XCursesInstructAndWait(CURSES_REFRESH);
}
