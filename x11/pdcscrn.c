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

#ifdef PDCDEBUG
char *rcsid_PDCscrn  = "$Id: pdcscrn.c,v 1.2 2001/01/10 08:30:13 mark Exp $";
#endif

bool GLOBAL_sb_on=FALSE;
bool GLOBAL_slk_on=FALSE;

/*man-start*********************************************************************

  PDC_scr_close()	- Internal low-level binding to close the physical screen

  PDCurses Description:
 	This function provides a low-level binding for the Flexos
 	platform which must close the screen before writing to it.
 	This is a nop for the DOS platform.

 	This function is provided in order to reset the FlexOS 16 bit
 	character set for input rather than the limited input
 	character set associated with the VT52.

  PDCurses Return Value:
 	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
 	The DOS platform will never fail.  The Flexos platform may fail
 	depending on the ability to close the current virtual console in
 	8 (as opposed to 16) bit mode.

  Portability:
 	PDCurses	int	PDC_scr_close( void );

**man-end**********************************************************************/

int	PDC_scr_close(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scr_close() - called\n");
#endif

	return( OK );
}

/*man-start*********************************************************************

  PDC_scrn_modes_equal()	- Decide if two screen modes are equal

  PDCurses Description:
 	Mainly required for OS/2. It decides if two screen modes
        (VIOMODEINFO structure) are equal. Under DOS it just compares
        two integers

  PDCurses Return Value:
 	This function returns TRUE if equal else FALSe.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_scrn_modes_equal( int mode1, int mode2 );
 	OS2 PDCurses	int PDC_scrn_modes_equal( VIOMODEINFO mode1, VIOMODEINFO mode2 );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_scrn_modes_equal(int mode1, int mode2)
#else
bool	PDC_scrn_modes_equal(mode1,mode2)
int mode1;
int mode2;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scrn_modes_equal() - called\n");
#endif
	return (mode1 == mode2);
}

/*man-start*********************************************************************

  PDC_scr_open()	- Internal low-level binding to open the physical screen

  PDCurses Description:
 	This function provides a low-level binding for the Flexos
 	platform which must open the screen before writing to it.

 	This function is provided in order to access the FlexOS 16 bit
 	character set for input rather than the limited input
 	character set associated with the VT52.

  PDCurses Return Value:
 	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
 	The DOS platform will never fail.  The Flexos platform may fail
 	depending on the ability to open the current virtual console in
 	8 (as opposed to 16) bit mode.

  Portability:
 	PDCurses	int	PDC_scr_open( SCREEN* internal, bool echo );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_scr_open(SCREEN *internal, bool echo)
#else
int	PDC_scr_open(internal,echo)
SCREEN *internal;
bool echo;
#endif
/***********************************************************************/
{
extern bool sb_started;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scr_open() - called\n");
#endif

	internal->cursrow = internal->curscol = 0;
	internal->direct_video	= FALSE;		/* Assume that we can't	      */
	internal->autocr	= TRUE;		/* lf -> crlf by default      */
	internal->raw_out	= FALSE;	/* tty I/O modes	      */
	internal->raw_inp	= FALSE;	/* tty I/O modes	      */
	internal->cbreak	= TRUE;
	internal->save_key_modifiers	= FALSE;
	internal->return_key_modifiers	= FALSE;
	internal->echo		= echo;
/* under System V Curses, typeahead checking is enabled by default */
	internal->refrbrk	= TRUE;	/* allow premature end of refresh*/
	internal->video_page	= 0;		/* Current Video Page	      */
	internal->visible_cursor= TRUE;		/* Assume that it is visible  */
	internal->cursor	= 0;
	internal->adapter	= 0;
	internal->audible	= TRUE;
	internal->visibility	= 1;
	internal->orig_cursor = internal->cursor;
	internal->orgcbr = 0;
	internal->blank = ' ';
	internal->resized = FALSE;
	internal->shell = FALSE;
	internal->_trap_mbe = 0L;
	internal->_map_mbe_to_key = 0L;
	internal->linesrippedoff = 0;
	internal->linesrippedoffontop = 0;
	internal->delaytenths = 0;
	internal->sb_on = sb_started;
	internal->sb_total_y = 0;
	internal->sb_viewport_y = 0;
	internal->sb_cur_y = 0;
	internal->sb_total_x = 0;
	internal->sb_viewport_x = 0;
	internal->sb_cur_x = 0;
	internal->line_color = COLOR_WHITE;

	return( OK );
}

/*man-start*********************************************************************

  PDC_resize_screen()	- Internal low-level function to resize screen

  PDCurses Description:
 	This function provides a means for the application program to
 	resize the overall dimensions of the screen.  Under DOS and OS/2
 	the application can tell PDCurses what size to make the screen;
 	under X11, resizing is done by the user and this function simply
 	adjusts its internal structures to fit the new size.

  PDCurses Return Value:
 	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:

  Portability:
 	PDCurses	int	PDC_resize_screen( int, int );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_resize_screen(int nlines, int ncols)
#else
int	PDC_resize_screen(nlines, ncols)
int nlines,ncols;
#endif
/***********************************************************************/
{
	int rc=OK;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_resize_screen() - called. Lines: %d Cols: %d\n",nlines,ncols);
#endif

	rc = XCursesResizeScreen(nlines,ncols);
	return (rc);
}
