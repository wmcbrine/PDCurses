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

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef inch
# undef winch
# undef mvwinch
#endif

RCSID("$Id: instr.c,v 1.25 2006/08/20 21:48:36 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                        instr

  Synopsis:
	int instr(char *str);
	int innstr(char *str, int n);
	int winstr(WINDOW *win, char *str);
	int winnstr(WINDOW *win, char *str, int n);
	int mvinstr(int y, int x, char *str);
	int mvinnstr(int y, int x, char *str, int n);
	int mvwinstr(WINDOW *win, int y, int x, char *str);
	int mvwinnstr(WINDOW *win, int y, int x, char *str, int n);

	int inwstr(wchar_t *wstr);
	int innwstr(wchar_t *wstr, int n);
	int winwstr(WINDOW *win, wchar_t *wstr);
	int winnwstr(WINDOW *win, wchar_t *wstr, int n);
	int mvinwstr(int y, int x, wchar_t *wstr);
	int mvinnwstr(int y, int x, wchar_t *wstr, int n);
	int mvwinwstr(WINDOW *win, int y, int x, wchar_t *wstr);
	int mvwinnwstr(WINDOW *win, int y, int x, wchar_t *wstr, int n);

  System V Curses Description:
	These routines return a string of characters in str, starting at 
	the current cursor position in the named window and ending at 
	the right margin of the window.  Attributes are stripped from 
	the characters. The four functions with n as the last argument 
	return the string at most n characters long.

  X/Open Return Value:
	Upon successful completion, innstr(), mvinnstr(), mvwinnstr() 
	and winnstr() return the number of characters actually read into
	the string; instr(), mvinstr(), mvwinstr() and winstr() return 
	OK. Otherwise, all these functions return ERR.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	instr					-	-      4.0
	winstr					-	-      4.0
	mvinstr					-	-      4.0
	mvwinstr				-	-      4.0
	innstr					-	-      4.0
	winnstr					-	-      4.0
	mvinnstr				-	-      4.0
	mvwinnstr				-	-      4.0

**man-end****************************************************************/

int instr(char *str)
{
	PDC_LOG(("instr() - called: string=\"%s\"\n", str));

	return (ERR == winnstr(stdscr, str, stdscr->_maxx)) ? ERR : OK;
}

int innstr(char *str, int n)
{
	PDC_LOG(("innstr() - called: n %d \n", n));

	return winnstr(stdscr, str, n);
}

int winstr(WINDOW *win, char *str)
{
	PDC_LOG(("winstr() - called: \n"));

	return (ERR == winnstr(win, str, win->_maxx)) ? ERR : OK;
}

int winnstr(WINDOW *win, char *str, int n)
{
	chtype tmp;
	int oldy, oldx, imax, ic;

	PDC_LOG(("winnstr() - called: n %d \n", n));

	if (win == (WINDOW *)NULL)
		return ERR;

	oldy = win->_cury;
	oldx = win->_curx;
	imax = win->_maxx - win->_curx;

	if (n > 0)
		imax = ((imax < n) ? imax : n);

	for (ic = 0; ic < imax; ic++)
	{
		tmp = mvwinch(win, oldy, oldx + ic);

		if (tmp == (chtype)ERR) 
		{
			str[ic] = '\0';
			return ERR;
		}

		str[ic] = tmp & A_CHARTEXT;
	}

	str[ic] = '\0';

	win->_curx = oldx;

	return ic;
}

int mvinstr(int y, int x, char *str)
{
	PDC_LOG(("mvinstr() - called: y %d x %d \n", y, x));

	if (move(y, x) == ERR)
		return ERR;

	return (ERR == winnstr(stdscr, str, stdscr->_maxx)) ? ERR : OK;
}

int mvinnstr(int y, int x, char *str, int n)
{
	PDC_LOG(("mvinnstr() - called: y %d x %d n %d \n", y, x, n));

	if (move(y, x) == ERR)
		return ERR;

	return winnstr(stdscr, str, n);
}

int mvwinstr(WINDOW *win, int y, int x, char *str)
{
	PDC_LOG(("mvwinstr() - called: y %d x %d \n", y, x));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return (ERR == winnstr(win, str, win->_maxx)) ? ERR : OK;
}

int mvwinnstr(WINDOW *win, int y, int x, char *str, int n)
{
	PDC_LOG(("mvwinnstr() - called: y %d x %d n %d \n", y, x, n));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return winnstr(win, str, n);
}

#ifdef PDC_WIDE
int inwstr(wchar_t *wstr)
{
	PDC_LOG(("inwstr() - called\n"));

	return (ERR == winnwstr(stdscr, wstr, stdscr->_maxx)) ? ERR : OK;
}

int innwstr(wchar_t *wstr, int n)
{
	PDC_LOG(("innwstr() - called\n"));

	return winnwstr(stdscr, wstr, n);
}

int winwstr(WINDOW *win, wchar_t *wstr)
{
	PDC_LOG(("winwstr() - called\n"));

	return (ERR == winnwstr(win, wstr, win->_maxx)) ? ERR : OK;
}

int winnwstr(WINDOW *win, wchar_t *wstr, int n)
{
	chtype tmp;
	int oldy, oldx, imax, ic;

	PDC_LOG(("winnstr() - called: n %d \n", n));

	if (win == (WINDOW *)NULL)
		return ERR;

	oldy = win->_cury;
	oldx = win->_curx;
	imax = win->_maxx - win->_curx;

	if (n > 0)
		imax = ((imax < n) ? imax : n);

	for (ic = 0; ic < imax; ic++)
	{
		tmp = mvwinch(win, oldy, oldx + ic);

		if (tmp == (chtype)ERR) 
		{
			wstr[ic] = L'\0';
			return ERR;
		}

		wstr[ic] = tmp & A_CHARTEXT;
	}

	wstr[ic] = L'\0';

	win->_curx = oldx;

	return ic;
}

int mvinwstr(int y, int x, wchar_t *wstr)
{
	PDC_LOG(("mvinwstr() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return (ERR == winnwstr(stdscr, wstr, stdscr->_maxx)) ? ERR : OK;
}

int mvinnwstr(int y, int x, wchar_t *wstr, int n)
{
	PDC_LOG(("mvinnstr() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return winnwstr(stdscr, wstr, n);
}

int mvwinwstr(WINDOW *win, int y, int x, wchar_t *wstr)
{
	PDC_LOG(("mvwinstr() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return (ERR == winnwstr(win, wstr, win->_maxx)) ? ERR : OK;
}

int mvwinnwstr(WINDOW *win, int y, int x, wchar_t *wstr, int n)
{
	PDC_LOG(("mvwinnwstr() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return winnwstr(win, wstr, n);
}
#endif
