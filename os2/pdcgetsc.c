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

#define CURSES_LIBRARY 1
#include <curses.h>
#include <stdlib.h>

#ifdef PDCDEBUG
const char *rcsid_PDCgetsc =
	"$Id: pdcgetsc.c,v 1.18 2006/02/23 01:46:52 wmcbrine Exp $";
#endif

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

  PDC_get_cur_col()  - get current column position of cursor

  PDCurses Description:
	This is a private PDCurses function

	This routine returns the current column position of the cursor 
	on screen.

  PDCurses Return Value:
	This routine returns the current column position of the cursor. 
	No error is returned.

  PDCurses Errors:
	There are no defined errors for this routine.

  Portability:
	PDCurses  int PDC_get_cur_col(void);

**man-end****************************************************************/

int PDC_get_cur_col(void)
{
#ifdef EMXVIDEO
	int curCol = 0, curRow = 0;
#else
	USHORT curCol = 0, curRow = 0;
#endif
	PDC_LOG(("PDC_get_cur_col() - called\n"));

	/* find the current cursor position */

#ifdef EMXVIDEO
	v_getxy(&curCol, &curRow);
#else
	VioGetCurPos((PUSHORT)&curRow, (PUSHORT)&curCol, 0);
#endif
	return curCol;
}

/*man-start**************************************************************

  PDC_get_cur_row()  - get current row position of cursor

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
	PDCurses  int PDC_get_cur_row(void);

**man-end****************************************************************/

int PDC_get_cur_row(void)
{
#ifdef  EMXVIDEO
	int curCol = 0, curRow = 0;
#else
	USHORT curCol = 0, curRow = 0;
#endif
	PDC_LOG(("PDC_get_cur_row() - called\n"));

	/* find the current cursor position */

#ifdef EMXVIDEO
	v_getxy(&curCol, &curRow);
#else
	VioGetCurPos((PUSHORT)&curRow, (PUSHORT)&curCol, 0);
#endif
	return curRow;
}

/*man-start**************************************************************

  PDC_get_attribute()   - Get attribute at current cursor

  PDCurses Description:
	This is a private PDCurses function

	Return the current attr at current cursor position on the 
	screen.

  PDCurses Return Value:
	This routine will return OK upon success and otherwise ERR will 
	be returned.

  PDCurses Errors:
	There are no defined errors for this routine.

  Portability:
	PDCurses  int PDC_get_attribute(void);

**man-end****************************************************************/

int PDC_get_attribute(void)
{
#ifndef EMXVIDEO
	USHORT cellLen = 2;
#endif
	int curRow = 0, curCol = 0;
	char Cell[4];

	PDC_LOG(("PDC_get_attribute() - called\n"));

	PDC_get_cursor_pos(&curRow, &curCol);

#ifdef EMXVIDEO
	v_getline(Cell, curCol, curRow, 1);
#else
	VioReadCellStr((PCH)&Cell, (PUSHORT)&cellLen, (USHORT)curRow, 
		(USHORT)curCol, 0);
#endif
	return (int)Cell[1];
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
	Gets the cursor type to begin in scan line startrow and end in
	scan line endrow.  Both values should be in the range 0 through 
	31.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

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

	/* I am not sure about this JGB */

	return (cursorInfo.yStart << 8) | cursorInfo.cEnd;
#endif
}

/*man-start**************************************************************

  PDC_get_font()  - Get the current font size

  PDCurses Description:
	This is a private PDCurses routine.

	This function returns the current font size.

  Portability:
	PDCurses  int PDC_get_font(void);

**man-end****************************************************************/

int PDC_get_font(void)
{
#ifdef EMXVIDEO
	int retval;
#else
	VIOMODEINFO modeInfo = {0};
#endif
	PDC_LOG(("PDC_get_font() - called\n"));

#ifdef EMXVIDEO
	retval = v_hardware();
	return (retval == V_MONOCHROME) ? 14 : (retval == V_COLOR_8) ? 8 : 12;
#else
	modeInfo.cb = sizeof(modeInfo);

	/* set most parameters of modeInfo */

	VioGetMode(&modeInfo, 0);
	return (modeInfo.vres / modeInfo.row);
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

  PDCurses Errors:
	No errors are defined for this function.

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

/*man-start**************************************************************

  PDC_get_scrn_mode()   - Return the current BIOS video mode

  PDCurses Description:
	This is a private PDCurses routine.


  PDCurses Return Value:
	Returns the current BIOS Video Mode Number.

  PDCurses Errors:
	The EMXVIDEO version of this routine returns an ERR.

  Portability:
	PDCurses  int PDC_get_scrn_mode(void);

**man-end****************************************************************/

#if defined(EMXVIDEO)
int PDC_get_scrn_mode(void)
#else
int PDC_get_scrn_mode(VIOMODEINFO *modeinfo)
#endif
{
	PDC_LOG(("PDC_get_scrn_mode() - called\n"));

#ifdef EMXVIDEO
	return ERR;
#else
	VioGetMode(modeinfo, 0);
	return OK;
#endif
}

/*man-start**************************************************************

  PDC_query_adapter_type() - Determine PC video adapter type

  PDCurses Description:
	This is a private PDCurses routine.

	Thanks to Jeff Duntemann, K16RA for providing the impetus
	(through the Dr. Dobbs Journal, March 1989 issue) for getting
	the routines below merged into Bjorn Larsson's PDCurses 1.3...
	-- frotz@dri.com  900730

  PDCurses Return Value:
	This function returns a macro identifier indicating the adapter
	type.  See the list of adapter types in CURSPRIV.H.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_query_adapter_type(void);

**man-end****************************************************************/

#if defined(EMXVIDEO)
int PDC_query_adapter_type(void)
#else
int PDC_query_adapter_type(VIOCONFIGINFO *configinfo)
#endif
{
	PDC_LOG(("PDC_query_adapter_type() - called\n"));
#ifdef EMXVIDEO
	return (v_hardware() == V_MONOCHROME) ? _UNIX_MONO : _UNIX_COLOR;
#else
	VioGetConfig(0, configinfo, 0);
	return OK;
#endif
}
