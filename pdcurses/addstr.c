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

/* undefine any macros for functions defined in this module */
#undef	addstr
#undef	addnstr
#undef	waddstr
#undef	waddnstr
#undef	mvaddstr
#undef	mvaddnstr
#undef	mvwaddstr
#undef	mvwaddnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#  undef	addch
#  undef	waddch
#endif

#ifdef PDCDEBUG
char *rcsid_addstr  = "$Id: addstr.c,v 1.1 2001/01/10 08:26:47 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        addstr

  Synopsis:
  	int addstr(char *str);
  	int addnstr(char *str, int n);
  	int waddstr(WINDOW *win, char *str);
  	int waddnstr(WINDOW *win, char *str, int n);
  	int mvaddstr(int y, int x, char *str);
  	int mvaddnstr(int y, int x, char *str, int n);
  	int mvwaddstr(WINDOW *, int y, int x, char *str);
  	int mvwaddnstr(WINDOW *, int y, int x, char *str, int n);

  X/Open Description:
 	These routines write all the characters of the null-terminated
 	string str on the given window.  The functionality is equivalent
 	to calling waddch() once for each character in the string.  The four
 	routines with n as the last argument write at most n characters.  If n
 	is negative, then the entire string will be added.

 	NOTE:	addstr(), mvaddstr(), and mvwaddstr() are implemented as macros.
 		addnstr(), mvaddnstr(), and mvwaddnstr() are implemented as macros.

  PDCurses Description:
 	The *raw*() routines output 8 bit values.  These contrast to their
 	normal counterparts which output 7 bit values and convert control
 	character to the ^X notation.

 	str is a standard 8 bit character string WITHOUT embedded attributes.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      addstr                                Y        Y       Y
      waddstr                               Y        Y       Y
      mvaddstr                              Y        Y       Y
      mvwaddstr                             Y        Y       Y
      addnstr                               -        -      4.0
      waddnstr                              -        -      4.0
      mvaddnstr                             -        -      4.0
      mvwaddnstr                            -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	addstr(char *str)
#else
int	PDC_CDECL	addstr(str)
char *str;
#endif
/***********************************************************************/
{
	int i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("addstr() - called: string=\"%s\"\n",str);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	while (*str)
	{
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( stdscr, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	addnstr(char *str, int n)
#else
int	PDC_CDECL	addnstr(str,n)
char *str;
int n;
#endif
/***********************************************************************/
{
	int ic = 0;
	int i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("addnstr() - called: string=\"%s\" n %d \n",str,n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	while ( *str && (ic < n || n < 0) )
	{
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( stdscr, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
		ic++;
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	waddstr(WINDOW *win, char *str)
#else
int	PDC_CDECL	waddstr(win,str)
WINDOW *win;
char *str;
#endif
/***********************************************************************/
{
	int i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("waddstr() - called: string=\"%s\"\n",str);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	while (*str)
	{
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( win, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	waddnstr(WINDOW *win, char *str, int n)
#else
int	PDC_CDECL	waddnstr(win,str,n)
WINDOW *win;
char *str;
int n;
#endif
/***********************************************************************/
{
	int ic = 0;
	int i;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("waddnstr() - called: string=\"%s\" n %d \n",str,n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	while ( *str && (ic < n || n < 0) )
	{
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( win, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
		ic++;
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvaddstr(int y, int x, char *str)
#else
int	PDC_CDECL	mvaddstr(y,x,str)
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
	int i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvaddstr() - called: y %d x %d string=\"%s\"\n",y,x,str);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (wmove(stdscr,y,x) == ERR)
		return( ERR );

	while (*str)
	{     
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( stdscr, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvaddnstr(int y, int x, char *str, int n)
#else
int	PDC_CDECL	mvaddnstr(y,x,str,n)
int y;
int x;
char *str;
int n;
#endif
/***********************************************************************/
{
	int ic = 0;
	int i;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvaddnstr() - called: y %d x %d string=\"%s\" n %d \n",y,x,str,n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (wmove(stdscr,y,x) == ERR)
		return( ERR );

	while ( *str && (ic < n || n < 0) )
	{                
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( stdscr, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
		ic++;
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwaddstr(WINDOW *win, int y, int x, char *str)
#else
int	PDC_CDECL	mvwaddstr(win,y,x,str)
WINDOW *win;
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
	int i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("waddstr() - called: string=\"%s\"\n",str);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	while (*str)
	{     
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( win, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwaddnstr(WINDOW *win,int y, int x, char *str, int n)
#else
int	PDC_CDECL	mvwaddnstr(win,y,x,str,n)
WINDOW *win;
int y;
int x;
char *str;
int n;
#endif
/***********************************************************************/
{
	int ic = 0;
	int i;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwaddnstr() - called: y %d x %d string=\"%s\" n %d \n",y,x,str,n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	while ( *str && (ic < n || n < 0) )
	{     
		if ((i = *str++)<0) i += 256;  /* make negative chars positive - PJK */
		if (PDC_chadd( win, (chtype)i, (bool)(!(SP->raw_out)), TRUE ) == ERR)
		{
			return( ERR );
		}
		ic++;
	}
	return( OK );
}
