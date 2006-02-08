/**************************************************************************
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
**************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef clearok
#undef idlok
#undef idcok
#undef immedok
#undef leaveok
#undef setscrreg
#undef wsetscrreg
#undef scrollok
#undef raw_output

#ifdef PDCDEBUG
const char *rcsid_outopts =
	"$Id: outopts.c,v 1.16 2006/02/08 17:40:33 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                       outopts

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

  X/Open Description:
	With the clearok() routine, if bf is TRUE, the next call to 
	wrefresh() with this window will clear the screen completely 
	and redraw the entire screen.

	If idlok() is enabled (bf is TRUE), curses will use the 
	insert/delete line feature of terminals so equipped.  If 
	disabled, curses will not use this feature.  (The insert/delete 
	character feature is always used.)  This option should be 
	enabled only if the application needs insert/delete line; for 
	example, for a screen editor.  It is disabled by default because 
	insert/delete line tends to be visually annoying when used in 
	applications where it isn't really needed.  If insert/delete 
	line cannot be used, curses will redraw the changed portions of 
	all lines.

	With the idcok() routine, if enabled (bf is TRUE), curses 
	considers using the hardware insert/delete character feature of 
	terminals so equipped. This is enabled by default.

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

  PDCurses Description:
	idlok() and idcok() are provided for portability.  

	Though not explicitly stated, the cursor will be made visible
	again, if leaveok() is passed a FALSE value.
 			(Frotz:911226 Comments?)

	The raw_output() routine enables the output of raw characters 
	using the 'standard' *add* and *ins* curses functions. To force 
	8 bit output and no translation of control characters, invoke 
	raw_output() with bf as TRUE.  To force 7 bit output and 
	translation of control characters, invoke raw_output() with bf 
	as FALSE.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	clearok					Y	Y	Y
	idlok					Y	Y	Y
	idcok					-	-      4.0
	immedok					-	-      4.0
	leaveok					Y	Y	Y
	setscrreg				Y	Y	Y
	wsetscrreg				Y	Y	Y
	scrollok				Y	Y	Y
	raw_output				-	-	-

**man-end**********************************************************************/

int clearok(WINDOW *win, bool bf)
{
	PDC_LOG(("clearok() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_clear = bf;

	return OK;
}

int idlok(WINDOW *win, bool bf)
{
	PDC_LOG(("idlok() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_use_idl = bf;

	return OK;
}

void idcok(WINDOW *win, bool bf)
{
	PDC_LOG(("idcok() - called\n"));

	if (win != (WINDOW *)NULL)
		win->_use_idc = bf;
}

void immedok(WINDOW *win, bool bf)
{
	PDC_LOG(("immedok() - called\n"));

	if (win != (WINDOW *)NULL)
		win->_immed = bf;
}

int leaveok(WINDOW *win, bool bf)
{
	PDC_LOG(("leaveok() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_leaveit = bf;

	if (bf)
		PDC_cursor_off();
	else
		PDC_cursor_on();

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

	if (win == (WINDOW *)NULL)
		return ERR;

	if ((0 <= top) && (top <= win->_cury) &&
	    (win->_cury <= bottom) && (bottom < win->_maxy))
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

	if (win == (WINDOW *)NULL)
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
