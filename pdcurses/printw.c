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
#undef	printw
#undef	wprintw
#undef	mvprintw
#undef	mvwprintw

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	waddstr
#  undef	wmove
#endif

#ifdef PDCDEBUG
char *rcsid_printw  = "$Id: printw.c,v 1.1 2001/01/10 08:27:24 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        printw

  Synopsis:
  	int printw(char *fmt, ...);
  	int wprintw(WINDOW *win, char *fmt, ...);
  	int mvprintw(int y, int x, char *fmt, ...);
  	int mvwprintw(WINDOW *win, int y, int x, char *fmt,...);
  ***	int vwprintw(WINDOW *win, char *fmt, va_list varglist);

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

  PDCurses Description:
 	The old Bjorn Larssen code for the 68K platform has been removed
 	from this module.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      printw                                Y        Y       Y
      wprintw                               Y        Y       Y
      mvprintw                              Y        Y       Y
      mvwprintw                             Y        Y       Y
      vwprintw                              -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	printw(char *fmt,...)
#else
int	PDC_CDECL	printw(fmt,va_alist)
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	int	retval = ERR;
	va_list args;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("printw() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return (retval);

#ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
#else
	va_start(args);
#endif

	vsprintf(c_printscanbuf, fmt, args);
	va_end(args);
	if (waddstr(stdscr, c_printscanbuf) == ERR)
		return (retval);
	retval = (strlen(c_printscanbuf));
	return (retval);
}
/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	wprintw(WINDOW *win,char *fmt,...)
#else
int	PDC_CDECL	wprintw(win,fmt,va_alist)
WINDOW *win;
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	int	retval = ERR;
	va_list args;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wprintw() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return (retval);

#ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
#else
	va_start(args);
#endif

	vsprintf(c_printscanbuf, fmt, args);
	va_end(args);
	if (waddstr(win, c_printscanbuf) == ERR)
		return (retval);
	retval = (strlen(c_printscanbuf));
	return (retval);
}
/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	mvprintw(int y, int x,char *fmt,...)
#else
int	PDC_CDECL	mvprintw(y,x,fmt,va_alist)
int y,x;
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	int	retval = ERR;
	va_list args;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvprintw() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return (retval);

	if (wmove(stdscr, y, x) == ERR)
		return( retval );

#ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
#else
	va_start(args);
#endif

	vsprintf(c_printscanbuf, fmt, args);
	va_end(args);

	if (waddstr(stdscr, c_printscanbuf) == ERR)
		return( retval );
	retval = (strlen(c_printscanbuf));
	return( retval );
}
/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int	PDC_CDECL	mvwprintw(WINDOW *win,int y, int x,char *fmt,...)
#else
int	PDC_CDECL	mvwprintw(win,y,x,fmt,va_alist)
WINDOW *win;
int y,x;
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
	int	retval = ERR;
	va_list args;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwprintw() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return (retval);
	if (wmove(win, y, x) == ERR)
		return( retval );

#ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(args, fmt);
#else
	va_start(args);
#endif

	vsprintf(c_printscanbuf, fmt, args);
	va_end(args);

	if (waddstr(win, c_printscanbuf) == ERR)
		return (retval);
	retval = (strlen(c_printscanbuf));
	return (retval);
}
