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

#ifdef PDC_WIDE
# define UNICODE
#endif

#include <windows.h>
#undef MOUSE_MOVED
#define	CURSES_LIBRARY 1
#include <curses.h>

RCSID("$Id: pdcgetsc.c,v 1.25 2006/08/11 07:04:31 wmcbrine Exp $");

extern HANDLE hConOut, hConIn;

/*man-start**************************************************************

  PDC_get_cursor_pos()	- return current cursor position

  PDCurses Description:
	This is a private PDCurses function

	Gets the cursor position in video page 0.  'row' and 'column'
	are the cursor address.  At this time, there is no support for
	use of multiple screen pages.

  PDCurses Return Value:
	This routine will return OK upon success and otherwise ERR will
	be returned.

  PDCurses Errors:
	There are no defined errors for this routine.

  Portability:
	PDCurses  int PDC_get_cursor_pos(int *row, int *col);

**man-end****************************************************************/

int PDC_get_cursor_pos(int *row, int *col)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_cursor_pos() - called\n"));

	GetConsoleScreenBufferInfo(hConOut, &scr);

	*col = scr.dwCursorPosition.X;
	*row = scr.dwCursorPosition.Y;

	return OK;
}

/*man-start**************************************************************

  PDC_get_cursor_mode()	- Get the cursor start and stop scan lines.

  PDCurses Description:
	Gets the cursor size.

  Portability:
	PDCurses  int PDC_get_cursor_mode(void);

**man-end****************************************************************/

int PDC_get_cursor_mode(void)
{
	CONSOLE_CURSOR_INFO ci;
    
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));

	GetConsoleCursorInfo(hConOut, &ci);

	return ci.dwSize;
}

/*man-start**************************************************************

  PDC_get_font()	- Get the current font size

  PDCurses Description:
	This is a private PDCurses routine.

	This function returns the current font size.

  Portability:
	PDCurses  int PDC_get_font(void);

**man-end****************************************************************/

int PDC_get_font(void)
{
	PDC_LOG(("PDC_get_font() - called\n"));

	return 0;
}

/*man-start**************************************************************

  PDC_get_rows()	- Return number of screen rows.

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the maximum number of rows supported by the display.
	e.g.  25, 28, 43, 50, 60, 66...

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_get_rows(void);

**man-end****************************************************************/

int PDC_get_rows(void)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_rows() - called\n"));

	GetConsoleScreenBufferInfo(hConOut, &scr);

	return scr.srWindow.Bottom - scr.srWindow.Top + 1;
}

/*man-start**************************************************************

  PDC_get_buffer_rows()	- Return number of screen buffer rows.

  PDCurses Description:
	This is a private PDCurses routine.

  PDCurses Return Value:
	Returns the maximum number of rows in the screen buffer.

  Portability:
	PDCurses  int PDC_get_buffer_rows(void);

**man-end****************************************************************/

int PDC_get_buffer_rows(void)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_buffer_rows() - called\n"));

	GetConsoleScreenBufferInfo(hConOut, &scr);

	return scr.dwSize.Y;
}

/*man-start**************************************************************

  PDC_get_columns()	- return width of screen/viewport.

  PDCurses Description:
	This is a private PDCurses function

	This function will return the width of the current screen.

  PDCurses Return Value:
	This routine will return OK upon success and otherwise ERR will
	be returned.

  PDCurses Errors:
	There are no defined errors for this routine.

  Portability:
	PDCurses  int PDC_get_columns(void);

**man-end****************************************************************/

int PDC_get_columns(void)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_columns() - called\n"));

	GetConsoleScreenBufferInfo(hConOut, &scr);

	return scr.srWindow.Right - scr.srWindow.Left + 1;
}
