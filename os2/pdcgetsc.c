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

#include "pdcos2.h"

RCSID("$Id: pdcgetsc.c,v 1.30 2006/10/13 23:29:27 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_get_cursor_pos()  - return current cursor position

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
	PDC_LOG(("PDC_get_cursor_pos() - called\n"));

#ifdef EMXVIDEO
	v_getxy(col, row);
#else
	VioGetCurPos((PUSHORT)row, (PUSHORT)col, 0);
#endif
	return OK;
}

/*man-start**************************************************************

  PDC_get_columns()  - return width of screen/viewport.

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
#ifdef EMXVIDEO
	int rows = 0;
#else
	VIOMODEINFO modeInfo = {0};
#endif
	int cols = 0;
	char *env_cols = NULL;

	PDC_LOG(("PDC_get_columns() - called\n"));

#ifdef EMXVIDEO
	v_dimen(&cols, &rows);
#else
	modeInfo.cb = sizeof(modeInfo);
	VioGetMode(&modeInfo, 0);
	cols = modeInfo.col;
#endif
	env_cols = (char *)getenv("COLS");

	if (env_cols != (char *)NULL)
		cols = min(atoi(env_cols), cols);

	PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

	return cols;
}

/*man-start**************************************************************

  PDC_get_cursor_mode() - Get the cursor start and stop scan lines.

  PDCurses Description:
	Gets the cursor size.

  Portability:
	PDCurses  int PDC_get_cursor_mode(void);

**man-end****************************************************************/

int PDC_get_cursor_mode(void)
{
#ifdef EMXVIDEO
	int curstart = 0, curend = 0;
#else
	VIOCURSORINFO cursorInfo;
#endif
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));

#ifdef EMXVIDEO
	v_getctype(&curstart, &curend);
	return (curstart << 8) | curend;
#else
	VioGetCurType (&cursorInfo, 0);

	return (cursorInfo.yStart << 8) | cursorInfo.cEnd;
#endif
}

/*man-start**************************************************************

  PDC_get_rows()  - Return number of screen rows.

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
#ifdef EMXVIDEO
	int cols = 0;
#else
	VIOMODEINFO modeInfo = {0};
#endif
	int rows = 0;
	char *env_rows = NULL;

	PDC_LOG(("PDC_get_rows() - called\n"));

 /* use the value from LINES environment variable, if set. MH 10-Jun-92 */
 /* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */

#ifdef EMXVIDEO
	v_dimen(&cols, &rows);
#else
	modeInfo.cb = sizeof(modeInfo);
	VioGetMode(&modeInfo, 0);
	rows = modeInfo.row;
#endif
	env_rows = (char *)getenv("LINES");

	if (env_rows != (char *)NULL)
		rows = min(atoi(env_rows), rows);

	PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

	return rows;
}
