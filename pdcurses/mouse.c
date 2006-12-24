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
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include <curspriv.h>
#include <string.h>

RCSID("$Id: mouse.c,v 1.32 2006/12/24 16:48:53 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                         mouse

  Synopsis:

	int mouse_set(unsigned long mbe);
	int mouse_on(unsigned long mbe);
	int mouse_off(unsigned long mbe);
	int request_mouse_pos(void);
	int map_button(unsigned long button);
	void wmouse_position(WINDOW *win, int *y, int *x);
	unsigned long getmouse(void);
	unsigned long getbmap(void);

	int mouseinterval(int wait);
	bool wenclose(const WINDOW *win, int y, int x);

  PDCurses Description:
	These functions are intended to be based on Sys V mouse 
	functions; however, those are undocumented.

	mouse_set(), mouse_on() and mouse_off() are analagous to 
	attrset(), attron() and attroff().  These functions set the 
	mouse button events to trap.  The button masks used in these 
	functions are defined in curses.h and can be or'ed together. 
	They are the group of masks starting with BUTTON1_RELEASED.

	request_mouse_pos() requests curses to fill in the Mouse_status 
	structure with the current state of the mouse.

	map_button() enables the specified mouse action to activate the 
	Soft Label Keys if the action occurs over the area of the screen 
	where the Soft Label Keys are displayed.  The mouse actions are 
	defined in curses.h in the group that starts with BUTTON_RELEASED.

	wmouse_position() determines if the current mouse position is 
	within the window passed as an argument.  If the mouse is 
	outside the current window, -1 is returned in the y and x 
	arguments otherwise the y and x coordinates of the mouse 
	(relative to the top left corner of the window) are returned in 
	y and x.

	getmouse() returns the current status of the trapped mouse 
	buttons as set by mouse_set() or mouse_on().

	getbmap() returns the current status of the button action used 
	to map a mouse action to the Soft Label Keys as set by the 
	map_button() function.

	Functions from ncurses: mouseinterval().

	mouseinterval() sets the timeout for a mouse click. On all
	current platforms, PDCurses receives mouse button press and
	release events, but must synthesize click events. It does this
	by checking whether a release event is queued up after a press
	event. If it gets a press event, and there are no more events
	waiting, it will wait for the timeout interval, then check again
	for a release. A press followed by a release is reported as
	BUTTON_CLICKED; otherwise it's passed through as BUTTON_PRESSED.
	The default timeout is 100ms; valid values are 0 (no clicks
	reported) through 1000ms. In x11, the timeout can also be set
	via the clickPeriod resource. The return value from
	mouseinterval() is the old timeout. To check the old value
	without setting a new one, call it with a parameter of -1.

	wenclose() reports whether the given screen-relative y, x 
	coordinates fall within the given window.

  Portability				     X/Open    BSD    SYS V
	mouse_set				-	-      4.0
	mouse_on				-	-      4.0
	mouse_off				-	-      4.0
	request_mouse_pos			-	-      4.0
	map_button				-	-      4.0
	wmouse_position				-	-      4.0
	getmouse				-	-      4.0
	getbmap					-	-      4.0
	mouseinterval				-	-	-
	wenclose				-	-	-

**man-end****************************************************************/

int mouse_set(unsigned long mbe)
{
	PDC_LOG(("mouse_set() - called: event %x\n", mbe));

	SP->_trap_mbe = mbe;
	return PDC_mouse_set();
}

int mouse_on(unsigned long mbe)
{
	PDC_LOG(("mouse_on() - called: event %x\n", mbe));

	SP->_trap_mbe |= mbe;
	return PDC_mouse_set();
}

int mouse_off(unsigned long mbe)
{
	PDC_LOG(("mouse_off() - called: event %x\n", mbe));

	SP->_trap_mbe &= ~mbe;
	return PDC_mouse_set();
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
	PDC_LOG(("request_mouse_pos() - called\n"));

	Mouse_status = pdc_mouse_status;

	return OK;
}

void wmouse_position(WINDOW *win, int *y, int *x)
{
	PDC_LOG(("wmouse_position() - called\n"));

	/* if the current mouse position is outside the provided window, 
	   put -1 in x and y */

	if (!win)
	{
		*y = *x = -1;
		return;
	}

	if (wenclose(win, MOUSE_Y_POS, MOUSE_X_POS))
	{
		*x = MOUSE_X_POS - win->_begx;
		*y = MOUSE_Y_POS - win->_begy;
	}
	else
	{
		*x = *y = -1;
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

/* ncurses mouse interface */

int mouseinterval(int wait)
{
	int old_wait;

	PDC_LOG(("mouseinterval() - called: %d\n", wait));

	if (SP)
	{
		old_wait = SP->mouse_wait;

		if (wait >= 0 && wait <= 1000)
			SP->mouse_wait = wait;
	}
	else
		old_wait = 100;

	return old_wait;
}

bool wenclose(const WINDOW *win, int y, int x)
{
	if (!win)
		return FALSE;

	return (y >= win->_begy &&
		x >= win->_begx &&
		y < win->_begy + win->_maxy &&
		x < win->_begx + win->_maxx);
}
