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

RCSID("$Id: clear.c,v 1.22 2006/09/24 21:22:33 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                         clear

  Synopsis:
	int clear(void);
	int wclear(WINDOW *win);
	int erase(void);
	int werase(WINDOW *win);
	int clrtobot(void);
	int wclrtobot(WINDOW *win);
	int clrtoeol(void);
	int wclrtoeol(WINDOW *win);

  X/Open Description:
	The erase() and werase() functions copy blanks to every position
	of the window.

	The clear() and wclear() functions are similar to erase() and
	werase() except they also call clearok() to ensure that the
	the screen is cleared on the next call to wrefresh() for that
	window.

	The clrtobot() and wclrtobot() functions clear the screen from
	the current cursor position to the end of the current line and
	all remaining lines in the window.

	The clrtoeol() and wclrtoeol() functions clear the screen from
	the current cursor position to the end of the current line only.

	NOTE: clear(), wclear(), erase(), clrtobot(), and clrtoeol()
	are implemented as macros

  PDCurses Description:

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	clear					Y	Y	Y
	wclear					Y	Y	Y
	erase					Y	Y	Y
	werase					Y	Y	Y
	clrtobot				Y	Y	Y
	wclrtobot				Y	Y	Y
	clrtoeol				Y	Y	Y
	wclrtoeol				Y	Y	Y

**man-end****************************************************************/

int clear(void)
{
	PDC_LOG(("clear() - called\n"));

	if  (stdscr == (WINDOW *)NULL)
		return ERR;

	stdscr->_clear = TRUE;
	return erase();
}

int wclear(WINDOW *win)
{
	PDC_LOG(("wclear() - called\n"));

	if  (win == (WINDOW *)NULL)
		return ERR;

	win->_clear = TRUE;
	return werase(win);
}

int erase(void)
{
	return werase(stdscr);
}

int werase(WINDOW *win)
{
	PDC_LOG(("werase() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	wmove(win, 0, 0);
	return wclrtobot(win);
}

int clrtobot(void)
{
	return wclrtobot(stdscr);
}

int wclrtobot(WINDOW *win)
{
	int savey = win->_cury;
	int savex = win->_curx;

	PDC_LOG(("wclrtobot() - called\n"));

	if  (win == (WINDOW *)NULL)
		return ERR;

	/* should this involve scrolling region somehow ? */

	if (win->_cury + 1 < win->_maxy)
	{
		win->_curx = 0;
		win->_cury++;
		for (; win->_maxy > win->_cury; win->_cury++)
			wclrtoeol(win);
		win->_cury = savey;
		win->_curx = savex;
	}
	wclrtoeol(win);

	PDC_sync(win);
	return OK;
}

int clrtoeol(void)
{
	return wclrtoeol(stdscr);
}

int wclrtoeol(WINDOW *win)
{
	int x, y, minx;
	chtype blank, *ptr;

	PDC_LOG(("wclrtoeol() - called: Row: %d Col: %d\n",
		win->_cury, win->_curx));

	if (win == (WINDOW *)NULL)
		return ERR;

	y = win->_cury;
	x = win->_curx;

	/* wrs (4/10/93) account for window background */

	blank = win->_bkgd;

	for (minx = x, ptr = &win->_y[y][x]; minx < win->_maxx; minx++, ptr++)
		*ptr = blank;

	win->_firstch[y] = (win->_firstch[y] == _NO_CHANGE) ?
		x : min(x, win->_firstch[y]);
	win->_lastch[y] = max(win->_lastch[y], win->_maxx - 1);

	PDC_sync(win);
	return OK;
}
