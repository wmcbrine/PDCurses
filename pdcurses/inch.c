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

/* undefine any macros for functions defined in this module */
#undef inch
#undef winch
#undef mvinch
#undef mvwinch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef move
# undef wmove
#endif

#ifdef PDCDEBUG
const char *rcsid_inch =
	"$Id: inch.c,v 1.15 2006/02/23 01:46:52 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  Name:                                                          inch

  Synopsis:
	chtype inch(void);
	chtype winch(WINDOW *win);
	chtype mvinch(int y, int x);
	chtype mvwinch(WINDOW *win, int y, int x);

  X/Open Description:

	NOTE: All these functions are implemented as macros.

  PDCurses Description:
	Depending upon the state of the raw character output, 7- or
	8-bit characters will be output.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	inch					Y	Y	Y
	winch					Y	Y	Y
	mvinch					Y	Y	Y
	mvwinch					Y	Y	Y

**man-end****************************************************************/

chtype winch(WINDOW *win)
{
	PDC_LOG(("winch() - called\n"));

	if (win == (WINDOW *)NULL)
		return (chtype)ERR;

	return win->_y[win->_cury][win->_curx];
}

chtype inch(void)
{
	PDC_LOG(("inch() - called\n"));

	return winch(stdscr);
}

chtype mvinch(int y, int x)
{
	PDC_LOG(("mvinch() - called\n"));

	if (move(y, x) == ERR)
		return (chtype)ERR;

	return stdscr->_y[stdscr->_cury][stdscr->_curx];
}

chtype mvwinch(WINDOW *win, int y, int x)
{
	PDC_LOG(("mvwinch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return (chtype)ERR;

	return win->_y[win->_cury][win->_curx];
}
