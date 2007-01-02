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

#include "pdcx11.h"

RCSID("$Id: pdcgetsc.c,v 1.21 2007/01/02 14:19:43 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_get_columns()	- return width of screen/viewport.

  PDCurses Description:
	This is a private PDCurses function

	This function will return the width of the current screen.

**man-end****************************************************************/

int PDC_get_columns(void)
{
	PDC_LOG(("PDC_get_columns() - called\n"));

	return XCursesCOLS;
}

int PDC_get_cursor_mode(void)
{
	return 0;
}

/*man-start**************************************************************

  PDC_get_rows()	- Return number of screen rows.

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the maximum number of rows supported by the display.
	e.g.  25, 28, 43, 50, 60, 66...

**man-end****************************************************************/

int PDC_get_rows(void)
{
	PDC_LOG(("PDC_get_rows() - called\n"));

	return XCursesLINES;
}
