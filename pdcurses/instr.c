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
#undef instr
#undef innstr
#undef winstr
#undef winnstr
#undef mvinstr
#undef mvinnstr
#undef mvwinstr
#undef mvwinnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef move
# undef wmove
# undef inch
# undef winch
# undef mvwinch
#endif

RCSID("$Id: instr.c,v 1.18 2006/03/29 20:06:41 wmcbrine Exp $");

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

  System V Curses Description:
	These routines return a string of characters in str, starting at 
	the current cursor position in the named window and ending at 
	the right margin of the window.  Attributes are stripped from 
	the characters. The four functions with n as the last argument 
	return the string at most n characters long.

	Note: instr(), innstr(), winstr(), mvinstr(), mvinnstr(),
	mvwinstr() and mvwinnstr() are all macros.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

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

	return winnstr(stdscr, str, stdscr->_maxx);
}

int innstr(char *str, int n)
{
	PDC_LOG(("innstr() - called: n %d \n", n));

	return winnstr(stdscr, str, n);
}

int winstr(WINDOW *win, char *str)
{
	PDC_LOG(("winstr() - called: \n"));

	return winnstr(win, str, win->_maxx);
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
			str[imax] = '\0';
			return ERR;
		}

		str[ic] = tmp & A_CHARTEXT;
	}

	str[imax] = '\0';

	win->_curx = oldx;

	return OK;
}

int mvinstr(int y, int x, char *str)
{
	PDC_LOG(("mvinstr() - called: y %d x %d \n", y, x));

	if (move(y, x) == ERR)
		return ERR;

	return winnstr(stdscr, str, stdscr->_maxx);
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

	return winnstr(win, str, win->_maxx);
}

int mvwinnstr(WINDOW *win, int y, int x, char *str, int n)
{
	PDC_LOG(("mvwinnstr() - called: y %d x %d n %d \n", y, x, n));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return winnstr(win, str, n);
}
