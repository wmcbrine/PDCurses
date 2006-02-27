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
#undef printw
#undef wprintw
#undef mvprintw
#undef mvwprintw
#undef vwprintw
#undef vw_printw

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef waddstr
# undef wmove
#endif

#ifdef PDCDEBUG
const char *rcsid_printw =
	"$Id: printw.c,v 1.20 2006/02/27 08:38:14 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  Name:                                                        printw

  Synopsis:
	int printw(const char *fmt, ...);
	int wprintw(WINDOW *win, const char *fmt, ...);
	int mvprintw(int y, int x, const char *fmt, ...);
	int mvwprintw(WINDOW *win, int y, int x, const char *fmt,...);
	int vwprintw(WINDOW *win, const char *fmt, va_list varglist);
	int vw_printw(WINDOW *win, const char *fmt, va_list varglist);

  X/Open Description:
	The printw() routine adds a string to the default window
	starting at the current cursor position.  This routine causes
	the string that would normally be output by printf() to be
	output by addstr().

	The routine wprintw() adds a string to the specified window
	starting at the current cursor position.  This routine causes
	the string that would normally be output by printf() to be
	output by waddstr().

	The routine mvprintw() adds a string to the default window
	starting at the specified cursor position.  This routine
	causes the string that would normally be output by printf() to
	be output by addstr().

	The routine mvwprintw() adds a string to the specified window
	starting at the specified cursor position.  This routine
	causes the string that would normally be output by printf() to
	be output by waddstr().

	All these routines are analogous to printf().  It is advisable
	to use the field width options of printf() to avoid leaving
	unwanted characters on the screen from earlier calls.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	printw					Y	Y	Y
	wprintw					Y	Y	Y
	mvprintw				Y	Y	Y
	mvwprintw				Y	Y	Y
	vwprintw				-	-      4.0

**man-end****************************************************************/

int printw(const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("printw() - called\n"));

	va_start(args, fmt);
	retval = vwprintw(stdscr, fmt, args);
	va_end(args);

	return retval;
}

int wprintw(WINDOW *win, const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("wprintw() - called\n"));

	va_start(args, fmt);
	retval = vwprintw(win, fmt, args);
	va_end(args);

	return retval;
}

int mvprintw(int y, int x, const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("mvprintw() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	va_start(args, fmt);
	retval = vwprintw(stdscr, fmt, args);
	va_end(args);

	return retval;
}

int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("mvwprintw() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	va_start(args, fmt);
	retval = vwprintw(win, fmt, args);
	va_end(args);

	return retval;
}

int vwprintw(WINDOW *win, const char *fmt, va_list varglist)
{
	char printbuf[513];
	int len;

	PDC_LOG(("vwprintw() - called\n"));

	len = vsprintf(printbuf, fmt, varglist);

	return (waddstr(win, printbuf) == ERR) ? ERR : len;
}

int vw_printw(WINDOW *win, const char *fmt, va_list varglist)
{
	PDC_LOG(("vw_printw() - called\n"));

	return vwprintw(win, fmt, varglist);
}
