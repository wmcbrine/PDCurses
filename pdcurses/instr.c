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
#undef	instr
#undef	innstr
#undef	winstr
#undef	winnstr
#undef	mvinstr
#undef	mvinnstr
#undef	mvwinstr
#undef	mvwinnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#  undef	inch
#  undef	winch
#  undef	mvwinch
#endif

#ifdef PDCDEBUG
char *rcsid_instr  = "$Id: instr.c,v 1.1 2001/01/10 08:27:12 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        instr

  Synopsis:
  	int instr(char *str);
  	int innstr(char *str, int n);
  	int winstr(WINDOW *win, char *str);
  	int winnstr(WINDOW *win, char *str, int n);
  	int mvinstr(int y, int x, char *str);
  	int mvinnstr(int y, int x, char *str, int n);
  	int mvwinstr(WINDOW *, int y, int x, char *str);
  	int mvwinnstr(WINDOW *, int y, int x, char *str, int n);

  System V Curses Description:
 	These routines return a string of characters in str, starting at the
 	current cursor position in the named window and ending at the right
 	margin of the window.  Attributes are stripped from the characters.
 	The four functions with n as the last argument return the string at most
 	n characters long.

 	Note:  instr(), innstr(), winstr(), mvinstr(), mvinnstr(),
 		mvwinstr() and mvwinnstr() are all macros.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      instr                                 -        -      4.0
      winstr                                -        -      4.0
      mvinstr                               -        -      4.0
      mvwinstr                              -        -      4.0
      innstr                                -        -      4.0
      winnstr                               -        -      4.0
      mvinnstr                              -        -      4.0
      mvwinnstr                             -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	instr(char *str)
#else
int	PDC_CDECL	instr(str)
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("instr() - called: string=\"%s\"\n",str);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return(winnstr(stdscr,str,stdscr->_maxx));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	innstr(char *str, int n)
#else
int	PDC_CDECL	innstr(str,n)
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("innstr() - called: n %d \n",n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return(winnstr(stdscr,str,n));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winstr(WINDOW *win, char *str)
#else
int	PDC_CDECL	winstr(win,str)
WINDOW *win;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winstr() - called: \n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	return(winnstr(win,str,win->_maxx));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winnstr(WINDOW *win, char *str, int n)
#else
int	PDC_CDECL	winnstr(win,str,n)
WINDOW *win;
char *str;
int n;
#endif
/***********************************************************************/
{
	chtype tmp;
	int oldy = win->_cury;
	int oldx = win->_curx;
	int imax = (win->_maxx - win->_curx);
	int ic;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winnstr() - called: n %d \n",n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if( n > 0 )
		imax = ((imax<n)?imax:n);

	for ( ic=0; ic < imax; ic++ )
	{
		tmp = mvwinch( win, oldy, oldx+ic );
		if( tmp == ERR ) 
		{
			*(str+imax) = '\0';
			return( ERR );
		}
		*(str+ic) = tmp & A_CHARTEXT;
	}

	*(str+imax) = '\0';

	win->_curx = oldx;

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinstr(int y, int x, char *str)
#else
int	PDC_CDECL	mvinstr(y,x,str)
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinstr() - called: y %d x %d \n",y,x);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (move(y,x) == ERR)
		return( ERR );

	return(winnstr(stdscr,str,stdscr->_maxx));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinnstr(int y, int x, char *str, int n)
#else
int	PDC_CDECL	mvinnstr(y,x,str,n)
int y;
int x;
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinnstr() - called: y %d x %d n %d \n",y,x,n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (move(y,x) == ERR)
		return( ERR );

	return(winnstr(stdscr,str,n));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinstr(WINDOW *win, int y, int x, char *str)
#else
int	PDC_CDECL	mvwinstr(win,y,x,str)
WINDOW *win;
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinstr() - called: y %d x %d \n",y,x);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	return(winnstr(win,str,win->_maxx));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinnstr(WINDOW *win,int y, int x, char *str, int n)
#else
int	PDC_CDECL	mvwinnstr(win,y,x,str,n)
WINDOW *win;
int y;
int x;
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinnstr() - called: y %d x %d n %d \n",y,x,n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	return(winnstr(win,str,n));
}
