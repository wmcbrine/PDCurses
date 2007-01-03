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

#include "pdcwin.h"

RCSID("$Id: pdcgetsc.c,v 1.32 2007/01/03 14:10:47 wmcbrine Exp $");

/* get the cursor size/shape */

int PDC_get_cursor_mode(void)
{
	CONSOLE_CURSOR_INFO ci;
    
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));

	GetConsoleCursorInfo(pdc_con_out, &ci);

	return ci.dwSize;
}

/* return number of screen rows */

int PDC_get_rows(void)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_rows() - called\n"));

	GetConsoleScreenBufferInfo(pdc_con_out, &scr);

	return scr.srWindow.Bottom - scr.srWindow.Top + 1;
}

/* return number of buffer rows */

int PDC_get_buffer_rows(void)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_buffer_rows() - called\n"));

	GetConsoleScreenBufferInfo(pdc_con_out, &scr);

	return scr.dwSize.Y;
}

/* return width of screen/viewport */

int PDC_get_columns(void)
{
	CONSOLE_SCREEN_BUFFER_INFO scr;

	PDC_LOG(("PDC_get_columns() - called\n"));

	GetConsoleScreenBufferInfo(pdc_con_out, &scr);

	return scr.srWindow.Right - scr.srWindow.Left + 1;
}
