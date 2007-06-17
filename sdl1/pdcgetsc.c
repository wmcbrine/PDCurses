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

#include "pdcsdl.h"

RCSID("$Id: pdcgetsc.c,v 1.6 2007/06/17 19:30:42 wmcbrine Exp $")

/* get the cursor size/shape */

int PDC_get_cursor_mode(void)
{
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));

	return 0;
}

/* return number of screen rows */

int PDC_get_rows(void)
{
	PDC_LOG(("PDC_get_rows() - called\n"));

	return pdc_sheight / pdc_fheight;
}

/* return width of screen/viewport */

int PDC_get_columns(void)
{
	PDC_LOG(("PDC_get_columns() - called\n"));

	return pdc_swidth / pdc_fwidth;
}
