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

#ifdef UNIX
#include <defs.h>
#include <term.h>
#endif

#ifdef PDCDEBUG
char *rcsid_PDCgetsc  = "$Id: pdcgetsc.c,v 1.1 2001/01/10 08:31:07 mark Exp $";
#endif

#if defined( OS2 ) && !defined( EMXVIDEO )
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

#ifdef	FLEXOS
	retcode = s_get(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	if (retcode < 0L)
		return( ERR );
	*row = vir.vc_cursor.pos_row;
	*col = vir.vc_cursor.pos_col;
	return( OK );
#endif

#ifdef	DOS
	regs.h.ah = 0x03;
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);
	*row = regs.h.dh;
	*col = regs.h.dl;
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	v_getxy (col, row);
# else
	VioGetCurPos((PUSHORT)row,(PUSHORT)col,0);
# endif
	return( OK );
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
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
#ifdef	OS2
# ifdef EMXVIDEO
	int curCol=0, curRow=0;
# else
	USHORT curCol=0, curRow=0;
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_col() - called\n");
#endif

#ifdef	FLEXOS
	retcode = s_get(T_VIRCON, 1L, (char *) &vir, (long) sizeof(vir));
	return( (retcode < 0L) ? ERR : vir.vc_cursor.pos_col );
#endif

#ifdef	DOS
# ifdef WATCOMC
	regs.w.ax = 0x0003;
# else
	regs.x.ax = 0x0003;
# endif
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);
	return((int) regs.h.dl);
#endif

#ifdef	OS2
	/* find the current cursor position */
# ifdef EMXVIDEO
	v_getxy (&curCol, &curRow);
# else
	VioGetCurPos ((PUSHORT) &curRow, (PUSHORT) &curCol, 0);
# endif
	return (curCol);
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
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
#ifdef	OS2
# ifdef  EMXVIDEO
	int curCol=0, curRow=0;
# else
	USHORT curCol=0, curRow=0;
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_row() - called\n");
#endif

#ifdef	FLEXOS
	retcode = s_get(T_VIRCON, 1L, (char *) &vir, (long) sizeof(vir));
	return( (retcode < 0L) ? ERR : vir.vc_cursor.pos_col );
#endif

#ifdef	DOS
# ifdef WATCOMC
	regs.w.ax = 0x0003;
# else
	regs.x.ax = 0x0003;
# endif
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);
	return ((int) regs.h.dh);
#endif

#ifdef	OS2
	/* find the current cursor position */
# ifdef EMXVIDEO
	v_getxy (&curCol, &curRow);
# else
	VioGetCurPos ((PUSHORT) &curRow, (PUSHORT) &curCol, 0);
# endif
	return (curRow);
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
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
#ifdef	OS2
# ifndef EMXVIDEO
	USHORT cellLen = 2;
# endif
	int curRow=0, curCol=0;
	char Cell[4];
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_attribute() - called\n");
#endif

#ifdef	FLEXOS
	/* Get and return current attribute.  Force error until fixed. */
	return ((COLOR_CYAN) >> ((sizeof(ch type) / 2) * 8));
#endif

#ifdef	DOS
# ifdef WATCOMC
	regs.w.ax = 0x0800;
# else
	regs.x.ax = 0x0800;
# endif
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);
	return ((int) regs.h.ah);
#endif

#ifdef	OS2
	PDC_get_cursor_pos(&curRow, &curCol);
# ifdef EMXVIDEO
	v_getline (Cell, curCol, curRow, 1);
# else
	VioReadCellStr((PCH)&Cell, (PUSHORT)&cellLen, (USHORT)curRow, (USHORT)curCol, 0);
# endif
	return ((int) Cell[1]);
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
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
#ifdef	DOS
	int	cols=0;
	char *env_cols=NULL;
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	int rows=0;
# else
	VIOMODEINFO modeInfo={0};
# endif
	int cols=0;
	char *env_cols=NULL;
#endif


#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - called\n");
#endif

#ifdef	FLEXOS
	return( vir.vc_size.rs_ncols );
#endif

#ifdef	DOS
/* use the value from COLS environment variable, if set. MH 10-Jun-92 */
/* and use the minimum of COLS and return from int10h    MH 18-Jun-92 */
	regs.h.ah = 0x0f;
	int86(0x10, &regs, &regs);
	cols = (int)regs.h.ah;
	env_cols = (char *)getenv("COLS");
	if (env_cols != (char *)NULL)
	{
		cols = min(atoi(env_cols),cols);
	}
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - returned: cols %d\n",cols);
#endif
	return(cols);
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	v_dimen (&cols, &rows);
# else
	modeInfo.cb = sizeof(modeInfo);
	VioGetMode(&modeInfo, 0);
	cols = modeInfo.col;
# endif
	env_cols = (char *)getenv("COLS");
	if (env_cols != (char *)NULL)
	{
		cols = min(atoi(env_cols),cols);
	}
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - returned: cols %d\n",cols);
#endif
	return(cols);
#endif

#ifdef UNIX
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - returned: cols %d\n",columns);
#endif
	return(columns);
#endif

#if defined(XCURSES)
	return(XCurses_get_cols());
#endif
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
#ifdef	DOS
	short	cmode=0;
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	int curstart=0, curend=0;
# else
	VIOCURSORINFO cursorInfo;
# endif
	short	cmode=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cursor_mode() - called\n");
#endif

#ifdef	FLEXOS
	/*
	 * Under FLEXOS, this routine returns 1 if the cursor is on and 0 if
	 * the cursor is off...
	 */
	s_getfield(T_VIRCON, VC_MODE, STDOUT, (far BYTE *) & vir,
		sizeof(vir.vc_mode));

	if (vir.vc_mode & VCWM_CURSOR)
		return (TRUE);
	else
		return (FALSE);
#endif

#ifdef	DOS
	cmode = getdosmemword (0x460);
	return (cmode);
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	v_getctype (&curstart, &curend);
	cmode = ((curstart << 8) | (curend));
# else
	VioGetCurType (&cursorInfo, 0);
/* I am not sure about this JGB */
	cmode = ((cursorInfo.yStart << 8) | (cursorInfo.cEnd));
# endif
	return(cmode);
#endif

#ifdef UNIX
	return(0);/* this is N/A */
#endif

#if defined (XCURSES)
	return(0);/* this is N/A */
#endif
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
#if	defined (DOS) && defined (FAST_VIDEO)
	int	retval=0;
#endif

#ifdef OS2
# ifdef EMXVIDEO
	int	retval=0;
# else
	VIOMODEINFO modeInfo={0};
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_font() - called\n");
#endif

#if	defined (DOS) && defined (FAST_VIDEO)
	retval = getdosmemword (0x485);
	if ((retval == 0) && (SP->adapter == _MDS_GENIUS))
	{
		retval = _FONT15; /* Assume the MDS Genius is in 66 line mode. */
	}
	switch (SP->adapter)
	{
	case _MDA:
			retval = 10; /* POINTS is not certain on MDA/Hercules */
			break;
	case _EGACOLOR:
	case _EGAMONO:
		switch (retval)
		{
		case _FONT8:
		case _FONT14:
			break;
		default:
			retval = _FONT14;
		}
		break;

	case _VGACOLOR:
	case _VGAMONO:
		switch (retval)
		{
		case _FONT8:
		case _FONT14:
		case _FONT16:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return( retval );
#endif

#ifdef OS2
# ifdef EMXVIDEO
	retval = v_hardware();
	return (retval == V_MONOCHROME) ? 14 : (retval == V_COLOR_8) ? 8 : 12;
# else
	modeInfo.cb = sizeof(modeInfo);
			/* set most parameters of modeInfo */
	VioGetMode(&modeInfo, 0);
	return ( modeInfo.vres / modeInfo.row);
# endif
#endif

#ifdef UNIX
	return(0); /* this is N/A */
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
#ifdef	DOS
	char *env_rows=NULL;
	int	rows=0;
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	int	cols=0;
# else
	VIOMODEINFO modeInfo={0};
# endif
	int	rows=0;
	char *env_rows=NULL;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - called\n");
#endif

#ifdef	FLEXOS
		return (vir.vc_size.rs_nrows);
#endif

#ifdef	DOS
/* use the value from LINES environment variable, if set. MH 10-Jun-92 */
/* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */
	rows = getdosmembyte(0x484) + 1;
	env_rows = (char *)getenv("LINES");
	if (env_rows != (char *)NULL)
		rows = min(atoi(env_rows),rows);

	if ((rows == 1) && (SP->adapter == _MDS_GENIUS))
		rows = 66;
	if ((rows == 1) && (SP->adapter == _MDA))
		rows = 25;  /* new test MH 10-Jun-92 */
	if (rows == 1)
	{
		rows = _default_lines;	/* Allow pre-setting LINES	 */
		SP->direct_video = FALSE;
	}
	switch (SP->adapter)
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
/* lets be reasonably flexible with VGAs - they could be Super VGAs */
/* capable of displaying any number of lines. MH 10-Jun-92          */
/*
		switch (rows)
		{
		case 25:
		case 28:
		case 50:
			break;
		default:
			rows = 25;
		}
*/
		break;

	default:
		rows = 25;
		break;
	}
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - returned: rows %d\n",rows);
#endif
	return (rows);
#endif

#ifdef	OS2
/* use the value from LINES environment variable, if set. MH 10-Jun-92 */
/* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */
# ifdef EMXVIDEO
	v_dimen (&cols, &rows);
# else
	modeInfo.cb = sizeof(modeInfo);
	VioGetMode(&modeInfo, 0);
	rows = modeInfo.row;
# endif
	env_rows = (char *)getenv("LINES");
	if (env_rows != (char *)NULL)
		rows = min(atoi(env_rows),rows);
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - returned: rows %d\n",rows);
#endif
	return(rows);
#endif

#ifdef UNIX
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - returned: rows %d\n",lines);
#endif
	return(lines);
#endif

#if defined(XCURSES)
	return(XCurses_get_rows());
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

#ifdef	FLEXOS
	return( ERR );
#endif

#ifdef	DOS
	regs.h.ah = 0x0f;
	int86(0x10, &regs, &regs);
	return ((int) regs.h.al);
#endif

#ifdef     OS2
# ifdef EMXVIDEO
	return(ERR);
# else
	VioGetMode (&modeInfo, 0);
	return(OK);
# endif
#endif

#ifdef UNIX
	return(ERR); /* this is N/A */
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

#ifdef	DOS
		/* thanks to paganini@ax.apc.org for the GO32 fix */
#  if defined(GO32) && defined(NOW_WORKS)
#    include <dpmi.h>
	_go32_dpmi_registers dpmi_regs;
#  endif
	int	equip;
	struct SREGS segs;
	short video_base = getdosmemword (0x463);
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_query_adapter_type() - called\n");
#endif

#ifdef	FLEXOS
	return (_FLEXOS);
#endif

#ifdef	DOS
	/*
	 * Attempt to call VGA Identify Adapter Function...
	 */
	regs.h.ah = 0x1a;
	regs.h.al = 0;
	int86(0x10, &regs, &regs);
	if ((regs.h.al == 0x1a) && (retval == _NONE))
	{
		/*
		 * We know that the PS/2 video BIOS is alive and well.
		 */
		switch (regs.h.al)
		{
		case 0:
			retval = _NONE;
			break;
		case 1:
			retval = _MDA;
			break;
		case 2:
			retval = _CGA;
			break;
		case 4:
			retval = _EGACOLOR;
			SP->sizeable = TRUE;
			break;
		case 5:
			retval = _EGAMONO;
			break;
		case 26:
			retval = _VGACOLOR;	/* ...alt. VGA BIOS... */
		case 7:
			retval = _VGACOLOR;
			SP->sizeable = TRUE;
			break;
		case 8:
			retval = _VGAMONO;
			break;
		case 10:
		case 13:
			retval = _MCGACOLOR;
			break;
		case 12:
			retval = _MCGAMONO;
			break;
		default:
			retval = _CGA;
			break;
		}
	}
	else
	{
		/*
		 * No VGA BIOS, check for an EGA BIOS by selecting an
		 * Alternate Function Service...
		 *
		 * bx == 0x0010	 -->  return EGA information
		 */
		regs.h.ah = 0x12;
# ifdef WATCOMC
		regs.w.bx = 0x10;
# else
		regs.x.bx = 0x10;
# endif
		int86(0x10, &regs, &regs);
		if ((regs.h.bl != 0x10) && (retval == _NONE))
		{
			/*
			 * An EGA BIOS exists...
			 */
			regs.h.ah = 0x12;
			regs.h.bl = 0x10;
			int86(0x10, &regs, &regs);
			if (regs.h.bh == 0)
				retval = _EGACOLOR;
			else
				retval = _EGAMONO;
		}
		else
		if (retval == _NONE)
		{
			/*
			 * Now we know we only have CGA or MDA...
			 */
			int86(0x11, &regs, &regs);
			equip = (regs.h.al & 0x30) >> 4;
			switch (equip)
			{
			case 1:
			case 2:
				retval = _CGA;
				break;
			case 3:
				retval = _MDA;
				break;
			default:
				retval = _NONE;
				break;
			}
		}
	}
	if (video_base == 0x3d4)
	{
		SP->video_seg = 0xb800;
		switch (retval)
		{
		case _EGAMONO:
			retval = _EGACOLOR;
			break;
		case _VGAMONO:
			retval = _VGACOLOR;
			break;
		default:
			break;
		}
	}
	if (video_base == 0x3b4)
	{
		SP->video_seg = 0xb000;
		switch (retval)
		{
		case _EGACOLOR:
			retval = _EGAMONO;
			break;
		case _VGACOLOR:
			retval = _VGAMONO;
			break;
		default:
			break;
		}
	}
	if ((retval == _NONE)
#ifndef CGA_DIRECT
	||  (retval == _CGA)
#endif
	)
	{
		SP->direct_video = FALSE;
	}
	if ((unsigned int) SP->video_seg == 0xb000)
		SP->mono = TRUE;
	else
		SP->mono = FALSE;

		/* Check for DESQview shadow buffer */
		/* thanks to paganini@ax.apc.org for the GO32 fix */
#if defined(GO32) && defined(NOW_WORKS)
	dpmi_regs.h.ah = 0xfe;
	dpmi_regs.h.al = 0;
	dpmi_regs.x.di = SP->video_ofs;
	dpmi_regs.x.es = SP->video_seg;
	_go32_dpmi_simulate_int(0x10, &dpmi_regs);
	SP->video_ofs = dpmi_regs.x.di;
	SP->video_seg = dpmi_regs.x.es;
#endif

#if !defined(GO32) && !defined(WATCOMC)
	regs.h.ah = 0xfe;
	regs.h.al = 0;
	regs.x.di = SP->video_ofs;
	segs.es   = SP->video_seg;
	int86x(0x10, &regs, &regs, &segs);
	SP->video_ofs = regs.x.di;
	SP->video_seg = segs.es;
#endif

	if  (!SP->adapter)
		SP->adapter = retval;
	return (PDC_sanity_check(retval));
#endif

#ifdef	OS2
# ifdef EMXVIDEO
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
# else
	VioGetConfig(0, &configInfo, 0);
	SP->sizeable = TRUE;
	return(OK);

#  if     0
	switch (configInfo.adapter)
	{
		case 0:
			retval = _MDA;
			SP->mono =TRUE;
			break;
		case 1:
			retval = _CGA;
			SP->mono = FALSE;
			break;
		case 2: switch (configInfo.display)
			{
				case 0:
				case 3:
					retval = _EGAMONO;
					SP->mono = TRUE;
					break;
				case 1:
				case 2:
				case 4:
				case 9:
					retval = _EGACOLOR;
					SP->mono = FALSE;
					break;
			}
			break;
		case 3: switch (configInfo.display)
			{
				case 0:
				case 3:
					retval = _VGAMONO;
					SP->sizeable = TRUE;
					SP->mono = TRUE;
					break;
				case 1:
				case 2:
				case 4:
				case 9:
					retval = _VGACOLOR;
					SP->sizeable = TRUE;
					SP->mono = FALSE;
					break;
			}
			break;
		default:
			retval = _CGA;
			SP->mono = FALSE;
			break;
	}
	return (PDC_sanity_check(retval));
#  endif
# endif
#endif

#ifdef UNIX
	if (set_foreground != NULL && set_background != NULL) /* we have a colour monitor */
		{
		SP->mono = FALSE;
		retval = _UNIX_COLOR;
		}
	else
		{
		SP->mono = TRUE;
		retval = _UNIX_MONO;
		}
	SP->sizeable = FALSE;
	SP->bogus_adapter = FALSE;
return(retval);
#endif
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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_sanity_check(int adapter)
#else
int	PDC_sanity_check(adapter)
int adapter;
#endif
/***********************************************************************/
{
	int	fontsize = PDC_get_font();
	int	rows	 = PDC_get_rows();

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_sanity_check() - called: Adapter %d\n",adapter);
#endif

	switch (adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
		switch (rows)
		{
		case 25:	break;
		case 43:	break;
		default:
			SP->bogus_adapter = TRUE;
			break;
		}

		switch (fontsize)
		{
		case _FONT8:	break;
		case _FONT14:	break;
		default:
			SP->bogus_adapter = TRUE;
			break;
		}
		break;

	case _VGACOLOR:
	case _VGAMONO:

/*                                                                  */
/* lets be reasonably flexible with VGAs - they could be Super VGAs */
/* capable of displaying any number of lines. MH 10-Jun-92          */
/* This also applies to font size.            MH 16-Jun-92          */
/*
		switch (rows)
		{
		case 25:	break;
		case 43:	break;
		case 50:	break;
		default:
			SP->bogus_adapter = TRUE;
			break;
		}

		switch (fontsize)
		{
		case _FONT8:	break;
		case _FONT14:	break;
		case _FONT16:	break;
		default:
			SP->bogus_adapter = TRUE;
			break;
		}
*/
		break;

	case _CGA:
	case _MDA:
	case _MCGACOLOR:
	case _MCGAMONO:
		switch (rows)
		{
		case 25:	break;
		default:
			SP->bogus_adapter = TRUE;
			break;
		}
		break;

	default:
		SP->bogus_adapter = TRUE;
		break;
	}
	if (SP->bogus_adapter)
	{
		SP->sizeable	= FALSE;
		SP->direct_video	= FALSE;
	}
	return (adapter);
}
