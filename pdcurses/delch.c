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

#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif

/* undefine any macros for functions defined in this module */
#undef	delch
#undef	wdelch
#undef	mvdelch
#undef	mvwdelch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#endif

#ifndef HAVE_MEMMOVE
#  define memmove PDC_memmove
#endif


#ifdef PDCDEBUG
char *rcsid_delch  = "$Id: delch.c,v 1.1 2001/01/10 08:26:57 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         delch

  Synopsis:
  	int delch(void);
  	int wdelch(WINDOW *win);
  	int mvdelch(int y, int x);
  	int mvwdelch(WINDOW *win, int y, int x);

  X/Open Description:
 	The character under the cursor in the window is deleted.  All
 	characters to the right on the same line are moved to the left
 	one position and the last character on the line is filled with
 	a blank.  The cursor position does not change (after moving to
 	y, x if coordinates are specified).

 	NOTE: delch(), mvdelch(), and mvwdelch() are implemented as macros.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  NOTE:
 	The behaviour of Unix curses is to display a blank in the last
 	column of the window with the A_NORMAL attribute. PDCurses
 	displays the blank with the window's current attributes 
 	(including current colour). To get the behaviour of PDCurses,
 	#define PDCURSES_WCLR in curses.h or add -DPDCURSES_WCLR to the 
 	compile switches.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      delch                                 Y        Y       Y
      wdelch                                Y        Y       Y
      mvdelch                               Y        Y       Y
      mvwdelch                              Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	delch(void)
#else
int	PDC_CDECL	delch()
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("delch() - called\n");
#endif

	return (wdelch(stdscr));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wdelch(WINDOW *win)
#else
int	PDC_CDECL	wdelch(win)
WINDOW *win;
#endif
/***********************************************************************/
{
	int		y;
	int		x;
	int		maxx;
	chtype*		temp1;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wdelch() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return (ERR);

	y	= win->_cury;
	x	= win->_curx;
	maxx	= win->_maxx - 1;
	temp1	= &win->_y[y][x];

	memmove( temp1, temp1 + 1, (maxx - x) * sizeof(chtype) );

#if defined(PDCURSES_WCLR)
	win->_y[y][maxx]	= win->_blank | win->_attrs;
#else
/* wrs (4/10/93) account for window background */
	win->_y[y][maxx]	= win->_bkgd;
#endif

	win->_lastch[y] = maxx;

	if ((win->_firstch[y] == _NO_CHANGE) ||
	    (win->_firstch[y] > x))
	{
		win->_firstch[y] = x;
	}
	PDC_sync(win);
	return (OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvdelch(int y, int x)
#else
int	PDC_CDECL	mvdelch(y,x)
int y;
int x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvdelch() - called\n");
#endif

	if (move(y,x) == ERR)
		return(ERR);
	return(delch());
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwdelch(WINDOW *win, int y, int x)
#else
int	PDC_CDECL	mvwdelch(win,y,x)
WINDOW *win;
int y;
int x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwdelch() - called\n");
#endif

	if (wmove(win,y,x) == ERR)
		return(ERR);
	return(wdelch(win));
}
