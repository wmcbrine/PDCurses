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
#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>
#include <string.h>

#if defined(HAVE_STDARG_H) && defined(HAVE_PROTO)
#  include <stdarg.h>
#  define HAVE_STDARG_H_HAVE_PROTO
#else
#  include <varargs.h>
#endif

/* undefine any macros for functions defined in this module */
#undef	scanw
#undef	wscanw
#undef	mvscanw
#undef	mvwscanw
#undef	vwscanw

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	wgetstr
#  undef	wrefresh
#  undef	wmove
#endif

#ifndef HAVE_VSSCANF
#  define vsscanf PDC_vsscanf
#endif

#ifdef PDCDEBUG
char *rcsid_scanw  = "$Id: scanw.c,v 1.1 2001/01/10 08:27:26 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         scanw

  Synopsis:
  	int scanw(char *fmt, ...);
  	int wscanw(WINDOW *win, char *fmt, ...);
  	int mvscanw(int y, int x, char *fmt, ...);
  	int mvwscanw(WINDOW *win, int y, int x, char *fmt,...);
  ***	int vwscanw(WINDOW *win, char *fmt, va_list varglist);

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

  PDCurses Description:
 	The old Bjorn Larssen code for the 68K platform has been removed
 	from this module.

  X/Open Return Value:
 	Upon successful completion, the scanw, mvscanw, mvwscanw and
 	wscanw functions return the number of items successfully
 	matched.  On end-of-file, they return EOF.  Otherwise they
 	return ERR.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      scanw                                 Y        Y       Y
      wscanw                                Y        Y       Y
      mvscanw                               Y        Y       Y
      mvwscanw                              Y        Y       Y
      vwscanw                               -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	scanw(char *fmt, ...)
#else
int	PDC_CDECL	scanw(fmt,va_alist)
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	va_list args;
	int	retval = ERR;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("scanw() - called\n");
#endif

#if	!defined (HC)
	if (stdscr == (WINDOW *)NULL)
		return( retval );

	wrefresh(stdscr);	/* set cursor position */

	/*
	 * get string
	 */
	c_printscanbuf[0] = '\0';  /* reset to empty string */
	if (wgetstr(stdscr, c_printscanbuf) == ERR)
		return( retval );
#ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
#else
	va_start(args);
#endif

	retval = vsscanf(c_printscanbuf, fmt, args);
	va_end(args);
#endif

	return( retval );
}

/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	wscanw(WINDOW *win, char *fmt, ...)
#else
int	PDC_CDECL	wscanw(win,fmt,va_alist)
WINDOW *win;
char *fmt;
va_dcl
# endif
/***********************************************************************/
{
	va_list args;
	int	retval = ERR;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wscanw() - called\n");
#endif

#if	!defined (HC)
	if (win == (WINDOW *)NULL)
		return (retval);

	wrefresh(win);		/* set cursor position */

	/*
	 * get string
	 */
	c_printscanbuf[0] = '\0';  /* reset to empty string */
	if (wgetstr(win, c_printscanbuf) == ERR)
		return( retval );
# ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
# else
	va_start(args);
# endif
	retval = vsscanf(c_printscanbuf, fmt, args);
	va_end(args);
#endif

	return( retval );
}

/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	mvscanw(int y, int x, char *fmt, ... )
#else
int	PDC_CDECL	mvscanw(y,x,fmt,va_alist)
int y,x;
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	va_list args;
	int	retval = ERR;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvscanw() - called\n");
#endif

#if	!defined (HC)
	if (stdscr == (WINDOW *)NULL)
		return( retval );

	if (wmove(stdscr, y, x) == ERR)
		return( retval );

	wrefresh(stdscr);			/* set cursor position */

	/*
	 * get string
	 */
	c_printscanbuf[0] = '\0';  /* reset to empty string */
	if (wgetstr(stdscr, c_printscanbuf) == ERR)
		return( retval );
# ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
# else
	va_start(args);
# endif
	retval = vsscanf(c_printscanbuf, fmt, args);
	va_end(args);
#endif

	return( retval );
}

/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	mvwscanw(WINDOW *win, int y, int x, char *fmt,...)
#else
int	PDC_CDECL	mvwscanw(win,y,x,fmt,va_alist)
WINDOW *win;
int y,x;
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	va_list args;
	int	retval = ERR;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvscanw() - called\n");
#endif

#if	!defined (HC)
	if (win == (WINDOW *)NULL)
		return( retval );

	if (wmove(win, y, x) == ERR)
		return( retval );

	wrefresh(win);		/* set cursor position */

	/*
	 * get string
	 */
	c_printscanbuf[0] = '\0';  /* reset to empty string */
	if (wgetstr(win, c_printscanbuf) == ERR)
		return( retval );
# ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
# else
	va_start(args);
# endif
	retval = vsscanf(c_printscanbuf, fmt, args);
	va_end(args);
#endif
	return( retval );
}
