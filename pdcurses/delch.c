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

/* undefine any macros for functions defined in this module */
#undef delch
#undef wdelch
#undef mvdelch
#undef mvwdelch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef move
# undef wmove
#endif

#ifdef PDCDEBUG
const char *rcsid_delch =
	"$Id: delch.c,v 1.17 2006/03/20 07:43:33 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  Name:                                                         delch

  Synopsis:
	int delch(void);
	int wdelch(WINDOW *win);
	int mvdelch(int y, int x);
	int mvwdelch(WINDOW *win, int y, int x);

  X/Open Description:
	The character under the cursor in the window is deleted.  All
	characters to the right on the same line are moved to the left
	one position and the last character on the line is filled with
	a blank.  The cursor position does not change (after moving to
	y, x if coordinates are specified).

	NOTE: delch(), mvdelch(), and mvwdelch() are implemented as macros.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	delch					Y	Y	Y
	wdelch					Y	Y	Y
	mvdelch					Y	Y	Y
	mvwdelch				Y	Y	Y

**man-end****************************************************************/

int delch(void)
{
	PDC_LOG(("delch() - called\n"));

	return wdelch(stdscr);
}

int wdelch(WINDOW *win)
{
	int y, x, maxx;
	chtype *temp1;

	PDC_LOG(("wdelch() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	y	= win->_cury;
	x	= win->_curx;
	maxx	= win->_maxx - 1;
	temp1	= &win->_y[y][x];

	memmove(temp1, temp1 + 1, (maxx - x) * sizeof(chtype));

	/* wrs (4/10/93) account for window background */

	win->_y[y][maxx] = win->_bkgd;

	win->_lastch[y] = maxx;

	if ((win->_firstch[y] == _NO_CHANGE) || (win->_firstch[y] > x))
		win->_firstch[y] = x;

	PDC_sync(win);

	return OK;
}

int mvdelch(int y, int x)
{
	PDC_LOG(("mvdelch() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wdelch(stdscr);
}

int mvwdelch(WINDOW *win, int y, int x)
{
	PDC_LOG(("mvwdelch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wdelch(win);
}
