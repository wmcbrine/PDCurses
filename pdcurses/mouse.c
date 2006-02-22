/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef mouse_set
#undef mouse_on
#undef mouse_off
#undef request_mouse_pos
#undef map_button
#undef wmouse_position
#undef getmouse
#undef getbmap

#ifdef PDCDEBUG
const char *rcsid_mouse =
	"$Id: mouse.c,v 1.16 2006/02/22 06:09:47 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         mouse

  Synopsis:

	int mouse_set(long);
	int mouse_on(long);
	int mouse_off(long);
	int request_mouse_pos(void);
	int map_button(unsigned long);
	void wmouse_position(WINDOW *, int *, int *);
	unsigned long getmouse(void);
	unsigned long getbmap(void);

  PDCurses Description:
	ALL DESCRIPTIONS ARE GUESSES. I DON'T KNOW ANYONE WHO KNOWS
	EXACTLY WHAT THESE FUNCTIONS DO!

	The mouse_set(), mouse_on() and mouse_off() functions are 
	analagous to the attrset(), attron() and attroff() functions.  
	These functions set the mouse button events to trap.  The button 
	masks used in these functions are defined in curses.h and can be 
	or'ed together. They are the group of masks starting with 
	BUTTON1_RELEASED.

	The request_mouse_pos() function requests curses to fill in the
	Mouse_status structure with the current state of the mouse.

	The map_button() function enables the mouse requested mouse 
	action to activate the Soft Label Keys if the mouse action 
	occurs over the area of the screen where the Soft Label Keys are 
	displayed.  The mouse actions are defined in curses.h and are 
	the group that start with BUTTON_RELEASED.

	The wmouse_position() function determines if the current mouse 
	position is within the window passed as an argument.  If the 
	mouse is outside the current window, -1 is returned in the y and 
	x arguments otherwise the y and x coordinates of the mouse 
	(relative to the top left corner of the window) are returned in 
	y and x.

	The getmouse() function returns the current status of the trapped
	mouse buttons as set by mouse_set(), mouse_on();

	The getbmap() function returns the current status of the button 
	action used to map a mouse action to the Soft Label Keys as set 
	by the map_button() function.

  PDCurses Errors:
	None.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	mouse_set				-	-      4.0
	mouse_on				-	-      4.0
	mouse_off				-	-      4.0
	request_mouse_pos			-	-      4.0
	map_button				-	-      4.0
	wmouse_position				-	-      4.0
	getmouse				-	-      4.0
	getbmap					-	-      4.0

**man-end**********************************************************************/

int mouse_set(unsigned long mbe)
{
	PDC_LOG(("mouse_set() - called: event %x\n", mbe));

	SP->_trap_mbe = mbe;

	return OK;
}

int mouse_on(unsigned long mbe)
{
	PDC_LOG(("mouse_on() - called: event %x\n", mbe));

	SP->_trap_mbe |= mbe;

	return OK;
}

int mouse_off(unsigned long mbe)
{
	PDC_LOG(("mouse_off() - called: event %x\n", mbe));

	SP->_trap_mbe &= ~mbe;

	return OK;
}

int map_button(unsigned long button)
{
	PDC_LOG(("map_button() - called: button %x\n", button));

/****************** this does nothing at the moment ***************/
	SP->_map_mbe_to_key = button;

	return OK;
}

int request_mouse_pos(void)
{
	extern MOUSE_STATUS Trapped_Mouse_status;

	PDC_LOG(("request_mouse_pos() - called\n"));

	memcpy((char*)&Mouse_status, (char*)&Trapped_Mouse_status,
		sizeof(MOUSE_STATUS));

	return OK;
}

void wmouse_position(WINDOW *win, int *y, int *x)
{
	PDC_LOG(("wmouse_position() - called\n"));

	/* if the current mouse position is outside the provided window, 
	   put -1 in x and y */

	if (win == (WINDOW *)NULL)
	{
		*y = *x = -1;
		return;
	}

	if (win->_begy > MOUSE_Y_POS || win->_begx > MOUSE_X_POS 
	    || win->_begy + win->_maxy <= MOUSE_Y_POS
	    || win->_begx + win->_maxx <= MOUSE_X_POS)
	{
		*x = *y = -1;
	}
	else
	{
		*x = MOUSE_X_POS - win->_begx;
		*y = MOUSE_Y_POS - win->_begy;
	}
}

unsigned long getmouse(void)
{
	PDC_LOG(("getmouse() - called\n"));

	return SP->_trap_mbe;
}

unsigned long getbmap(void)
{
	PDC_LOG(("getbmap() - called\n"));

	return SP->_map_mbe_to_key;
}
