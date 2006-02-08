/**************************************************************************
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
**************************************************************************/

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
	"$Id: printw.c,v 1.16 2006/02/08 17:40:33 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        printw

  Synopsis:
	int printw(char *fmt, ...);
	int wprintw(WINDOW *win, char *fmt, ...);
	int mvprintw(int y, int x, char *fmt, ...);
	int mvwprintw(WINDOW *win, int y, int x, char *fmt,...);
	int vwprintw(WINDOW *win, char *fmt, va_list varglist);
	int vw_printw(WINDOW *win, char *fmt, va_list varglist);

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

**man-end**********************************************************************/

int printw(char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("printw() - called\n"));

	va_start(args, fmt);
	retval = vwprintw(stdscr, fmt, args);
	va_end(args);

	return retval;
}

int wprintw(WINDOW *win, char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("wprintw() - called\n"));

	va_start(args, fmt);
	retval = vwprintw(win, fmt, args);
	va_end(args);

	return retval;
}

int mvprintw(int y, int x, char *fmt, ...)
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

int mvwprintw(WINDOW *win, int y, int x, char *fmt, ...)
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

int vwprintw(WINDOW *win, char *fmt, va_list varglist)
{
	char printbuf[513];
	int len;

	PDC_LOG(("vwprintw() - called\n"));

	len = vsprintf(printbuf, fmt, varglist);

	return (waddstr(win, printbuf) == ERR) ? ERR : len;
}

int vw_printw(WINDOW *win, char *fmt, va_list varglist)
{
	PDC_LOG(("vw_printw() - called\n"));

	return vwprintw(win, fmt, varglist);
}
