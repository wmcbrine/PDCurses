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
char *rcsid_PDCsetsc  = "$Id: pdcsetsc.c,v 1.1 2001/01/10 08:31:13 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_set_80x25()	- force a known screen state: 80x25 text mode.

  PDCurses Description:
 	This is a private PDCurses function.

 	Forces the appropriate 80x25 alpha mode given the display adapter.

 	Since we currently do not support changing the virtual console size,
 	this routine is a NOP under Flexos.

  PDCurses Return Value:
 	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
 	No errors are defined for this routine.

  Portability:
 	PDCurses	int	PDC_set_80x25( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_80x25(void)
#else
int	PDC_set_80x25()
#endif
/***********************************************************************/
{
#if defined( OS2 ) && !defined( EMXVIDEO )
	VIOMODEINFO modeInfo={0};
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_80x25() - called\n");
#endif

#ifdef	FLEXOS
	return( OK );
#endif

#ifdef	DOS
	switch (SP->adapter)
	{
	case _CGA:
	case _EGACOLOR:
	case _EGAMONO:
	case _VGACOLOR:
	case _VGAMONO:
	case _MCGACOLOR:
	case _MCGAMONO:
		regs.h.ah = 0x00;
		regs.h.al = 0x03;
		int86(0x10, &regs, &regs);
		break;
	case _MDA:
		regs.h.ah = 0x00;
		regs.h.al = 0x07;
		int86(0x10, &regs, &regs);
	default:
		break;
	}
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	return( OK );
# else
	modeInfo.cb = sizeof(modeInfo);
	/* set most parameters of modeInfo */
	VioGetMode(&modeInfo, 0);
	modeInfo.fbType = 1;
	VioSetMode(&modeInfo, 0);
# endif
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
}

/*man-start*********************************************************************

  PDC_set_cursor_mode()	- Set the cursor start and stop scan lines.

  PDCurses Description:
 	Sets the cursor type to begin in scan line startrow and end in
 	scan line endrow.  Both values should be 0-31.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_set_cursor_mode( int startrow, int endrow );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_cursor_mode( int startrow, int endrow )
#else
int	PDC_set_cursor_mode(startrow,endrow)
int startrow;
int endrow;
#endif
/***********************************************************************/
{
#ifdef	OS2
# ifndef EMXVIDEO
	VIOCURSORINFO cursorInfo={0};
# endif
#endif

#ifdef	FLEXOS
	unsigned short mybuff = 0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_cursor_mode() - called: startrow %d endrow %d\n",startrow,endrow);
#endif

#ifdef	FLEXOS
	/*
	 * Under FLEXOS, this routine translates the input parameters in the
	 * following way:
	 *
	 * startrow --> visible_cursor endrow	 -->	cursor type:
	 * underline = 0; block = 1;
	 *
	 * VCWM_CURSOR	   0x0100	   bit - 8 Cursor off VCWM_BLOCK	
	 * 0x0200	   bit - 9 Block Cursor	
	 *
	 */
	retcode = s_getfield(T_VIRCON, VC_MODE, 1L, (void far *) &mybuff, 2L);
	if (retcode < 0L)
		return( ERR );
	if (startrow)
		mybuff &= ~VCWM_CURSOR;
	else
		mybuff |= VCWM_CURSOR;

	if (endrow)
		mybuff |= VCWM_BLOCK;
	else
		mybuff &= ~VCWM_BLOCK;

	retcode = s_setfield(T_VIRCON, VC_MODE, 1L, (void far *) &mybuff, 2L);
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
	regs.h.ah = 0x01;
	regs.h.ch = (unsigned char) startrow;
	regs.h.cl = (unsigned char) endrow;
	int86(0x10, &regs, &regs);
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	if (endrow <= startrow)
		v_hidecursor();
	else
		v_ctype (startrow, endrow);
	return( OK );
# else
	cursorInfo.yStart = startrow;
	cursorInfo.cEnd = endrow;
	cursorInfo.cx = 1;
	cursorInfo.attr = 0;
	return (VioSetCurType (&cursorInfo, 0) == 0);
# endif
#endif

#ifdef UNIX
	return(0); /* this is N/A */
#endif
}

/*man-start*********************************************************************

  PDC_set_font()	- sets the current font size

  PDCurses Description:
 	This is a private PDCurses function.

 	This routine sets the current font size, if the adapter allows
 	such a change.

  PDCurses Return Value:
 	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
 	It is an error to attempt to change the font size on a "bogus"
 	adapter.  The reason for this is that we have a known video
 	adapter identity problem.  e.g. Two adapters report the same
 	identifying characteristics.

 	It is also an error to attempt to change the size of the Flexos
 	console (as there is currently no support for that).

  Portability:
 	PDCurses	int	PDC_set_font( int size );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_font(int size)
#else
int	PDC_set_font(size)
int size;
#endif
/***********************************************************************/
{
#ifdef	OS2
# ifndef EMXVIDEO
	VIOMODEINFO modeInfo={0};
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_font() - called\n");
#endif

#ifdef	FLEXOS
	return( ERR );
#endif

#ifdef	DOS
	if (SP->bogus_adapter)
		return( ERR );

	switch (SP->adapter)
	{
	case _CGA:
	case _MDA:
	case _MCGACOLOR:
	case _MCGAMONO:
	case _MDS_GENIUS:
		break;

	case _EGACOLOR:
	case _EGAMONO:
		if (SP->sizeable && (SP->font != size))
		{
			switch (size)
			{
			case _FONT8:
				regs.h.ah = 0x11;
				regs.h.al = 0x12;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			case _FONT14:
				regs.h.ah = 0x11;
				regs.h.al = 0x11;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			default:
				break;
			}
		}
		break;

	case _VGACOLOR:
	case _VGAMONO:
		if (SP->sizeable && (SP->font != size))
		{
			switch (size)
			{
			case _FONT8:
				regs.h.ah = 0x11;
				regs.h.al = 0x12;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			case _FONT14:
				regs.h.ah = 0x11;
				regs.h.al = 0x11;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			case _FONT16:
				regs.h.ah = 0x11;
				regs.h.al = 0x14;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	if (SP->visible_cursor)
		PDC_cursor_on();
	else
		PDC_cursor_off();
	SP->font = PDC_get_font();
	return( OK );
#endif

#ifdef	OS2
# ifndef EMXVIDEO
	if (SP->sizeable && (SP->font != size))
	{
	       modeInfo.cb = sizeof(modeInfo);
	       /* set most parameters of modeInfo */
	       VioGetMode(&modeInfo, 0);
	       modeInfo.cb = 8;     /* ignore horiz an vert resolution */
               modeInfo.row = modeInfo.vres / size;
	       VioSetMode(&modeInfo, 0);
	}
	if (SP->visible_cursor)
		PDC_cursor_on();
	else
		PDC_cursor_off();
	SP->font = PDC_get_font();
# endif
	return( OK );
#endif

#ifdef UNIX
	return(OK); /* this is N/A */
#endif
}

/*man-start*********************************************************************

  PDC_set_rows()	- sets the physical number of rows on screen

  PDCurses Description:
 	This is a private PDCurses function.

 	This routine attempts to set the number of rows on the physical
 	screen to the passed value.

  PDCurses Return Value:
 	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
 	It is an error to attempt to change the screen size on a "bogus"
 	adapter.  The reason for this is that we have a known video
 	adapter identity problem.  e.g. Two adapters report the same
 	identifying characteristics.

 	It is also an error to attempt to change the size of the Flexos
 	console (as there is currently no support for that).

  Portability:
 	PDCurses	int	PDC_set_rows( int rows );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_rows(int rows)
#else
int	PDC_set_rows(rows)
int rows;
#endif
/***********************************************************************/
{
#ifdef	OS2
# ifndef EMXVIDEO
	VIOMODEINFO modeInfo={0};
	USHORT result=0;
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_rows() - called\n");
#endif

#ifdef	FLEXOS
	return( ERR );
#endif

#ifdef	DOS
	if (SP->bogus_adapter)
		return( ERR );

	switch (SP->adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
		if (rows < 43)
			PDC_set_font(_FONT14);
		else
			PDC_set_font(_FONT8);
		break;

	case _VGACOLOR:
	case _VGAMONO:
		if (rows < 28)
			PDC_set_font(_FONT16);
		else
		if (rows < 50)
			PDC_set_font(_FONT14);
		else
			PDC_set_font(_FONT8);
		break;

	case _MCGACOLOR:
	case _MCGAMONO:
	case _MDA:
	case _CGA:
	case _MDS_GENIUS:
	default:
		break;
	}
	SP->font = PDC_get_font();
	LINES = PDC_get_rows();
	COLS = PDC_get_columns();
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	return (ERR);
# else
	modeInfo.cb = sizeof(modeInfo);
	/* set most parameters of modeInfo */
	VioGetMode(&modeInfo, 0);
	modeInfo.fbType = 1;
	modeInfo.row = rows;
	result = VioSetMode(&modeInfo, 0);
	SP->font = PDC_get_font();
	LINES = PDC_get_rows();
	COLS = PDC_get_columns();
	return ((result == 0) ? OK : ERR);
# endif
#endif

#ifdef UNIX
	return(0); /* this is N/A */
#endif
}

/*man-start*********************************************************************

  PDC_set_scrn_mode()	- Set BIOS Video Mode

  PDCurses Description:
 	Sets the BIOS Video Mode Number ONLY if it is different from
 	the current video mode.  This routine is for DOS systems only.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_set_scrn_mode( int new_mode );

**man-end**********************************************************************/

/***********************************************************************/
#if defined( OS2 ) && !defined( EMXVIDEO )
int	PDC_set_scrn_mode(VIOMODEINFO new_mode)
#else
#  ifdef HAVE_PROTO
int	PDC_set_scrn_mode(int new_mode)
#  else
int	PDC_set_scrn_mode(new_mode)
int new_mode;
#  endif
#endif
/***********************************************************************/
{

#ifdef	DOS
	int	cur=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_scrn_mode() - called\n");
#endif

#ifdef	FLEXOS
	return( OK );
#endif

#ifdef	DOS
	cur = (int) PDC_get_scrn_mode();
	if (cur != new_mode)
	{
		regs.h.ah = 0x00;
		regs.h.al = (char) new_mode;
		int86(0x10, &regs, &regs);
	}
	SP->font = PDC_get_font();
	SP->scrnmode = new_mode;
	LINES = PDC_get_rows();
	COLS = PDC_get_columns();
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	return ( OK );
# else
	if (VioSetMode (&new_mode, 0) != 0)
		{
		SP->font = PDC_get_font();
		memcpy(SP->scrnmode,new_mode,sizeof(VIOMODEINFO));
		LINES = PDC_get_rows();
		COLS = PDC_get_columns();
		return( OK );
		}
	else
		return (ERR);
# endif
#endif

#ifdef UNIX
	return(OK); /* this is N/A */
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_curs_set(int visibility)
#else
int	PDC_curs_set(visibility)
int visibility;
#endif
/***********************************************************************/
{
#ifdef OS2
# ifndef EMXVIDEO
 VIOCURSORINFO pvioCursorInfo;
# endif
#endif
#if defined(DOS) || defined(OS2)
 int start,end,hidden=0;
#endif
 int ret_vis;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_curs_set() - called: visibility=%d\n",visibility);
#endif

	ret_vis = SP->visibility;
	SP->visibility = visibility;

#ifdef UNIX
	switch(visibility)
	{
		case 0:  /* invisible */
			if (cursor_invisible != NULL)
				putp(cursor_invisible);
			break;
		case 2:  /* highly visible */
			if (cursor_visible != NULL)
				putp(cursor_visible);
			break;
		default:  /* normal visibility */
			if (cursor_visible != NULL)
				putp(cursor_visible);
			break;
	}
	return(OK);
#endif

#if defined(DOS) || defined(OS2)
	switch(visibility)
	{
		case 0:  /* invisible */
# ifdef OS2
#  ifdef EMXVIDEO
			start = end = 0;
#  else
			start = SP->font / 4;
			end = SP->font;
#  endif
# else
			start = 32;
			end = 32;  /* was 33 */
# endif
			hidden = (-1);
			break;
		case 2:  /* highly visible */
			start = 2;   /* almost full-height block */
			end = SP->font-1;
			break;
		default:  /* normal visibility */
			start = SP->font - 4;
			end = SP->font-1;
			break;
	}

# ifdef OS2
#  ifdef EMXVIDEO
	if (hidden)
		v_hidecursor();
	else
		v_ctype (start, end);
#  else
	pvioCursorInfo.yStart = (USHORT)start;
	pvioCursorInfo.cEnd = (USHORT)end;
	pvioCursorInfo.cx = (USHORT)1;
	pvioCursorInfo.attr = hidden;
	VioSetCurType((PVIOCURSORINFO)&pvioCursorInfo,0);
#  endif
# endif

# ifdef DOS
	regs.h.ah = 0x01;
	regs.h.al = (unsigned char)SP->scrnmode;  /* if not set, some BIOSes hang */
	regs.h.ch = (unsigned char)start;
	regs.h.cl = (unsigned char)end;
	int86(0x10, &regs, &regs);
# endif
	return( ret_vis );
#endif

#if defined (XCURSES)
	XCurses_display_cursor(SP->cursrow,SP->curscol,SP->cursrow,SP->curscol);
	return(ret_vis);
#endif

}

/*man-start*********************************************************************

  PDC_set_title()	- Set window title

  PDCurses Description:
 	Sets the title of the window in which the curses program is running.
 	This function may not do anything on some platforms.

  PDCurses Return Value:
 	N/A

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	void PDC_set_title( char *title );

**man-end**********************************************************************/
/***********************************************************************/
#ifdef HAVE_PROTO
void	PDC_set_title(char *title)
#else
void	PDC_set_title(title)
char *title;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_title() - called:<%s>\n",title);
#endif

	return;
}
