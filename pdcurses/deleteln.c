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
#undef	deleteln
#undef	wdeleteln
#undef	insdelln
#undef	winsdelln
#undef	insertln
#undef	winsertln
#undef	mvwinsertln

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_deleteln  = "$Id: deleteln.c,v 1.1 2001/01/10 08:26:58 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                      deleteln

  Synopsis:
  	int deleteln(void);
  	int wdeleteln(WINDOW *win);
  	int insdelln(int n);
  	int winsdelln(WINDOW *win, int n);
  	int insertln(void);
  	int winsertln(WINDOW *win);
  	int mvwinsertln(WINDOW *win, int y, int x);

  X/Open Description:
 	With the deleteln() and wdelteln() functions,
 	the line under the cursor in the window is deleted.  All
 	lines below the current line are moved up one line.  The
 	bottom line of the window is cleared.  The cursor position
 	does not change.

 	With the insertln() and winsertn() functions,
 	a blank line is inserted above the current line and the bottom
 	line is lost.

 	NOTE: deleteln() and insertln() are implemented as macros.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  NOTE:
 	The behaviour of Unix curses is to clear the line with a space
 	and attributes of A_NORMAL. PDCurses clears the line with the
 	window's current attributes (including current colour). To get
 	the behaviour of PDCurses, #define PDCURSES_WCLR in curses.h or
 	add -DPDCURSES_WCLR to the compile switches.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      deleteln                              Y        Y       Y
      wdeleteln                             Y        Y       Y
      insdelln                              -        -      4.0
      winsdelln                             -        -      4.0
      insertln                              Y        Y       Y
      winsertln                             Y        Y       Y
      mvwinsertln                           -        -       -

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	deleteln(void)
#else
int	PDC_CDECL	deleteln()
#endif
/***********************************************************************/
{
	chtype	blank;
	chtype*	temp;
	chtype*	ptr;
	int	y;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("deleteln() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

#if defined(PDCURSES_WCLR)
	blank	= stdscr->_blank | stdscr->_attrs;
#else
/* wrs (4/10/93) account for window background */
	blank	= stdscr->_bkgd;
#endif
	temp	= stdscr->_y[stdscr->_cury];

	for (y = stdscr->_cury; y < stdscr->_bmarg; y++)
	{
		stdscr->_y[y]	 = stdscr->_y[y + 1];
		stdscr->_firstch[y] = 0;
		stdscr->_lastch[y] = stdscr->_maxx - 1;
	}

	for (ptr = temp; (ptr - temp < stdscr->_maxx); ptr++)
		*ptr = blank;			/* make a blank line */

	if( stdscr->_cury <= stdscr->_bmarg ) 
	{
		stdscr->_firstch[stdscr->_bmarg]	= 0;
		stdscr->_lastch[stdscr->_bmarg]	= stdscr->_maxx - 1;
		stdscr->_y[stdscr->_bmarg]		= temp;
	}

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wdeleteln(WINDOW *win)
#else
int	PDC_CDECL	wdeleteln(win)
WINDOW *win;
#endif
/***********************************************************************/
{
	chtype	blank;
	chtype*	temp;
	chtype*	ptr;
	int	y;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wdeleteln() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

#if defined(PDCURSES_WCLR)
	blank	= win->_blank | win->_attrs;
#else
/* wrs (4/10/93) account for window background */
	blank	= win->_bkgd;
#endif
	temp	= win->_y[win->_cury];

	for (y = win->_cury; y < win->_bmarg; y++)
	{
		win->_y[y]	 = win->_y[y + 1];
		win->_firstch[y] = 0;
		win->_lastch[y] = win->_maxx - 1;
	}

	for (ptr = temp; (ptr - temp < win->_maxx); ptr++)
		*ptr = blank;			/* make a blank line */

	if( win->_cury <= win->_bmarg ) 
	{
		win->_firstch[win->_bmarg]	= 0;
		win->_lastch[win->_bmarg]	= win->_maxx - 1;
		win->_y[win->_bmarg]		= temp;
	}

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	insdelln(int n)
#else
int	PDC_CDECL	insdelln(n)
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("insdelln() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return(winsdelln(stdscr,n));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winsdelln(WINDOW *win, int n)
#else
int	PDC_CDECL	winsdelln(win,n)
WINDOW *win;
int n;
#endif
/***********************************************************************/
{
	int i;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winsdelln() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if( n > 0 ) {
		for(i=0; i<n; i++) {
			if( winsertln(win) == ERR )
				return ERR;
		}
	}
	else if( n < 0 ) {
		n = -n;
		for(i=0; i<n; i++) {
			if( wdeleteln(win) == ERR )
				return ERR;
		}
	}

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winsertln(WINDOW *win)
#else
int	PDC_CDECL	winsertln(win)
WINDOW *win;
#endif
/***********************************************************************/
{
	chtype	blank;
	chtype*	temp;
	chtype*	end;
	short	y;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winsertln() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

#if defined(PDCURSES_WCLR)
	blank	= win->_blank | win->_attrs;
#else
/* wrs (4/10/93) account for window background */
	blank	= win->_bkgd;
#endif
	temp	= win->_y[win->_maxy-1];

	for (y = win->_maxy-1; y > win->_cury; y--)
	{
		win->_y[y]	 = win->_y[y - 1];
		win->_firstch[y] = 0;
		win->_lastch[y] = win->_maxx - 1;
	}

	win->_y[win->_cury] = temp;

	for (end = &temp[win->_maxx - 1]; temp <= end; temp++)
	{
		*temp = blank;
	}

	win->_firstch[win->_cury] = 0;
	win->_lastch[win->_cury] = win->_maxx - 1;

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	insertln(void)
#else
int	PDC_CDECL	insertln()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("insertln() - called\n");
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	return(winsertln(stdscr));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinsertln(WINDOW *win, int y, int x)
#else
int	PDC_CDECL	mvwinsertln(win, y, x)
WINDOW *win;
int y,x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinsertln() - called\n");
#endif

	if (wmove(win,y,x) == ERR)
		return(ERR);
	return(winsertln(win));
}
