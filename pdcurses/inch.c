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
#undef	inch
#undef	winch
#undef	mvinch
#undef	mvwinch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#endif

#ifdef PDCDEBUG
char *rcsid_inch  = "$Id: inch.c,v 1.1 2001/01/10 08:27:04 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          inch

  Synopsis:
  	chtype inch(void);
  	chtype winch(WINDOW *win);
  	chtype mvinch(int y, int x);
  	chtype mvwinch(WINDOW *win, int y, int x);

  X/Open Description:

 	NOTE: All these functions are implemented as macros.

  PDCurses Description:
 	Depending upon the state of the raw character output, 7- or
 	8-bit characters will be output.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      inch                                  Y        Y       Y
      winch                                 Y        Y       Y
      mvinch                                Y        Y       Y
      mvwinch                               Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
chtype	PDC_CDECL	inch(void)
#else
chtype	PDC_CDECL	inch()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("inch() - called\n");
#endif

	return( stdscr->_y[stdscr->_cury][stdscr->_curx] );
}
/***********************************************************************/
#ifdef HAVE_PROTO
chtype	PDC_CDECL	winch(WINDOW *win)
#else
chtype	PDC_CDECL	winch(win)
WINDOW *win;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winch() - called\n");
#endif

	return( win->_y[win->_cury][win->_curx] );
}
/***********************************************************************/
#ifdef HAVE_PROTO
chtype	PDC_CDECL	mvinch(int y, int x)
#else
chtype	PDC_CDECL	mvinch(y,x)
int y;
int x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinch() - called\n");
#endif

	(void)(move(y,x));
	return( stdscr->_y[stdscr->_cury][stdscr->_curx] );
}
/***********************************************************************/
#ifdef HAVE_PROTO
chtype	PDC_CDECL	mvwinch(WINDOW *win, int y, int x)
#else
chtype	PDC_CDECL	mvwinch(win,y,x)
WINDOW *win;
int y;
int x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinch() - called\n");
#endif

	(void)(wmove(win,y,x));
	return( win->_y[win->_cury][win->_curx] );
}
