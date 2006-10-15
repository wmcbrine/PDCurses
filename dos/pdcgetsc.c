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

#include "pdcdos.h"

#include <stdlib.h>

RCSID("$Id: pdcgetsc.c,v 1.34 2006/10/15 02:42:25 wmcbrine Exp $");

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
	PDCREGS regs;

	PDC_LOG(("PDC_get_cursor_pos() - called\n"));

	regs.h.ah = 0x03;
	regs.h.bh = 0;
	PDCINT(0x10, regs);
	*row = regs.h.dh;
	*col = regs.h.dl;

	return OK;
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
	PDCREGS regs;
	int cols;
	const char *env_cols;

	PDC_LOG(("PDC_get_columns() - called\n"));

 /* use the value from COLS environment variable, if set. MH 10-Jun-92 */
 /* and use the minimum of COLS and return from int10h    MH 18-Jun-92 */

	regs.h.ah = 0x0f;
	PDCINT(0x10, regs);
	cols = (int)regs.h.ah;

	env_cols = getenv("COLS");

	if (env_cols)
		cols = min(atoi(env_cols), cols);

	PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

	return cols;
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
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));

	return getdosmemword(0x460);
}

/*man-start**************************************************************

  PDC_get_rows()	- Return number of screen rows.

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the maximum number of rows supported by the display.
	e.g. 25, 28, 43, 50, 60, 66...

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_get_rows(void);

**man-end****************************************************************/

int PDC_get_rows(void)
{
	char *env_rows;
	int rows;

	PDC_LOG(("PDC_get_rows() - called\n"));

 /* use the value from LINES environment variable, if set. MH 10-Jun-92 */
 /* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */

	rows = getdosmembyte(0x484) + 1;
	env_rows = (char *)getenv("LINES");

	if (env_rows != (char *)NULL)
		rows = min(atoi(env_rows), rows);

	if ((rows == 1) && (pdc_adapter == _MDS_GENIUS))
		rows = 66;
	if ((rows == 1) && (pdc_adapter == _MDA))
		rows = 25;

	if (rows == 1)
	{
		rows = 25;
		pdc_direct_video = FALSE;
	}

	switch (pdc_adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
		switch (rows)
		{
		case 25:
		case 43:
			break;
		default:
			rows = 25;
		}
		break;

	case _VGACOLOR:
	case _VGAMONO:
		break;

	default:
		rows = 25;
		break;
	}

	PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

	return rows;
}
