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

#include <curspriv.h>

RCSID("$Id: clear.c,v 1.29 2007/03/16 06:33:44 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								clear

  Synopsis:
	int clear(void);
	int wclear(WINDOW *win);
	int erase(void);
	int werase(WINDOW *win);
	int clrtobot(void);
	int wclrtobot(WINDOW *win);
	int clrtoeol(void);
	int wclrtoeol(WINDOW *win);

  Description:
	erase() and werase() copy blanks to every position of the 
	window.

	clear() and wclear() are similar to erase() and werase(), except 
	they also call clearok() to ensure that the the screen is 
	cleared on the next call to wrefresh() for that window.

	clrtobot() and wclrtobot() clear the screen from the current 
	cursor position to the end of the window.

	clrtoeol() and wclrtoeol() clear the screen from the current 
	cursor position to the end of the current line.

  Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
	clear					Y	Y	Y
	wclear					Y	Y	Y
	erase					Y	Y	Y
	werase					Y	Y	Y
	clrtobot				Y	Y	Y
	wclrtobot				Y	Y	Y
	clrtoeol				Y	Y	Y
	wclrtoeol				Y	Y	Y

**man-end****************************************************************/

int wclrtoeol(WINDOW *win)
{
	int x, y, minx;
	chtype blank, *ptr;

	PDC_LOG(("wclrtoeol() - called: Row: %d Col: %d\n",
		win->_cury, win->_curx));

	if (!win)
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

int clrtoeol(void)
{
	PDC_LOG(("clrtoeol() - called\n"));

	return wclrtoeol(stdscr);
}

int wclrtobot(WINDOW *win)
{
	int savey = win->_cury;
	int savex = win->_curx;

	PDC_LOG(("wclrtobot() - called\n"));

	if (!win)
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

int clrtobot(void)
{
	PDC_LOG(("clrtobot() - called\n"));

	return wclrtobot(stdscr);
}

int werase(WINDOW *win)
{
	PDC_LOG(("werase() - called\n"));

	if (wmove(win, 0, 0) == ERR)
		return ERR;

	return wclrtobot(win);
}

int erase(void)
{
	PDC_LOG(("erase() - called\n"));

	return werase(stdscr);
}

int wclear(WINDOW *win)
{
	PDC_LOG(("wclear() - called\n"));

	if (!win)
		return ERR;

	win->_clear = TRUE;
	return werase(win);
}

int clear(void)
{
	PDC_LOG(("clear() - called\n"));

	return wclear(stdscr);
}
