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
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef addchstr
#undef addchnstr
#undef waddchstr
#undef waddchnstr
#undef mvaddchstr
#undef mvaddchnstr
#undef mvwaddchstr
#undef mvwaddchnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef move
# undef wmove
#endif

RCSID("$Id: addchstr.c,v 1.28 2006/07/15 15:38:24 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                        addchstr

  Synopsis:
	int addchstr(const chtype *ch);
	int addchnstr(const chtype *ch, int n);
	int waddchstr(WINDOW *win, const chtype *ch);
	int waddchnstr(WINDOW *win, const chtype *ch, int n);
	int mvaddchstr(int y, int x, const chtype *ch);
	int mvaddchnstr(int y, int x, const chtype *ch, int n);
	int mvwaddchstr(WINDOW *, int y, int x, const chtype *ch);
	int mvwaddchnstr(WINDOW *, int y, int x, const chtype *ch, int n);

  X/Open Description:
	These routines write a chtype directly into the window structure 
	starting at the current position. The four routines with n as 
	the last argument copy at most n elements, but no more than will 
	fit on the line. If n = -1 then the whole string is copied, to 
	the maximum number that will fit on the line.

	The cursor position is not advanced. These routines do not check 
	for newline or other special characters, nor does any line 
	wrapping occur.

	NOTE: addchstr(), mvaddchstr(), mvwaddchstr() addchnstr(), 
	mvaddchnstr(), and mvwaddchnstr() are implemented as macros.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	addchstr				-	-      4.0
	waddchstr				-	-      4.0
	mvaddchstr				-	-      4.0
	mvwaddchstr				-	-      4.0
	addchnstr				-	-      4.0
	waddchnstr				-	-      4.0
	mvaddchnstr				-	-      4.0
	mvwaddchnstr				-	-      4.0

**man-end****************************************************************/

int addchstr(const chtype *ch)
{
	PDC_LOG(("addchstr() - called\n"));

	return waddchnstr(stdscr, ch, -1);
}

int addchnstr(const chtype *ch, int n)
{
	PDC_LOG(("addchnstr() - called\n"));

	return waddchnstr(stdscr, ch, n);
}

int waddchstr(WINDOW *win, const chtype *ch)
{
	PDC_LOG(("waddchstr() - called: win=%x\n", win));

	return waddchnstr(win, ch, -1);
}

int waddchnstr(WINDOW *win, const chtype *ch, int n)
{
	int y, x, maxx, minx;
	chtype *ptr;

	PDC_LOG(("waddchnstr() - called: win=%x n=%d\n", win, n));

	if (!win || !ch || !n || n < -1)
		return ERR;

	x = win->_curx;
	y = win->_cury;
	ptr = &(win->_y[y][x]);

	if (n == -1 || n > win->_maxx - x)
		n = win->_maxx - x;

	minx = win->_firstch[y];
	maxx = win->_lastch[y];

	for (; n && *ch; n--, x++, ptr++, ch++)
	{
		if (*ptr != *ch)
		{
			if (x < minx || minx == _NO_CHANGE)
				minx = x;

			if (x > maxx)
				maxx = x;

			PDC_LOG(("y %d x %d minx %d maxx %d *ptr %x *ch"
				 " %x firstch: %d lastch: %d\n",
				 y, x, minx, maxx, *ptr, *ch, 
				 win->_firstch[y], win->_lastch[y]));

			*ptr = *ch;
		}
	}

	win->_firstch[y] = minx;
	win->_lastch[y] = maxx;

	return OK;
}

int mvaddchstr(int y, int x, const chtype *ch)
{
	PDC_LOG(("mvaddchstr() - called: y %d x %d\n", y, x));

	if (move(y, x) == ERR)
		return ERR;

	return waddchnstr(stdscr, ch, -1);
}

int mvaddchnstr(int y, int x, const chtype *ch, int n)
{
	PDC_LOG(("mvaddchnstr() - called: y %d x %d n %d\n", y, x, n));

	if (move(y, x) == ERR)
		return ERR;

	return waddchnstr(stdscr, ch, n);
}

int mvwaddchstr(WINDOW *win, int y, int x, const chtype *ch)
{
	PDC_LOG(("mvwaddchstr() - called:\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return waddchnstr(win, ch, -1);
}

int mvwaddchnstr(WINDOW *win, int y, int x, const chtype *ch, int n)
{
	PDC_LOG(("mvwaddchnstr() - called: y %d x %d n %d \n", y, x, n));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return waddchnstr(win, ch, n);
}

#ifdef PDC_WIDE
int add_wchstr(const cchar_t *wch)
{
	PDC_LOG(("add_wchstr() - called\n"));

	return wadd_wchnstr(stdscr, wch, -1);
}

int add_wchnstr(const cchar_t *wch, int n)
{
	PDC_LOG(("add_wchnstr() - called\n"));

	return wadd_wchnstr(stdscr, wch, n);
}

int wadd_wchstr(WINDOW *win, const cchar_t *wch)
{
	PDC_LOG(("wadd_wchstr() - called: win=%x\n", win));

	return wadd_wchnstr(win, wch, -1);
}

int wadd_wchnstr(WINDOW *win, const cchar_t *wch, int n)
{
	PDC_LOG(("wadd_wchnstr() - called: win=%x n=%d\n", win, n));

	return waddchnstr(win, wch, n);
}

int mvadd_wchstr(int y, int x, const cchar_t *wch)
{
	PDC_LOG(("mvadd_wchstr() - called: y %d x %d\n", y, x));

	if (move(y, x) == ERR)
		return ERR;

	return wadd_wchnstr(stdscr, wch, -1);
}

int mvadd_wchnstr(int y, int x, const cchar_t *wch, int n)
{
	PDC_LOG(("mvadd_wchnstr() - called: y %d x %d n %d\n", y, x, n));

	if (move(y, x) == ERR)
		return ERR;

	return wadd_wchnstr(stdscr, wch, n);
}

int mvwadd_wchstr(WINDOW *win, int y, int x, const cchar_t *wch)
{
	PDC_LOG(("mvwadd_wchstr() - called:\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wadd_wchnstr(win, wch, -1);
}

int mvwadd_wchnstr(WINDOW *win, int y, int x, const cchar_t *wch, int n)
{
	PDC_LOG(("mvwadd_wchnstr() - called: y %d x %d n %d \n", y, x, n));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wadd_wchnstr(win, wch, n);
}
#endif
