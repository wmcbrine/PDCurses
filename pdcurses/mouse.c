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
#undef	mouse_set
#undef	mouse_on
#undef	mouse_off
#undef	request_mouse_pos
#undef	map_button
#undef	wmouse_position
#undef	getmouse
#undef	getbmap

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_mouse = "$Id: mouse.c,v 1.2 2002/03/22 22:36:53 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         mouse

  Synopsis:

  	int mouse_set( long );
  	int mouse_on( long );
  	int mouse_off( long );
  	int request_mouse_pos( void );
  	int map_button( unsigned long );
  	void wmouse_position( WINDOW *, int *, int * );
  	unsigned long getmouse( void );
  	unsigned long getbmap( void );

  PDCurses Description:
 	ALL DESCRIPTIONS ARE GUESSES. I DON'T KNOW ANYONE WHO KNOWS
 	EXACTLY WHAT THESE FUNCTIONS DO!

 	The mouse_set(), mouse_on() and mouse_off() functions are analagous to
 	the attrset(), attron() and attroff() functions.  These functions
 	set the mouse button events to trap.  The button masks used in these
 	functions are defined in curses.h and can be or'ed together. They are
 	the group of masks starting with BUTTON1_RELEASED.

 	The request_mouse_pos() function requests curses to fill in the
 	Mouse_status structure with the current state of the mouse.

 	The map_button() function enables the mouse requested mouse action
 	to activate the Soft Label Keys if the mouse action occurs over the
 	area of the screen where the Soft Label Keys are displayed.  The
 	mouse actions are defined in curses.h and are the group that start
 	with BUTTON_RELEASED.

 	The wmouse_position() function determines if the current mouse
 	position is within the window passed as an argument.  If the mouse
 	is outside the current window, -1 is returned in the y and x arguments
 	otherwise the y and x coordinates of the mouse (relative to the top
 	left corner of the window) are returned in y and x.

 	The getmouse() function returns the current status of the trapped
 	mouse buttons as set by mouse_set(), mouse_on();

 	The getbmap() function returns the current status of the button action
 	used to map a mouse action to the Soft Label Keys as set by the
 	map_button() function.

  PDCurses Errors:
 	None.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      mouse_set                             -        -      4.0
      mouse_on                              -        -      4.0
      mouse_off                             -        -      4.0
      request_mouse_pos                     -        -      4.0
      map_button                            -        -      4.0
      wmouse_position                       -        -      4.0
      getmouse                              -        -      4.0
      getbmap                               -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mouse_set(unsigned long mbe)
#else
int	PDC_CDECL	mouse_set(mbe)
unsigned long mbe;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mouse_set() - called: event %x\n",mbe);
#endif
	SP->_trap_mbe = mbe;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mouse_on(unsigned long mbe)
#else
int	PDC_CDECL	mouse_on(mbe)
unsigned long mbe;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mouse_on() - called: event %x\n",mbe);
#endif
	SP->_trap_mbe |= mbe;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mouse_off(unsigned long mbe)
#else
int	PDC_CDECL	mouse_off(mbe)
unsigned long mbe;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mouse_off() - called: event %x\n",mbe);
#endif
	SP->_trap_mbe &= ~mbe;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	map_button(unsigned long button)
#else
int	PDC_CDECL	map_button(button)
unsigned long button;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("map_button() - called: button %x\n",button);
#endif
/****************** this does nothing at the moment ***************/
	SP->_map_mbe_to_key = button;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	request_mouse_pos(void)
#else
int	PDC_CDECL	request_mouse_pos()
#endif
/***********************************************************************/
{
extern MOUSE_STATUS Trapped_Mouse_status;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("request_mouse_pos() - called\n");
#endif
	memcpy((char*)&Mouse_status,(char*)&Trapped_Mouse_status,sizeof(MOUSE_STATUS));
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
void	PDC_CDECL	wmouse_position(WINDOW *win, int *y, int *x)
#else
void	PDC_CDECL	wmouse_position(win,y,x)
WINDOW *win;
int *y,*x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wmouse_position() - called\n");
#endif
/* if the current mouse position is outside the provided window, put  */
/* -1 in x and y                                                      */
	if (win == (WINDOW *)NULL)
	{
		*y = *x = (-1);
		return;
	}
	if (win->_begy > MOUSE_Y_POS 
	||  win->_begx > MOUSE_X_POS 
	||  win->_begy+win->_maxy <= MOUSE_Y_POS
	||  win->_begx+win->_maxx <= MOUSE_X_POS)
	{
		*x = *y = (-1);
	}
	else
	{
		*x = MOUSE_X_POS - win->_begx;
		*y = MOUSE_Y_POS - win->_begy;
	}
	return;
}
/***********************************************************************/
#ifdef HAVE_PROTO
unsigned long	PDC_CDECL	getmouse(void)
#else
unsigned long	PDC_CDECL	getmouse()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("getmouse() - called\n");
#endif
	return(SP->_trap_mbe);
}
/***********************************************************************/
#ifdef HAVE_PROTO
unsigned long	PDC_CDECL	getbmap(void)
#else
unsigned long	PDC_CDECL	getbmap()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("getbmap() - called\n");
#endif
	return(SP->_map_mbe_to_key);
}
