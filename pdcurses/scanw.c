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
	"$Id: scanw.c,v 1.15 2006/01/30 12:17:17 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         scanw

  Synopsis:
	int scanw(char *fmt, ...);
	int wscanw(WINDOW *win, char *fmt, ...);
	int mvscanw(int y, int x, char *fmt, ...);
	int mvwscanw(WINDOW *win, int y, int x, char *fmt, ...);
	int vwscanw(WINDOW *win, char *fmt, va_list varglist);
	int vw_scanw(WINDOW *win, char *fmt, va_list varglist);

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

**man-end**********************************************************************/

int PDC_CDECL scanw(char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("scanw() - called\n"));

	va_start(args, fmt);
	retval = vwscanw(stdscr, fmt, args);
	va_end(args);

	return retval;
}

int PDC_CDECL wscanw(WINDOW *win, char *fmt, ...)
{
	va_list args;
	int retval;

	PDC_LOG(("wscanw() - called\n"));

	va_start(args, fmt);
	retval = vwscanw(win, fmt, args);
	va_end(args);

	return retval;
}

int PDC_CDECL mvscanw(int y, int x, char *fmt, ...)
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

int PDC_CDECL mvwscanw(WINDOW *win, int y, int x, char *fmt, ...)
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

int PDC_CDECL vwscanw(WINDOW *win, char *fmt, va_list varglist)
{
	char scanbuf[513];

	PDC_LOG(("vwscanw() - called\n"));

	if (wgetstr(win, scanbuf) == ERR)
		return ERR;

	return vsscanf(scanbuf, fmt, varglist);
}

int PDC_CDECL vw_scanw(WINDOW *win, char *fmt, va_list varglist)
{
	PDC_LOG(("vw_scanw() - called\n"));

	return vwscanw(win, fmt, varglist);
}
