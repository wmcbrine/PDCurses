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
#undef	attroff
#undef	wattroff
#undef	attron
#undef	wattron
#undef	attrset
#undef	wattrset
#undef	standend
#undef	standout
#undef	wstandend
#undef	wstandout

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_attr  = "$Id: attr.c,v 1.1 2001/01/10 08:26:48 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          attr

  Synopsis:
  	int attroff(attr_t attrs);
  	int wattroff(WINDOW *win, attr_t attrs);
  	int attron(attr_t attrs);
  	int wattron(WINDOW *win, attr_t attrs);
  	int attrset(attr_t attrs);
  	int wattrset(WINDOW *win, attr_t attrs);
  	int standend(void);
  	int wstandend(WINDOW *win);
  	int standout(void);
  	int wstandout(WINDOW *win);

  X/Open Description:
 	These functions manipulate the current attributes of the named
 	window.  These attributes can be any combination of A_STANDOUT,
 	A_REVERSE, A_BOLD, A_DIM, A_BLINK, A_UNDERLINE.

 	These constants are defined in <curses.h> and can be combined
 	with the bitwise-OR operator (|).

 	The current attributes of a window are applied to all characters
 	that are written into the window with waddch().  Attributes are
 	a property of the character, and move with the character
 	through any scrolling and insert/delete line/character operations.
 	To the extent possible on the particular terminal, they will be
 	displayed as the graphic rendition of characters put on the
 	screen.

 	The attrset() function sets the current attributes of the given
 	window to attrs.  The attroff() function turns off the named
 	attributes without turning on or off any other attributes.  The
 	attron() function turs on the named attriutes without affecting
 	any others.  The standout() function is the same as
 	attron( A_STANDOUT ).  The standend() function is the same as
 	attrset( A_NORMAL ); that is it turns off all attributes.

 	NOTE:	attroff(), attron(), attrset(), standend(), standout(),
 		wstandend(), and wstandout() are defined as macros.

  PDCurses Description:
 	System V compatible color support is included.
 	See <curses.h> for further details.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	It is an error to call this function with a NULL window pointer.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      attroff                               Y        Y       Y
      wattroff                              Y        Y       Y
      attron                                Y        Y       Y
      wattron                               Y        Y       Y
      attrset                               Y        Y       Y
      wattrset                              Y        Y       Y
      standend                              Y        Y       Y
      wstandend                             Y        Y       Y
      standout                              Y        Y       Y
      wstandout                             Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	attroff(attr_t attrs)
#else
int	PDC_CDECL	attroff(attrs)
chtype attrs;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("attroff() - called\n");
#endif
	if (stdscr == (WINDOW *)NULL)
		return( ERR );

	stdscr->_attrs &= (~attrs & A_ATTRIBUTES);
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wattroff(WINDOW *win, attr_t attrs)
#else
int	PDC_CDECL	wattroff(win,attrs)
WINDOW *win;
attr_t attrs;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wattroff() - called\n");
#endif
	if (win == (WINDOW *)NULL)
		return( ERR );

	win->_attrs &= (~attrs & A_ATTRIBUTES);
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	attron(attr_t attrs)
#else
int	PDC_CDECL	attron(attrs)
attr_t attrs;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("attron() - called\n");
#endif
	if (stdscr == (WINDOW *)NULL)
		return( ERR );
	return(wattron(stdscr,attrs));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wattron(WINDOW *win, attr_t attrs)
#else
int	PDC_CDECL	wattron(win,attrs)
WINDOW *win;
attr_t attrs;
#endif
/***********************************************************************/
{
	attr_t newcolr, oldcolr;
	attr_t newattr, oldattr;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wattron() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );
	if( (win->_attrs & A_COLOR) && (attrs & A_COLOR) ) 
		{
		oldcolr = win->_attrs & A_COLOR;
		oldattr = win->_attrs ^ oldcolr;
		newcolr = attrs & A_COLOR;
		newattr = (attrs & A_ATTRIBUTES) ^ newcolr;
		newattr |= oldattr;
		win->_attrs = newattr | newcolr;
		}
	else
		win->_attrs |= (attrs & A_ATTRIBUTES);
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	attrset(attr_t attrs)
#else
int	PDC_CDECL	attrset(attrs)
attr_t attrs;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("attrset() - called\n");
#endif
	if (stdscr == (WINDOW *)NULL)
		return( ERR );
	stdscr->_attrs = attrs & A_ATTRIBUTES;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wattrset(WINDOW *win, attr_t attrs)
#else
int	PDC_CDECL	wattrset(win,attrs)
WINDOW *win;
attr_t attrs;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wattrset() - called\n");
#endif
	if (win == (WINDOW *)NULL)
		return( ERR );
	win->_attrs = attrs & A_ATTRIBUTES;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	standend(void)
#else
int	PDC_CDECL	standend()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("standend() - called\n");
#endif
	return( wattrset( stdscr, A_NORMAL ) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	standout(void)
#else
int	PDC_CDECL	standout()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("standout() - called\n");
#endif
	return( wattrset( stdscr, A_STANDOUT ) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wstandend( WINDOW *win )
#else
int	PDC_CDECL	wstandend(win)
WINDOW *win;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wstandend() - called\n");
#endif
	return( wattrset( win, A_NORMAL ) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wstandout( WINDOW *win )
#else
int	PDC_CDECL	wstandout(win)
WINDOW *win;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wstandout() - called\n");
#endif
	return( wattrset( win, A_STANDOUT ) );
}
