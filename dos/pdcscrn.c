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

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <stdlib.h>

#ifdef HAVE_MEMORY_H
# include <memory.h>
#endif

#ifdef __DJGPP__
# include <sys/movedata.h>
#endif

#ifdef PDCDEBUG
const char *rcsid_PDCscrn =
	"$Id: pdcscrn.c,v 1.23 2006/03/25 00:41:04 wmcbrine Exp $";
#endif

static unsigned short *saved_screen = NULL;
static int saved_lines = 0;
static int saved_cols = 0;

/*man-start**************************************************************

  PDC_scr_close()	- Internal low-level binding to close the
			  physical screen

  PDCurses Description:
	This is a nop for the DOS platform.

  PDCurses Return Value:
	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
	The DOS platform will never fail.

  Portability:
	PDCurses  int PDC_scr_close(void);

**man-end****************************************************************/

int PDC_scr_close(void)
{
#if SMALL || MEDIUM
# if !PC
	struct SREGS segregs;
# endif
	int ds;
#endif
	PDC_LOG(("PDC_scr_close() - called\n"));

	if (getenv("PDC_RESTORE_SCREEN") != NULL)
	{
		if (saved_screen == NULL)
			return OK;
#ifdef __DJGPP__
		dosmemput(saved_screen,
			saved_lines * saved_cols * sizeof(unsigned short),
			(unsigned long)_FAR_POINTER(SP->video_seg,
			SP->video_ofs));
#else
# if (SMALL || MEDIUM)
#  if PC
		ds = FP_SEG((void far *)saved_screen);
#  else
		segread(&segregs);
		ds = segregs.ds;
#  endif
		movedata(ds, (int)saved_screen,
		SP->video_seg,SP->video_ofs,
		(saved_lines * saved_cols * sizeof(unsigned short)));
# else
		memcpy((void *)_FAR_POINTER(SP->video_seg, SP->video_ofs),
		(void *)saved_screen,
		(saved_lines * saved_cols * sizeof(unsigned short)));
# endif
#endif
		free(saved_screen);
		saved_screen = NULL;
	}

	return OK;
}

/*man-start**************************************************************

  PDC_scrn_modes_equal()	- Decide if two screen modes are equal

  PDCurses Description:
	Mainly required for OS/2. It decides if two screen modes
	(VIOMODEINFO structure) are equal. Under DOS it just compares
	two integers.

  PDCurses Return Value:
	This function returns TRUE if equal else FALSE.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses      int PDC_scrn_modes_equal(int mode1, int mode2);
	OS2 PDCurses  int PDC_scrn_modes_equal(VIOMODEINFO mode1, 
					       VIOMODEINFO mode2);

**man-end****************************************************************/

bool PDC_scrn_modes_equal(int mode1, int mode2)
{
	PDC_LOG(("PDC_scrn_modes_equal() - called\n"));

	return (mode1 == mode2);
}

/*man-start**************************************************************

  PDC_scr_open()	- Internal low-level binding to open the
			  physical screen

  PDCurses Description:
	This is a NOP for the DOS platform.

  PDCurses Return Value:
	This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
	The DOS platform will never fail.

  Portability:
	PDCurses  int PDC_scr_open(SCREEN *internal, bool echo);

**man-end****************************************************************/

int PDC_scr_open(SCREEN *internal, bool echo)
{
#if SMALL || MEDIUM
# if !PC
	struct SREGS segregs;
# endif
	int ds;
#endif
	PDC_LOG(("PDC_scr_open() - called\n"));

	internal->orig_attr	 = FALSE;
	internal->orig_emulation = getdosmembyte (0x487);

	PDC_get_cursor_pos(&internal->cursrow, &internal->curscol);

	internal->direct_video	= TRUE;		/* Assume that we can	      */
	internal->autocr	= TRUE;		/* cr -> lf by default	      */
	internal->raw_out	= FALSE;	/* tty I/O modes	      */
	internal->raw_inp	= FALSE;	/* tty I/O modes	      */
	internal->cbreak	= TRUE;
	internal->save_key_modifiers = FALSE;
	internal->return_key_modifiers = FALSE;
	internal->echo		= echo;
	internal->video_seg	= 0xb000;	/* Base screen segment addr   */
	internal->video_ofs	= 0x0;		/* Base screen segment ofs    */
	internal->video_page	= 0;		/* Current Video Page	      */
	internal->visible_cursor= TRUE;		/* Assume that it is visible  */
	internal->cursor	= PDC_get_cursor_mode();

	internal->adapter	= PDC_query_adapter_type();
	internal->scrnmode	= PDC_get_scrn_mode();

	internal->orig_font = internal->font = PDC_get_font();
	internal->lines		= PDC_get_rows();
	internal->cols		= PDC_get_columns();

	internal->audible	= TRUE;
	internal->visibility	= 1;
	internal->orig_cursor	= internal->cursor;
	internal->orgcbr	= PDC_get_ctrl_break();
	internal->blank		= ' ';
	internal->resized	= FALSE;
	internal->shell		= FALSE;
	internal->_trap_mbe	= 0L;
	internal->_map_mbe_to_key = 0L;
	internal->linesrippedoff = 0;
	internal->linesrippedoffontop = 0;
	internal->delaytenths	= 0;

	/* If the environment variable PDCURSES_BIOS is set, the DOS 
	   int10() BIOS calls are used in place of direct video memory 
	   access. */

	if (getenv("PDCURSES_BIOS") != NULL)
		internal->direct_video = FALSE;

	/* This code for preserving the current screen. */

	if (getenv("PDC_RESTORE_SCREEN") != NULL)
	{
		saved_lines = internal->lines;
		saved_cols = internal->cols;
		if ((saved_screen = (unsigned short*)malloc(saved_lines
		    * saved_cols * sizeof(unsigned short))) == NULL)
			return ERR;

#ifdef __DJGPP__
		dosmemget ((unsigned long)_FAR_POINTER(SP->video_seg,
			SP->video_ofs), saved_lines * saved_cols * 
			sizeof(unsigned short), saved_screen);
#else
# if	(SMALL || MEDIUM)
#  if PC
		ds = FP_SEG((void far *) saved_screen);
#  else
		segread(&segregs);
		ds = segregs.ds;
#  endif
		movedata(SP->video_seg,SP->video_ofs, ds, (int)saved_screen,
			(saved_lines * saved_cols * sizeof(unsigned short)));
# else
		memcpy((void*)saved_screen,
			(void *)_FAR_POINTER(SP->video_seg, SP->video_ofs),
			(saved_lines * saved_cols * sizeof(unsigned short)));
# endif
#endif
	}

	internal->_preserve = (getenv("PDC_PRESERVE_SCREEN") != NULL);

	return OK;
}

/*man-start**************************************************************

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
	PDCurses  int PDC_resize_screen(int, int);

**man-end****************************************************************/

int PDC_resize_screen(int nlines, int ncols)
{
	PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
		nlines, ncols));

	switch (SP->adapter)
	{
	case _EGACOLOR:
		if (nlines >= 43)
			PDC_set_font(_FONT8);
		else
			PDC_set_80x25();
		break;

	case _VGACOLOR:
		if (nlines > 28)
			PDC_set_font(_FONT8);
		else
			if (nlines > 25)
				PDC_set_font(_FONT14);
			else
				PDC_set_80x25();
	}

	return OK;
}
