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
char *rcsid_PDCgetsc  = "$Id: pdcgetsc.c,v 1.1 2001/01/10 08:29:32 mark Exp $";
#endif

#if !defined( EMXVIDEO )
	VIOCONFIGINFO configInfo={0};
	VIOMODEINFO modeInfo={0};
#endif

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_cursor_pos(int *row, int *col)
#else
int	PDC_get_cursor_pos(row,col)
int *row;
int *col;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cursor_pos() - called\n");
#endif

#ifdef EMXVIDEO
	v_getxy (col, row);
#else
	VioGetCurPos((PUSHORT)row,(PUSHORT)col,0);
#endif
	return( OK );
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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_cur_col(void)
#else
int	PDC_get_cur_col()
#endif
/***********************************************************************/
{
#ifdef EMXVIDEO
	int curCol=0, curRow=0;
#else
	USHORT curCol=0, curRow=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_col() - called\n");
#endif

	/* find the current cursor position */
#ifdef EMXVIDEO
	v_getxy (&curCol, &curRow);
#else
	VioGetCurPos ((PUSHORT) &curRow, (PUSHORT) &curCol, 0);
#endif
	return (curCol);

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_cur_row(void)
#else
int	PDC_get_cur_row()
#endif
/***********************************************************************/
{
#ifdef  EMXVIDEO
	int curCol=0, curRow=0;
#else
	USHORT curCol=0, curRow=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_row() - called\n");
#endif

	/* find the current cursor position */
#ifdef EMXVIDEO
	v_getxy (&curCol, &curRow);
#else
	VioGetCurPos ((PUSHORT) &curRow, (PUSHORT) &curCol, 0);
#endif
	return (curRow);

}

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_attribute(void)
#else
int	PDC_get_attribute()
#endif
/***********************************************************************/
{
#ifndef EMXVIDEO
	USHORT cellLen = 2;
#endif
	int curRow=0, curCol=0;
	char Cell[4];

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_attribute() - called\n");
#endif

	PDC_get_cursor_pos(&curRow, &curCol);
#ifdef EMXVIDEO
	v_getline (Cell, curCol, curRow, 1);
#else
	VioReadCellStr((PCH)&Cell, (PUSHORT)&cellLen, (USHORT)curRow, (USHORT)curCol, 0);
#endif
	return ((int) Cell[1]);

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_columns(void)
#else
int	PDC_get_columns()
#endif
/***********************************************************************/
{
#ifdef EMXVIDEO
	int rows=0;
#else
	VIOMODEINFO modeInfo={0};
#endif
	int cols=0;
	char *env_cols=NULL;


#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - called\n");
#endif

#ifdef EMXVIDEO
	v_dimen (&cols, &rows);
#else
	modeInfo.cb = sizeof(modeInfo);
	VioGetMode(&modeInfo, 0);
	cols = modeInfo.col;
#endif

	env_cols = (char *)getenv("COLS");
	if (env_cols != (char *)NULL)
	{
		cols = min(atoi(env_cols),cols);
	}
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - returned: cols %d\n",cols);
#endif
	return(cols);

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_cursor_mode(void)
#else
int	PDC_get_cursor_mode()
#endif
/***********************************************************************/
{
#ifdef EMXVIDEO
	int curstart=0, curend=0;
#else
	VIOCURSORINFO cursorInfo;
#endif
	short	cmode=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cursor_mode() - called\n");
#endif

#ifdef EMXVIDEO
	v_getctype (&curstart, &curend);
	cmode = ((curstart << 8) | (curend));
#else
	VioGetCurType (&cursorInfo, 0);
/* I am not sure about this JGB */
	cmode = ((cursorInfo.yStart << 8) | (cursorInfo.cEnd));
#endif
	return(cmode);

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_font(void)
#else
int	PDC_get_font()
#endif
/***********************************************************************/
{
#ifdef EMXVIDEO
	int	retval=0;
#else
	VIOMODEINFO modeInfo={0};
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_font() - called\n");
#endif

#ifdef EMXVIDEO
	retval = v_hardware();
	return (retval == V_MONOCHROME) ? 14 : (retval == V_COLOR_8) ? 8 : 12;
#else
	modeInfo.cb = sizeof(modeInfo);
			/* set most parameters of modeInfo */
	VioGetMode(&modeInfo, 0);
	return ( modeInfo.vres / modeInfo.row);
#endif
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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_rows(void)
#else
int	PDC_get_rows()
#endif
/***********************************************************************/
{

#ifdef EMXVIDEO
	int	cols=0;
#else
	VIOMODEINFO modeInfo={0};
#endif
	int	rows=0;
	char *env_rows=NULL;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - called\n");
#endif

/* use the value from LINES environment variable, if set. MH 10-Jun-92 */
/* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */
#ifdef EMXVIDEO
	v_dimen (&cols, &rows);
#else
	modeInfo.cb = sizeof(modeInfo);
	VioGetMode(&modeInfo, 0);
	rows = modeInfo.row;
#endif
	env_rows = (char *)getenv("LINES");
	if (env_rows != (char *)NULL)
		rows = min(atoi(env_rows),rows);
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - returned: rows %d\n",rows);
#endif
	return(rows);

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_scrn_mode(void)
#else
int	PDC_get_scrn_mode()
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_scrn_mode() - called\n");
#endif

#ifdef EMXVIDEO
	return(ERR);
#else
	VioGetMode (&modeInfo, 0);
	return(OK);
#endif

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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_query_adapter_type(void)
#else
int	PDC_query_adapter_type()
#endif
/***********************************************************************/
{
	int	retval = _NONE;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_query_adapter_type() - called\n");
#endif

#ifdef EMXVIDEO
	SP->adapter = v_hardware();
	if (SP->adapter == V_MONOCHROME)
	{
		SP->mono = TRUE;
		retval = _UNIX_MONO;
	}
	else
	{
		SP->mono = FALSE;
		retval = _UNIX_COLOR;
	}
	SP->sizeable = TRUE;
	SP->bogus_adapter = FALSE;
	return(retval);
#else
	VioGetConfig(0, &configInfo, 0);
	SP->sizeable = TRUE;
	return(OK);
#endif

}
