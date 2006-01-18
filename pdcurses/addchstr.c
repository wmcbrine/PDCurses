/*
***************************************************************************
* This file comprises part of PDCurses. PDCurses is Public Domain software.
* You may use this code for whatever purposes you desire. This software
* is provided AS IS with NO WARRANTY whatsoever.
* Should this software be used in another application, an acknowledgement
* that PDCurses code is used would be appreciated, but is not mandatory.
*
* Any changes which you make to this software which may improve or enhance
* it, should be forwarded to the current maintainer for the benefit of 
* other users.
*
* The only restriction placed on this code is that no distribution of
* modified PDCurses code be made under the PDCurses name, by anyone
* other than the current maintainer.
* 
* See the file maintain.er for details of the current maintainer.
***************************************************************************
*/
#define CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef   addchstr
#undef   addchnstr
#undef   waddchstr
#undef   waddchnstr
#undef   mvaddchstr
#undef   mvaddchnstr
#undef   mvwaddchstr
#undef   mvwaddchnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef move
#  undef wmove
#endif

#ifdef PDCDEBUG
char *rcsid_addchstr  = "$Id: addchstr.c,v 1.12 2006/01/18 19:30:22 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

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

  X/Open Errors:
	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      addchstr                              -        -      4.0
      waddchstr                             -        -      4.0
      mvaddchstr                            -        -      4.0
      mvwaddchstr                           -        -      4.0
      addchnstr                             -        -      4.0
      waddchnstr                            -        -      4.0
      mvaddchnstr                           -        -      4.0
      mvwaddchnstr                          -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   addchstr(const chtype *ch)
#else
int   PDC_CDECL   addchstr(ch)
chtype *ch;
#endif
/***********************************************************************/
{
	PDC_LOG(("addchstr() - called\n"));

	return waddchnstr(stdscr, ch, -1);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   addchnstr(const chtype *ch, int n)
#else
int   PDC_CDECL   addchnstr(ch, n)
chtype *ch;
int n;
#endif
/***********************************************************************/
{
	PDC_LOG(("addchnstr() - called\n"));

	return waddchnstr(stdscr, ch, n);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   waddchstr(WINDOW *win, const chtype *ch)
#else
int   PDC_CDECL   waddchstr(win, ch)
WINDOW *win;
chtype *ch;
#endif
/***********************************************************************/
{
	PDC_LOG(("waddchstr() - called: win=%x\n", win));

	return waddchnstr(win, ch, -1);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   waddchnstr(WINDOW *win, const chtype *ch, int n)
#else
int   PDC_CDECL   waddchnstr(win, ch, n)
WINDOW *win;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
	int y, x, maxx, minx;
	chtype *ptr;

	PDC_LOG(("waddchnstr() - called: win=%x n=%d\n", win, n));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n == 0 || n < -1)
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

			if (x > maxx || maxx == _NO_CHANGE)
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

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvaddchstr(int y, int x, const chtype *ch)
#else
int   PDC_CDECL   mvaddchstr(y, x, ch)
int y;
int x;
chtype *ch;
#endif
/***********************************************************************/
{
	PDC_LOG(("mvaddchstr() - called: y %d x %d\n", y, x));

	if (move(y, x) == ERR)
		return ERR;

	return waddchnstr(stdscr, ch, -1);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvaddchnstr(int y, int x, const chtype *ch, int n)
#else
int   PDC_CDECL   mvaddchnstr(y, x, ch, n)
int y;
int x;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
	PDC_LOG(("mvaddchnstr() - called: y %d x %d n %d\n", y, x, n));

	if (move(y, x) == ERR)
		return ERR;

	return waddchnstr(stdscr, ch, n);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvwaddchstr(WINDOW *win, int y, int x, const chtype *ch)
#else
int   PDC_CDECL   mvwaddchstr(win, y, x, ch)
WINDOW *win;
int y;
int x;
chtype *ch;
#endif
/***********************************************************************/
{
	PDC_LOG(("waddchstr() - called:\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return waddchnstr(win, ch, -1);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvwaddchnstr(WINDOW *win, int y, int x,
			       const chtype *ch, int n)
#else
int   PDC_CDECL   mvwaddchnstr(win, y, x, ch, n)
WINDOW *win;
int y;
int x;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
	PDC_LOG(("mvwaddchnstr() - called: y %d x %d n %d \n", y, x, n));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return waddchnstr(win, ch, n);
}
