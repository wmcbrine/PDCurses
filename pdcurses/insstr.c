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

#ifdef HAVE_STRING_H
# include <string.h>
#endif

/* undefine any macros for functions defined in this module */
#undef	insstr
#undef	insnstr
#undef	winsstr
#undef	winsnstr
#undef	mvinsstr
#undef	mvinsnstr
#undef	mvwinsstr
#undef	mvwinsnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#  undef	addch
#  undef	waddch
#endif

#ifdef PDCDEBUG
char *rcsid_insstr  = "$Id: insstr.c,v 1.2 2002/03/22 22:36:52 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        insstr

  Synopsis:
  	int insstr(char *str);
  	int insnstr(char *str, int n);
  	int winsstr(WINDOW *win, char *str);
  	int winsnstr(WINDOW *win, char *str, int n);
  	int mvinsstr(int y, int x, char *str);
  	int mvinsnstr(int y, int x, char *str, int n);
  	int mvwinsstr(WINDOW *, int y, int x, char *str);
  	int mvwinsnstr(WINDOW *, int y, int x, char *str, int n);

  System V Curses Description:
  	With these routines, a character string (as many characters as 
  	will fit on the line) is inserted before the character under 
  	the cursor.  All characters to the right of the cursor are moved 
  	to the right, with the possibility of the rightmost characters 
  	on the line being lost.  The cursor position does not change 
  	(after moving to y,x if specified).  The four routines with n as 
  	the last argument insert at most n characters.  If n<=0, then 
  	the entire string is inserted.

 	NOTE:	insstr(), mvinsstr(), and mvwinsstr() are implemented as macros.
 		insnstr(), mvinsnstr(), and mvwinsnstr() are implemented as macros.

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
      insstr                                -        -      4.0
      winsstr                               -        -      4.0
      mvinsstr                              -        -      4.0
      mvwinsstr                             -        -      4.0
      insnstr                               -        -      4.0
      winsnstr                              -        -      4.0
      mvinsnstr                             -        -      4.0
      mvwinsnstr                            -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	insstr(char *str)
#else
int	PDC_CDECL	insstr(str)
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("insstr() - called: string=\"%s\"\n",str);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return(winsnstr(stdscr,str,(-1)));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	insnstr(char *str, int n)
#else
int	PDC_CDECL	insnstr(str,n)
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("insnstr() - called: string=\"%s\" n %d \n",str,n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return(winsnstr(stdscr,str,n));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winsstr(WINDOW *win, char *str)
#else
int	PDC_CDECL	winsstr(win,str)
WINDOW *win;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winsstr() - called: string=\"%s\"\n",str);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	return(winsnstr(win,str,(-1)));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winsnstr(WINDOW *win, char *str, int n)
#else
int	PDC_CDECL	winsnstr(win,str,n)
WINDOW *win;
char *str;
int n;
#endif
/***********************************************************************/
{
	int ic = strlen(str);

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winsnstr() - called: string=\"%s\" n %d \n",str,n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if( n > 0 )
		ic = ((ic<n)?ic:n) - 1;
	else
		ic = ic - 1;

	for ( ; ic >= 0; ic-- )
	{
		if (winsch(win, *(str+ic) ) == ERR)
		{
			return( ERR );
		}
	}
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinsstr(int y, int x, char *str)
#else
int	PDC_CDECL	mvinsstr(y,x,str)
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinsstr() - called: y %d x %d string=\"%s\"\n",y,x,str);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (move(y,x) == ERR)
		return( ERR );

	return(winsnstr(stdscr,str,(-1)));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinsnstr(int y, int x, char *str, int n)
#else
int	PDC_CDECL	mvinsnstr(y,x,str,n)
int y;
int x;
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinsnstr() - called: y %d x %d string=\"%s\" n %d \n",y,x,str,n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (move(y,x) == ERR)
		return( ERR );

	return(winsnstr(stdscr,str,n));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinsstr(WINDOW *win, int y, int x, char *str)
#else
int	PDC_CDECL	mvwinsstr(win,y,x,str)
WINDOW *win;
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winsstr() - called: string=\"%s\"\n",str);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	return(winsnstr(win,str,(-1)));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinsnstr(WINDOW *win,int y, int x, char *str, int n)
#else
int	PDC_CDECL	mvwinsnstr(win,y,x,str,n)
WINDOW *win;
int y;
int x;
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinsnstr() - called: y %d x %d string=\"%s\" n %d \n",y,x,str,n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	return(winsnstr(win,str,n));
}
