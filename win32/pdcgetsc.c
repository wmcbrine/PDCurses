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
#define	INCLUDE_WINDOWS_H
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCgetsc  = "$Id: pdcgetsc.c,v 1.1 2001/01/10 08:30:48 mark Exp $";
#endif

extern HANDLE hConOut, hConIn;
extern CONSOLE_SCREEN_BUFFER_INFO scr;

/*man-start*********************************************************************

  PDC_get_cursor_pos()	- return current cursor position

  PDCurses Description:
 	This is a private PDCurses function

 	Gets the cursor position in video page 0.  'row' and 'column'
 	are the cursor address.  At this time, there is no support for
 	use of multiple screen pages.

  PDCurses Return Value:
 	This routine will return OK upon success and otherwise ERR will be
 	returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_cursor_pos( int* row, int* col );

**man-end**********************************************************************/

int	PDC_get_cursor_pos(int *row, int *col)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cursor_pos() - called\n");
#endif

   GetConsoleScreenBufferInfo(hConOut, &scr);
   *col = scr.dwCursorPosition.X;
   *row = scr.dwCursorPosition.Y;
   return(OK);
}

/*man-start*********************************************************************

  PDC_get_cur_col()	- get current column position of cursor

  PDCurses Description:
 	This is a private PDCurses function

 	This routine returns the current column position of the cursor on
 	screen.

  PDCurses Return Value:
 	This routine returns the current column position of the cursor. No
 	error is returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_cur_col( void );

**man-end**********************************************************************/

int	PDC_get_cur_col(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_col() - called\n");
#endif

   GetConsoleScreenBufferInfo(hConOut, &scr);
   return (scr.dwCursorPosition.X);
}

/*man-start*********************************************************************

  PDC_get_cur_row()	- get current row position of cursor

  PDCurses Description:
 	This is a private PDCurses function

 	This routine returns the current row position of the cursor on
 	screen.

  PDCurses Return Value:
 	This routine returns the current row position of the cursor. No
 	error is returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_cur_row( void );

**man-end**********************************************************************/

int	PDC_get_cur_row(void)
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_row() - called\n");
#endif

   GetConsoleScreenBufferInfo(hConOut, &scr);
   return (scr.dwCursorPosition.Y);
}

// Stopped Here...

/*man-start*********************************************************************

  PDC_get_attribute()	- Get attribute at current cursor

  PDCurses Description:
 	This is a private PDCurses function

 	Return the current attr at current cursor position on the screen.

  PDCurses Return Value:
 	This routine will return OK upon success and otherwise ERR will be
 	returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_attribute( void );

**man-end**********************************************************************/

int	PDC_get_attribute(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_attribute() - called\n");
#endif
/* doesnt do anything !! */
	return(0);
}

/*man-start*********************************************************************

  PDC_get_cursor_mode()	- Get the cursor start and stop scan lines.

  PDCurses Description:
 	Gets the cursor type to begin in scan line startrow and end in
 	scan line endrow.  Both values should be 0-31.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_get_cursor_mode( void );

**man-end**********************************************************************/

int	PDC_get_cursor_mode(void)
{
 CONSOLE_CURSOR_INFO ci;
    
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cursor_mode() - called\n");
#endif

 GetConsoleCursorInfo(hConOut, &ci);
/* size is between 1 and 100, so convert it */
 return ((ci.dwSize * 32 / 100) - 1);
}

/*man-start*********************************************************************

  PDC_get_font()	- Get the current font size

  PDCurses Description:
 	This is a private PDCurses routine.

 	This function returns the current font size.  This function only
 	works if the #define FAST_VIDEO is true.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	An ERR will be returned if FAST_VIDEO is not true.

  Portability:
 	PDCurses	int PDC_get_font( void );

**man-end**********************************************************************/

int	PDC_get_font(void)
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_font() - called\n");
#endif

	return(0); /* this is N/A */
}

/*man-start*********************************************************************

  PDC_get_rows()	- Return number of screen rows.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Returns the maximum number of rows supported by the display.
 	e.g.  25, 28, 43, 50, 60, 66...

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_get_rows( void );

**man-end**********************************************************************/

int	PDC_get_rows(void)
{

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_get_rows() - called\n");
#endif

   GetConsoleScreenBufferInfo(hConOut, &scr);
#if FGC0
   return ( scr.dwSize.Y ); /* Allow the whole screen to be accessed */
#else
   return (scr.srWindow.Bottom - scr.srWindow.Top + 1);
#endif
}

/*man-start*********************************************************************

  PDC_get_buffer_rows()	- Return number of screen buffer rows.

  PDCurses Description:
 	This is a private PDCurses routine.

  PDCurses Return Value:
 	Returns the maximum number of rows in the screen buffer.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_get_buffer_rows( void );

**man-end**********************************************************************/

int	PDC_get_buffer_rows(void)
{

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_get_buffer_rows() - called\n");
#endif

   GetConsoleScreenBufferInfo( hConOut, &scr );
   return ( scr.dwSize.Y );
}

/*man-start*********************************************************************

  PDC_get_columns()	- return width of screen/viewport.

  PDCurses Description:
 	This is a private PDCurses function

 	This function will return the width of the current screen.

  PDCurses Return Value:
 	This routine will return OK upon success and otherwise ERR will be
 	returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_columns( void );

**man-end**********************************************************************/

int	PDC_get_columns(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - called\n");
#endif

   GetConsoleScreenBufferInfo(hConOut, &scr);
#if FGC0
   return ( scr.dwSize.X ); /* Allow the whole screen to be accessed */
#else
   return (scr.srWindow.Right - scr.srWindow.Left + 1);
#endif
}

/*man-start*********************************************************************

  PDC_get_scrn_mode()	- Return the current BIOS video mode

  PDCurses Description:
 	This is a private PDCurses routine.


  PDCurses Return Value:
 	Returns the current BIOS Video Mode Number.

  PDCurses Errors:
 	The FLEXOS version of this routine returns an ERR.
 	The UNIX version of this routine returns an ERR.
 	The EMXVIDEO version of this routine returns an ERR.

  Portability:
 	PDCurses	int PDC_get_scrn_mode( void );

**man-end**********************************************************************/

int	PDC_get_scrn_mode(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_scrn_mode() - called\n");
#endif
	return(OK);
}

/*man-start*********************************************************************

  PDC_query_adapter_type()	- Determine PC video adapter type

  PDCurses Description:
 	This is a private PDCurses routine.

 	Thanks to Jeff Duntemann, K16RA for providing the impetus
 	(through the Dr. Dobbs Journal, March 1989 issue) for getting
 	the routines below merged into Bjorn Larsson's PDCurses 1.3...
 		-- frotz@dri.com	900730

  PDCurses Return Value:
 	This function returns a macro identifier indicating the adapter
 	type.  See the list of adapter types in CURSPRIV.H.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_query_adapter_type( void );

**man-end**********************************************************************/

int	PDC_query_adapter_type(void)
{
	int	retval = _NONE;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_query_adapter_type() - called\n");
#endif

	SP->mono = FALSE;
	retval = _VGACOLOR;

	return (retval);
}

/*man-start*********************************************************************

  PDC_sanity_check() - A video adapter identification sanity check

  PDCurses Description:
 	This is a private PDCurses routine.

 	This routine will force sane values for various control flags.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_sanity_check( int adapter );

**man-end**********************************************************************/

int	PDC_sanity_check(int adapter)
{
#ifdef PDCDEBUG
   if (trace_on) 
      PDC_debug("PDC_sanity_check() - called: Adapter %d\n",adapter);
#endif

   return (adapter);
}
