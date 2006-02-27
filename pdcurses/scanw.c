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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef scanw
#undef wscanw
#undef mvscanw
#undef mvwscanw
#undef vwscanw
#undef vw_scanw

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef wgetstr
# undef wrefresh
# undef wmove
#endif

#ifndef HAVE_VSSCANF
# define vsscanf PDC_vsscanf
#endif

#ifdef PDCDEBUG
const char *rcsid_scanw =
	"$Id: scanw.c,v 1.20 2006/02/27 21:48:04 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  Name:                                                         scanw

  Synopsis:
	int scanw(const char *fmt, ...);
	int wscanw(WINDOW *win, const char *fmt, ...);
	int mvscanw(int y, int x, const char *fmt, ...);
	int mvwscanw(WINDOW *win, int y, int x, const char *fmt, ...);
	int vwscanw(WINDOW *win, const char *fmt, va_list varglist);
	int vw_scanw(WINDOW *win, const char *fmt, va_list varglist);

  X/Open Description:
	These routines correspond to scanf(). The function scanw() reads
	input from the default window. The function wscanw() reads
	input from the specified window. The function mvscanw() moves
	the cursor to the specified position and then reads input from
	the default window. The function mvwscanw() moves the cursor to
	the specified position and then reads input from the specified
	window.

	For all the functions, the routine wgetstr() is called to get a
	string from the window, and the resulting line is used as
	input for the scan.  All character interpretation is carried
	out according to the scanf function rules.

  X/Open Return Value:
	Upon successful completion, the scanw, mvscanw, mvwscanw and
	wscanw functions return the number of items successfully
	matched.  On end-of-file, they return EOF.  Otherwise they
	return ERR.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	scanw					Y	Y	Y
	wscanw					Y	Y	Y
	mvscanw					Y	Y	Y
	mvwscanw				Y	Y	Y
	vwscanw					-	-      4.0

**man-end****************************************************************/

int scanw(const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("scanw() - called\n"));

	va_start(args, fmt);
	retval = vwscanw(stdscr, fmt, args);
	va_end(args);

	return retval;
}

int wscanw(WINDOW *win, const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("wscanw() - called\n"));

	va_start(args, fmt);
	retval = vwscanw(win, fmt, args);
	va_end(args);

	return retval;
}

int mvscanw(int y, int x, const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("mvscanw() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	va_start(args, fmt);
	retval = vwscanw(stdscr, fmt, args);
	va_end(args);

	return retval;
}

int mvwscanw(WINDOW *win, int y, int x, const char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("mvscanw() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	va_start(args, fmt);
	retval = vwscanw(win, fmt, args);
	va_end(args);

	return retval;
}

int vwscanw(WINDOW *win, const char *fmt, va_list varglist)
{
	char scanbuf[513];

	PDC_LOG(("vwscanw() - called\n"));

	if (wgetstr(win, scanbuf) == ERR)
		return ERR;

	return vsscanf(scanbuf, fmt, varglist);
}

int vw_scanw(WINDOW *win, const char *fmt, va_list varglist)
{
	PDC_LOG(("vw_scanw() - called\n"));

	return vwscanw(win, fmt, varglist);
}
