/**************************************************************************
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
**************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <stdlib.h>

#ifdef PDCDEBUG
const char *rcsid_PDCgetsc =
	"$Id: pdcgetsc.c,v 1.15 2006/02/06 02:54:27 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

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

**man-end**********************************************************************/

int PDC_get_cursor_pos(int *row, int *col)
{
	PDC_LOG(("PDC_get_cursor_pos() - called\n"));

	regs.h.ah = 0x03;
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);
	*row = regs.h.dh;
	*col = regs.h.dl;

	return OK;
}

/*man-start*********************************************************************

  PDC_get_cur_col()	- get current column position of cursor

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

**man-end**********************************************************************/

int PDC_get_cur_col(void)
{
	PDC_LOG(("PDC_get_cur_col() - called\n"));

#ifdef WATCOMC
	regs.w.ax = 0x0003;
#else
	regs.x.ax = 0x0003;
#endif
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);

	return (int)regs.h.dl;
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
	PDCurses  int PDC_get_cur_row(void);

**man-end**********************************************************************/

int PDC_get_cur_row(void)
{
	PDC_LOG(("PDC_get_cur_row() - called\n"));

#ifdef WATCOMC
	regs.w.ax = 0x0003;
#else
	regs.x.ax = 0x0003;
#endif
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);

	return (int)regs.h.dh;
}

/*man-start*********************************************************************

  PDC_get_attribute()	- Get attribute at current cursor

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

**man-end**********************************************************************/

int PDC_get_attribute(void)
{
	PDC_LOG(("PDC_get_attribute() - called\n"));

#ifdef WATCOMC
	regs.w.ax = 0x0800;
#else
	regs.x.ax = 0x0800;
#endif
	regs.h.bh = SP->video_page;
	int86(0x10, &regs, &regs);

	return (int)regs.h.ah;
}

/*man-start*********************************************************************

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

**man-end**********************************************************************/

int PDC_get_columns(void)
{
	int cols;
	char *env_cols;

	PDC_LOG(("PDC_get_columns() - called\n"));

 /* use the value from COLS environment variable, if set. MH 10-Jun-92 */
 /* and use the minimum of COLS and return from int10h    MH 18-Jun-92 */

	regs.h.ah = 0x0f;
	int86(0x10, &regs, &regs);
	cols = (int)regs.h.ah;
	env_cols = (char *)getenv("COLS");

	if (env_cols != (char *)NULL)
		cols = min(atoi(env_cols), cols);

	PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

	return cols;
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
	PDCurses  int PDC_get_cursor_mode(void);

**man-end**********************************************************************/

int PDC_get_cursor_mode(void)
{
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));

	return getdosmemword(0x460);
}

/*man-start*********************************************************************

  PDC_get_font()	- Get the current font size

  PDCurses Description:
	This is a private PDCurses routine.

	This function returns the current font size.

  Portability:
	PDCurses  int PDC_get_font(void);

**man-end**********************************************************************/

int PDC_get_font(void)
{
	int retval;

	PDC_LOG(("PDC_get_font() - called\n"));

	retval = getdosmemword(0x485);

	/* Assume the MDS Genius is in 66 line mode. */

	if ((retval == 0) && (SP->adapter == _MDS_GENIUS))
		retval = _FONT15;

	switch (SP->adapter)
	{
	case _MDA:
		retval = 10;	/* POINTS is not certain on MDA/Hercules */
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

	case _CGA:
		retval = _FONT8;
	}

	return retval;
}

/*man-start*********************************************************************

  PDC_get_rows()	- Return number of screen rows.

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

**man-end**********************************************************************/

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

	if ((rows == 1) && (SP->adapter == _MDS_GENIUS))
		rows = 66;
	if ((rows == 1) && (SP->adapter == _MDA))
		rows = 25;

	if (rows == 1)
	{
		rows = _default_lines;		/* Allow pre-setting LINES */
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
		break;

	default:
		rows = 25;
		break;
	}

	PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

	return rows;
}

/*man-start*********************************************************************

  PDC_get_scrn_mode()	- Return the current BIOS video mode

  PDCurses Description:
	This is a private PDCurses routine.


  PDCurses Return Value:
	Returns the current BIOS Video Mode Number.

  Portability:
	PDCurses  int PDC_get_scrn_mode(void);

**man-end**********************************************************************/

int PDC_get_scrn_mode(void)
{
	PDC_LOG(("PDC_get_scrn_mode() - called\n"));

	regs.h.ah = 0x0f;
	int86(0x10, &regs, &regs);

	return (int)regs.h.al;
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
	PDCurses  int PDC_query_adapter_type(void);

**man-end**********************************************************************/

int PDC_query_adapter_type(void)
{
	int equip;
	int retval = _NONE;

	/* thanks to paganini@ax.apc.org for the GO32 fix */

#if defined(__DJGPP__) && defined(NOW_WORKS)
# include <dpmi.h>
	_go32_dpmi_registers dpmi_regs;
#endif

#if !defined(__DJGPP__) && !defined(WATCOMC)
	struct SREGS segs;
#endif
	short video_base = getdosmemword(0x463);

	PDC_LOG(("PDC_query_adapter_type() - called\n"));

	/* attempt to call VGA Identify Adapter Function */

	regs.h.ah = 0x1a;
	regs.h.al = 0;
	int86(0x10, &regs, &regs);

	if ((regs.h.al == 0x1a) && (retval == _NONE))
	{
		/* We know that the PS/2 video BIOS is alive and well. */

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
		case 26:			/* ...alt. VGA BIOS... */
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
		}
	}
	else
	{
		/* No VGA BIOS, check for an EGA BIOS by selecting an
		   Alternate Function Service...

		   bx == 0x0010 --> return EGA information */

		regs.h.ah = 0x12;
# ifdef WATCOMC
		regs.w.bx = 0x10;
# else
		regs.x.bx = 0x10;
# endif
		int86(0x10, &regs, &regs);

		if ((regs.h.bl != 0x10) && (retval == _NONE))
		{
			/* An EGA BIOS exists */

			regs.h.ah = 0x12;
			regs.h.bl = 0x10;
			int86(0x10, &regs, &regs);

			if (regs.h.bh == 0)
				retval = _EGACOLOR;
			else
				retval = _EGAMONO;
		}
		else if (retval == _NONE)
		{
			/* Now we know we only have CGA or MDA */

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
		}
	}

	if ((retval == _NONE)
#ifndef CGA_DIRECT
	||  (retval == _CGA)
#endif
	)
		SP->direct_video = FALSE;

	if ((unsigned int) SP->video_seg == 0xb000)
		SP->mono = TRUE;
	else
		SP->mono = FALSE;

	/* Check for DESQview shadow buffer
	   thanks to paganini@ax.apc.org for the GO32 fix */

#if defined(__DJGPP__) && defined(NOW_WORKS)
	dpmi_regs.h.ah = 0xfe;
	dpmi_regs.h.al = 0;
	dpmi_regs.x.di = SP->video_ofs;
	dpmi_regs.x.es = SP->video_seg;
	_go32_dpmi_simulate_int(0x10, &dpmi_regs);
	SP->video_ofs = dpmi_regs.x.di;
	SP->video_seg = dpmi_regs.x.es;
#endif

#if !defined(__DJGPP__) && !defined(WATCOMC)
	regs.h.ah = 0xfe;
	regs.h.al = 0;
	regs.x.di = SP->video_ofs;
	segs.es   = SP->video_seg;
	int86x(0x10, &regs, &regs, &segs);
	SP->video_ofs = regs.x.di;
	SP->video_seg = segs.es;
#endif
	if (!SP->adapter)
		SP->adapter = retval;

	return PDC_sanity_check(retval);
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
	PDCurses  int PDC_sanity_check(int adapter);

**man-end**********************************************************************/

int PDC_sanity_check(int adapter)
{
	int fontsize = PDC_get_font();
	int rows = PDC_get_rows();

	PDC_LOG(("PDC_sanity_check() - called: Adapter %d\n", adapter));

	switch (adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
		switch (rows)
		{
		case 25:
		case 43:	
			break;
		default:
			SP->bogus_adapter = TRUE;
		}

		switch (fontsize)
		{
		case _FONT8:
		case _FONT14:
			break;
		default:
			SP->bogus_adapter = TRUE;
		}
		break;

	case _VGACOLOR:
	case _VGAMONO:
		break;

	case _CGA:
	case _MDA:
	case _MCGACOLOR:
	case _MCGAMONO:
		switch (rows)
		{
		case 25:
			break;
		default:
			SP->bogus_adapter = TRUE;
		}
		break;

	default:
		SP->bogus_adapter = TRUE;
	}

	if (SP->bogus_adapter)
	{
		SP->sizeable = FALSE;
		SP->direct_video = FALSE;
	}

	return adapter;
}
