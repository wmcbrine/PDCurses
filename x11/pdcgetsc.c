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

#include "pdcx11.h"

RCSID("$Id: pdcgetsc.c,v 1.17 2006/07/15 15:38:24 wmcbrine Exp $");

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
	PDC_LOG(("PDC_get_columns() - called\n"));

	return XCursesCOLS;
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
	PDC_LOG(("PDC_get_rows() - called\n"));

	return XCursesLINES;
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

	return OK;
}
