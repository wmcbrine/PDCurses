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
#undef	inchstr
#undef	inchnstr
#undef	winchstr
#undef	winchnstr
#undef	mvinchstr
#undef	mvinchnstr
#undef	mvwinchstr
#undef	mvwinchnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#endif

#ifdef PDCDEBUG
char *rcsid_inchstr  = "$Id: inchstr.c,v 1.1 2001/01/10 08:27:05 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        inchstr

  Synopsis:
  	int inchstr(chtype *ch);
  	int inchnstr(chtype *ch, int n);
  	int winchstr(WINDOW *win, chtype *ch);
  	int winchnstr(WINDOW *win, chtype *ch, int n);
  	int mvinchstr(int y, int x, chtype *ch);
  	int mvinchnstr(int y, int x, chtype *ch, int n);
  	int mvwinchstr(WINDOW *, int y, int x, chtype *ch);
  	int mvwinchnstr(WINDOW *, int y, int x, chtype *ch, int n);

  X/Open Description:
 	These routines read a chtype string directly from the window structure
 	starting at the current position and ending at the right margin.
 	The four routines with n as the last argument copy at most n
 	elements, but no more than will fit on the line.

 	NOTE:	inchstr(), mvinchstr(), mvwinchstr() inchnstr(), 
 		mvinchnstr(), and mvwinchnstr() are implemented as macros.

  X/Open Return Value:
 	All functions return ERR on error and an integer value other than
 	ERR on success. 

  X/Open Errors:
 	Normally the value returned indicates the number of chtypes
 	returned.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      inchstr                               -        -      4.0
      winchstr                              -        -      4.0
      mvinchstr                             -        -      4.0
      mvwinchstr                            -        -      4.0
      inchnstr                              -        -      4.0
      winchnstr                             -        -      4.0
      mvinchnstr                            -        -      4.0
      mvwinchnstr                           -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	inchstr(chtype *ch)
#else
int	PDC_CDECL	inchstr(ch)
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("inchstr() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return( inchnstr( ch, stdscr->_maxx - stdscr->_curx ) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	inchnstr(chtype *ch, int n)
#else
int	PDC_CDECL	inchnstr(ch,n)
chtype *ch;
int n;
#endif
/***********************************************************************/
{
	chtype	*ptr = &(stdscr->_y[stdscr->_cury][stdscr->_curx]);
	int	i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("inchnstr() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (n < 0)
		return( ERR );

	if ((stdscr->_curx + n) > stdscr->_maxx)
		n = stdscr->_maxx - stdscr->_curx;

	for(i=0;i<n;i++)
		*ch++ = *ptr++;

	*ch = (chtype)0;
	return(i);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winchstr(WINDOW *win, chtype *ch)
#else
int	PDC_CDECL	winchstr(win,ch)
WINDOW *win;
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winchstr() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	return(winchnstr(win,ch,win->_maxx - win->_curx));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winchnstr(WINDOW *win, chtype *ch, int n)
#else
int	PDC_CDECL	winchnstr(win,ch,n)
WINDOW *win;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
	chtype	*ptr = &(win->_y[win->_cury][win->_curx]);
	int	i;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winchnstr() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (n < 0)
		return( ERR );

	if ((win->_curx + n) > win->_maxx)
		n = win->_maxx - win->_curx;

	for(i=0;i<n;i++)
		*ch++ = *ptr++;

	*ch = (chtype)0;
	return(i);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinchstr(int y, int x, chtype *ch)
#else
int	PDC_CDECL	mvinchstr(y,x,ch)
int y;
int x;
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinchstr() - called: y %d x %d\n",y,x);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (wmove(stdscr,y,x) == ERR)
		return( ERR );

	return( inchnstr( ch, stdscr->_maxx - stdscr->_curx) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinchnstr(int y, int x, chtype *ch, int n)
#else
int	PDC_CDECL	mvinchnstr(y,x,ch,n)
int y;
int x;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinchnstr() - called: y %d x %d n %d\n",y,x,n);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if (wmove(stdscr,y,x) == ERR)
		return( ERR );

	return( inchnstr( ch, n) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinchstr(WINDOW *win, int y, int x, chtype *ch)
#else
int	PDC_CDECL	mvwinchstr(win,y,x,ch)
WINDOW *win;
int y;
int x;
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winchstr() - called:\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	return( winchnstr( win, ch, win->_maxx - win->_curx) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinchnstr(WINDOW *win,int y, int x, chtype *ch, int n)
#else
int	PDC_CDECL	mvwinchnstr(win,y,x,ch,n)
WINDOW *win;
int y;
int x;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinchnstr() - called: y %d x %d n %d \n",y,x,n);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if (wmove(win,y,x) == ERR)
		return( ERR );

	return( winchnstr( win, ch, n) );
}
