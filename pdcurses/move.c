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
#undef	move
#undef	wmove

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_move  = "$Id: move.c,v 1.1 2001/01/10 08:27:16 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          move

  Synopsis:
  	int move(int y, int x);
  	int wmove(WINDOW *win, int y, int x);

  X/Open Description:
 	The cursor associated with the window is moved to the given
 	location.  This does not move the physical cursor of the
 	terminal until refresh() is called.  The position specified is
 	relative to the upper left corner of the window, which is (0,0).

 	NOTE: move() is a macro.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      move                                  Y        Y       Y
      wmove                                 Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	move(int y, int x)
#else
int	PDC_CDECL	move(y,x)
int y;
int x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("move() - called: y=%d x=%d\n",y,x);
#endif

	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	if ((x < 0) ||
	    (y < 0) ||
	    (x >= stdscr->_maxx) ||
	    (y >= stdscr->_maxy))
	{
		return( ERR );
	}
	stdscr->_curx = x;
	stdscr->_cury = y;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wmove(WINDOW *win, int y, int x)
#else
int	PDC_CDECL	wmove(win,y,x)
WINDOW *win;
int y;
int x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wmove() - called: y=%d x=%d\n",y,x);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if ((x < 0) ||
	    (y < 0) ||
	    (x >= win->_maxx) ||
	    (y >= win->_maxy))
	{
		return( ERR );
	}
	win->_curx = x;
	win->_cury = y;
	return( OK );
}
