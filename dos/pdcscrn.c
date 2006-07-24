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

#include "pdcdos.h"

#include <stdlib.h>
#include <string.h>

#ifdef __DJGPP__
# include <sys/movedata.h>
#endif

RCSID("$Id: pdcscrn.c,v 1.36 2006/07/24 20:34:18 wmcbrine Exp $");

Regs regs;	/* used in various other modules */

static unsigned short *saved_screen = NULL;
static int saved_lines = 0;
static int saved_cols = 0;

/*man-start**************************************************************

  PDC_scr_close()	- Internal low-level binding to close the
			  physical screen

  PDCurses Description:
	May restore the screen to its state before PDC_scr_open(); 
	miscellaneous cleanup.

  PDCurses Return Value:
	This function returns OK on success, otherwise an ERR is returned.

  Portability:
	PDCurses  int PDC_scr_close(void);

**man-end****************************************************************/

int PDC_scr_close(void)
{
#if SMALL || MEDIUM
# ifndef __PACIFIC__
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
#  ifdef __PACIFIC__
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

  PDC_scr_open()	- Internal low-level binding to open the
			  physical screen

  PDCurses Description:
	The platform-specific part of initscr() -- allocates SP, does 
	miscellaneous intialization, and may save the existing screen 
	for later restoration.

  PDCurses Return Value:
	This function returns OK on success, otherwise an ERR is returned.

  Portability:
	PDCurses  int PDC_scr_open(int argc, char **argv);

**man-end****************************************************************/

int PDC_scr_open(int argc, char **argv)
{
#if SMALL || MEDIUM
# ifndef __PACIFIC__
	struct SREGS segregs;
# endif
	int ds;
#endif
	PDC_LOG(("PDC_scr_open() - called\n"));

        if ((SP = (SCREEN *)calloc(1, sizeof(SCREEN))) == (SCREEN *)NULL)
		return ERR;

	SP->orig_attr	 = FALSE;
	SP->orig_emulation = getdosmembyte(0x487);

	PDC_get_cursor_pos(&SP->cursrow, &SP->curscol);

	SP->direct_video = TRUE;	/* Assume that we can	      */
	SP->video_seg	= 0xb000;	/* Base screen segment addr   */
	SP->video_ofs	= 0x0;		/* Base screen segment ofs    */
	SP->video_page	= 0;		/* Current Video Page	      */

	SP->adapter	= PDC_query_adapter_type();
	SP->scrnmode	= PDC_get_scrn_mode();

	SP->orig_font = SP->font = PDC_get_font();
	SP->lines	= PDC_get_rows();
	SP->cols	= PDC_get_columns();

	SP->orig_cursor	= PDC_get_cursor_mode();
	SP->orgcbr	= PDC_get_ctrl_break();

	/* If the environment variable PDCURSES_BIOS is set, the DOS 
	   int10() BIOS calls are used in place of direct video memory 
	   access. */

	if (getenv("PDCURSES_BIOS") != NULL)
		SP->direct_video = FALSE;

	/* This code for preserving the current screen. */

	if (getenv("PDC_RESTORE_SCREEN") != NULL)
	{
		saved_lines = SP->lines;
		saved_cols = SP->cols;
		if ((saved_screen = (unsigned short*)malloc(saved_lines
		    * saved_cols * sizeof(unsigned short))) == NULL)
		{
			SP->_preserve = FALSE;
			return OK;
		}
#ifdef __DJGPP__
		dosmemget ((unsigned long)_FAR_POINTER(SP->video_seg,
			SP->video_ofs), saved_lines * saved_cols * 
			sizeof(unsigned short), saved_screen);
#else
# if SMALL || MEDIUM
#  ifdef __PACIFIC__
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

	SP->_preserve = (getenv("PDC_PRESERVE_SCREEN") != NULL);

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

	/* Trash the stored value of orig_cursor -- it's only good if 
	   the video mode doesn't change */

	SP->orig_cursor = 0x0607;

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
