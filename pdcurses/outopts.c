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

RCSID("$Id: outopts.c,v 1.33 2007/03/16 06:33:44 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								outopts

  Synopsis:
	int clearok(WINDOW *win, bool bf);
	int idlok(WINDOW *win, bool bf);
	void idcok(WINDOW *win, bool bf);
	void immedok(WINDOW *win, bool bf);
	int leaveok(WINDOW *win, bool bf);
	int setscrreg(int top, int bot);
	int wsetscrreg(WINDOW *win, int top, int bot);
	int scrollok(WINDOW *win, bool bf);

	int raw_output(bool bf);

  Description:
	With clearok(), if bf is TRUE, the next call to wrefresh() with 
	this window will clear the screen completely and redraw the 
	entire screen.

	The immedok() routine, called with a second argument of TRUE, 
	causes an automatic wrefrsh() to be called on the window every 
	time a change is made to that window, due to calls like; 
	waddch(), wclrtoeol(), etc... Not surprisingly, this causes a 
	severe performance overhead.

	Normally, the hardware cursor is left at the location of the
	window being refreshed.  leaveok() allows the cursor to be
	left wherever the update happens to leave it.  It is useful
	for applications where the cursor is not used, since it reduces
	the need for cursor motions.  If possible, the cursor is made
	invisible when this option is enabled.

	The setscrreg() and wsetscrreg() functions allow the user to set 
	a software scrolling region in a window.  The parameters 'top' 
	and 'bot' are the line numbers of the top and bottom margin of 
	the scrolling region.  (Line 0 is the top line of the window.)  
	If this option and scrollok() are enabled, an attempt to move 
	off the bottom margin will cause all lines in the scrolling 
	region to scroll up one line.  Only the text of the window is 
	scrolled.

	idlok() and idcok() do nothing in PDCurses, but are provided for 
	compatibility with other curses implementations.

	raw_output() enables the output of raw characters using the 
	'standard' *add* and *ins* curses functions (that is, it 
	disables translation of control characters).

  Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
	clearok					Y	Y	Y
	idlok					Y	Y	Y
	idcok					Y	-      4.0
	immedok					Y	-      4.0
	leaveok					Y	Y	Y
	setscrreg				Y	Y	Y
	wsetscrreg				Y	Y	Y
	scrollok				Y	Y	Y
	raw_output				-	-	-

**man-end****************************************************************/

int clearok(WINDOW *win, bool bf)
{
	PDC_LOG(("clearok() - called\n"));

	if (!win)
		return ERR;

	win->_clear = bf;

	return OK;
}

int idlok(WINDOW *win, bool bf)
{
	PDC_LOG(("idlok() - called\n"));

	return OK;
}

void idcok(WINDOW *win, bool bf)
{
	PDC_LOG(("idcok() - called\n"));
}

void immedok(WINDOW *win, bool bf)
{
	PDC_LOG(("immedok() - called\n"));

	if (win)
		win->_immed = bf;
}

int leaveok(WINDOW *win, bool bf)
{
	PDC_LOG(("leaveok() - called\n"));

	if (!win)
		return ERR;

	win->_leaveit = bf;

	curs_set(!bf);

	return OK;
}

int setscrreg(int top, int bottom)
{
	PDC_LOG(("setscrreg() - called: top %d bottom %d\n", top, bottom));

	return wsetscrreg(stdscr, top, bottom);
}

int wsetscrreg(WINDOW *win, int top, int bottom)
{
	PDC_LOG(("wsetscrreg() - called: top %d bottom %d\n", top, bottom));

	if (win && 0 <= top && top <= win->_cury &&
	    win->_cury <= bottom && bottom < win->_maxy)
	{
		win->_tmarg = top;
		win->_bmarg = bottom;

		return OK;
	}
	else
		return ERR;
}

int scrollok(WINDOW *win, bool bf)
{
	PDC_LOG(("scrollok() - called\n"));

	if (!win)
		return ERR;

	win->_scroll = bf;

	return OK;
}

int raw_output(bool bf)
{
	PDC_LOG(("raw_output() - called\n"));

	SP->raw_out = bf;

	return OK;
}
