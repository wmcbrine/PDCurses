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
#undef	clear
#undef	wclear
#undef	erase
#undef	werase
#undef	clrtobot
#undef	wclrtobot
#undef	clrtoeol
#undef	wclrtoeol

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_clear  = "$Id: clear.c,v 1.1 2001/01/10 08:26:54 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         clear

  Synopsis:
  	int clear(void);
  	int wclear(WINDOW *win);
  	int erase(void);
  	int werase(WINDOW *win);
  	int clrtobot(void);
  	int wclrtobot(WINDOW *win);
  	int clrtoeol(void);
  	int wclrtoeol(WINDOW *win);

  X/Open Description:
  	The erase() and werase() functions copy blanks to every position
  	of the window.

  	The clear() and wclear() functions are similar to erase() and
  	werase() except they also call clearok() to ensure that the
  	the screen is cleared on the next call to wrefresh() for that
  	window.

  	The clrtobot() and wclrtobot() functions clear the screen from
  	the current cursor position to the end of the current line and
  	all remaining lines in the window.

  	The clrtoeol() and wclrtoeol() functions clear the screen from
  	the current cursor position to the end of the current line only.

 	NOTE: clear(), wclear(), erase(), clrtobot(), and clrtoeol()
 	are implemented as macros

  PDCurses Description:

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
      clear                                 Y        Y       Y
      wclear                                Y        Y       Y
      erase                                 Y        Y       Y
      werase                                Y        Y       Y
      clrtobot                              Y        Y       Y
      wclrtobot                             Y        Y       Y
      clrtoeol                              Y        Y       Y
      wclrtoeol                             Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	clear(void)
#else
int	PDC_CDECL	clear()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("clear() - called\n");
#endif

	if  (stdscr == (WINDOW *)NULL)
		return(ERR);

	stdscr->_clear = TRUE;
	return(erase());
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wclear( WINDOW *win )
#else
int	PDC_CDECL	wclear(win)
WINDOW *win;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wclear() - called\n");
#endif

	if  (win == (WINDOW *)NULL)
		return( ERR );

	win->_clear = TRUE;
	return( werase( win ) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	erase(void)
#else
int	PDC_CDECL	erase()
#endif
/***********************************************************************/
{
	return( werase(stdscr) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	werase(WINDOW *win)
#else
int	PDC_CDECL	werase(win)
WINDOW *win;
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("werase() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	(void)wmove(win,0,0);
	return (wclrtobot(win));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	clrtobot(void)
#else
int	PDC_CDECL	clrtobot()
#endif
/***********************************************************************/
{
	return( wclrtobot(stdscr) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wclrtobot(WINDOW *win)
#else
int	PDC_CDECL	wclrtobot(win)
WINDOW *win;
#endif
/***********************************************************************/
{
	int	savey=win->_cury;
	int	savex=win->_curx;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wclrtobot() - called\n");
#endif

	if  (win == (WINDOW *)NULL)
		return( ERR );

/* should this involve scrolling region somehow ? */

	if (win->_cury + 1 < win->_maxy)
		{
		win->_curx = 0;
		win->_cury++;
		for ( ; win->_maxy > win->_cury; win->_cury++)
			(void)wclrtoeol(win);
		win->_cury = savey;
		win->_curx = savex;
		}
	(void)wclrtoeol(win);

	PDC_sync(win);
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	clrtoeol(void)
#else
int	PDC_CDECL	clrtoeol()
#endif
/***********************************************************************/
{
	return( wclrtoeol(stdscr) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wclrtoeol(WINDOW *win)
#else
int	PDC_CDECL	wclrtoeol(win)
WINDOW *win;
#endif
/***********************************************************************/
{
	int	y=0;
	int	x=0;
	int	minx=0;
	chtype	blank=0;
	chtype*	ptr=NULL;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wclrtoeol() - called: Row: %d Col: %d\n",win->_cury,win->_curx);
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	y	= win->_cury;
	x	= win->_curx;

#if defined(PDCURSES_WCLR)
	blank	= win->_blank | win->_attrs;
#else
/* wrs (4/10/93) account for window background */
	blank	= win->_bkgd;
#endif

	for (minx=x,ptr=&win->_y[y][x];minx<win->_maxx;minx++,*ptr++)
		*ptr = blank;

	win->_firstch[y] = (win->_firstch[y]==_NO_CHANGE)?x:min(x,win->_firstch[y]);
	win->_lastch[y] = max(win->_lastch[y],win->_maxx-1);

	PDC_sync(win);
	return( OK );
}
