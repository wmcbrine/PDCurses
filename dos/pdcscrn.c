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

#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif

#ifndef HAVE_MEMMOVE
#  define memmove PDC_memmove
#endif

#ifdef GO32
# include <sys/movedata.h>
#endif

#ifdef PDCDEBUG
char *rcsid_PDCscrn  = "$Id: pdcscrn.c,v 1.2 2001/01/10 08:28:48 mark Exp $";
#endif

	static unsigned short *saved_screen = NULL;
	static int saved_lines = 0;
	static int saved_cols = 0;

/*man-start*********************************************************************

  PDC_scr_close()	- Internal low-level binding to close the physical screen

  PDCurses Description:
 	This function provides a low-level binding for the Flexos
 	platform which must close the screen before writing to it.
 	This is a nop for the DOS platform.

 	This function is provided in order to reset the FlexOS 16 bit
 	character set for input rather than the limited input
 	character set associated with the VT52.

  PDCurses Return Value:
 	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
 	The DOS platform will never fail.  The Flexos platform may fail
 	depending on the ability to close the current virtual console in
 	8 (as opposed to 16) bit mode.

  Portability:
 	PDCurses	int	PDC_scr_close( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_scr_close(void)
#else
int	PDC_scr_close()
#endif
/***********************************************************************/
{
#if SMALL || MEDIUM
struct SREGS segregs;
int ds=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scr_close() - called\n");
#endif
	if (getenv("PDC_RESTORE_SCREEN") != NULL)
	{
		if (saved_screen == NULL)
			return( OK );
#ifdef GO32
		dosmemput(saved_screen,
			saved_lines * saved_cols * sizeof(unsigned short),
		(unsigned long)_FAR_POINTER(SP->video_seg,SP->video_ofs));
#else
#  if	(SMALL || MEDIUM)
	segread(&segregs);
	ds = segregs.ds;
	movedata(ds, (int)saved_screen,
		SP->video_seg,SP->video_ofs,
		(saved_lines * saved_cols * sizeof(unsigned short)));
#  else
	memcpy((void *)_FAR_POINTER(SP->video_seg,SP->video_ofs),
		(void*)saved_screen,
		(saved_lines * saved_cols * sizeof(unsigned short)));
#  endif
#endif
		free(saved_screen);
		saved_screen = NULL;
	}
	return( OK );
}

/*man-start*********************************************************************

  PDC_scrn_modes_equal()	- Decide if two screen modes are equal

  PDCurses Description:
 	Mainly required for OS/2. It decides if two screen modes
        (VIOMODEINFO structure) are equal. Under DOS it just compares
        two integers

  PDCurses Return Value:
 	This function returns TRUE if equal else FALSe.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_scrn_modes_equal( int mode1, int mode2 );
 	OS2 PDCurses	int PDC_scrn_modes_equal( VIOMODEINFO mode1, VIOMODEINFO mode2 );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_scrn_modes_equal(int mode1, int mode2)
#else
bool	PDC_scrn_modes_equal(mode1,mode2)
int mode1;
int mode2;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scrn_modes_equal() - called\n");
#endif

	return (mode1 == mode2);
}

/*man-start*********************************************************************

  PDC_scr_open()	- Internal low-level binding to open the physical screen

  PDCurses Description:
 	This function provides a low-level binding for the Flexos
 	platform which must open the screen before writing to it.

 	This function is provided in order to access the FlexOS 16 bit
 	character set for input rather than the limited input
 	character set associated with the VT52.

  PDCurses Return Value:
 	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
 	The DOS platform will never fail.  The Flexos platform may fail
 	depending on the ability to open the current virtual console in
 	8 (as opposed to 16) bit mode.

  Portability:
 	PDCurses	int	PDC_scr_open( SCREEN* internal, bool echo );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_scr_open(SCREEN *internal, bool echo)
#else
int	PDC_scr_open(internal,echo)
SCREEN *internal;
bool echo;
#endif
/***********************************************************************/
{
#if SMALL || MEDIUM
struct SREGS segregs;
int ds=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scr_open() - called\n");
#endif

	internal->orig_attr	 = 0;
	internal->orig_emulation = getdosmembyte (0x487);

	PDC_get_cursor_pos(&internal->cursrow, &internal->curscol);
	internal->direct_video	= TRUE;		/* Assume that we can	      */
	internal->autocr	= TRUE;		/* lf -> crlf by default      */
	internal->raw_out	= FALSE;	/* tty I/O modes	      */
	internal->raw_inp	= FALSE;	/* tty I/O modes	      */
	internal->cbreak	= TRUE;
	internal->save_key_modifiers	= FALSE;
	internal->return_key_modifiers	= FALSE;
	internal->echo		= echo;
/* under System V Curses, typeahead checking is enabled by default */
	internal->refrbrk	= TRUE;	/* allow premature end of refresh*/
	internal->video_seg	= 0xb000;	/* Base screen segment addr   */
	internal->video_ofs	= 0x0;		/* Base screen segment ofs    */
	internal->video_page	= 0;		/* Current Video Page	      */
	internal->visible_cursor= TRUE;		/* Assume that it is visible  */
	internal->cursor	= PDC_get_cursor_mode();

	internal->adapter	= PDC_query_adapter_type();
	internal->scrnmode	= PDC_get_scrn_mode();

	internal->orig_font = internal->font	= PDC_get_font();
	internal->lines		= PDC_get_rows();
	internal->cols		= PDC_get_columns();

	internal->audible	= TRUE;
	internal->visibility	= 1;
	internal->orig_cursor = internal->cursor;
	internal->orgcbr = PDC_get_ctrl_break();
	internal->blank = ' ';
	internal->resized = FALSE;
	internal->shell = FALSE;
	internal->_trap_mbe = 0L;
	internal->_map_mbe_to_key = 0L;
	internal->linesrippedoff = 0;
	internal->linesrippedoffontop = 0;
	internal->delaytenths = 0;
/*
 * If the environment variable PDCURSES_BIOS is set, the DOS int10()
 * BIOS calls are used in place of direct video memory access.
 */
	internal->direct_video = TRUE;
	if (getenv("PDCURSES_BIOS") != NULL)
		internal->direct_video = FALSE;
/*
 * This code for preserving the current screen...
 */
	if (getenv("PDC_RESTORE_SCREEN") != NULL)
	{
		saved_lines = internal->lines;
		saved_cols = internal->cols;
		if ((saved_screen = (unsigned short*)malloc(saved_lines*saved_cols*sizeof(unsigned short))) == NULL)
			return(ERR);

#ifdef GO32
		dosmemget ((unsigned long)_FAR_POINTER(SP->video_seg,SP->video_ofs),
			saved_lines * saved_cols * sizeof(unsigned short),saved_screen);
#else
#  if	(SMALL || MEDIUM)
	segread(&segregs);
	ds = segregs.ds;
	movedata(SP->video_seg,SP->video_ofs,
		ds, (int)saved_screen,
		(saved_lines * saved_cols * sizeof(unsigned short)));
#  else
	memcpy((void*)saved_screen,
		(void *)_FAR_POINTER(SP->video_seg,SP->video_ofs),
		(saved_lines * saved_cols * sizeof(unsigned short)));
#  endif
#endif
	}

	if (getenv("PDC_PRESERVE_SCREEN") != NULL)
		internal->_preserve = TRUE;
	else
		internal->_preserve = FALSE;

	return( OK );
}

/*man-start*********************************************************************

  PDC_resize_screen()	- Internal low-level function to resize screen

  PDCurses Description:
 	This function provides a means for the application program to
 	resize the overall dimensions of the screen.  Under DOS and OS/2
 	the application can tell PDCurses what size to make the screen;
 	under X11, resizing is done by the user and this function simply
 	adjusts its internal structures to fit the new size.

  PDCurses Return Value:
 	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:

  Portability:
 	PDCurses	int	PDC_resize_screen( int, int );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_resize_screen(int nlines, int ncols)
#else
int	PDC_resize_screen(nlines, ncols)
int nlines,ncols;
#endif
/***********************************************************************/
{
	int rc=OK;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_resize_screen() - called. Lines: %d Cols: %d\n",nlines,ncols);
#endif

	switch (SP->adapter)
	{
	case _EGACOLOR:
		if (nlines >= 43)		PDC_set_font(_FONT8);
		else				PDC_set_80x25();
		break;

	case _VGACOLOR:
		if	(nlines > 28)		PDC_set_font(_FONT8);
		else	if (nlines > 25)	PDC_set_font(_FONT14);
		else				PDC_set_80x25();
		break;

	default:
		break;
	}

	return ( rc );
}
