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

RCSID("$Id: pdcscrn.c,v 1.47 2006/08/12 21:13:45 wmcbrine Exp $");

union REGS regs;		/* used in various other modules 	*/

int	pdc_adapter;		/* screen type				*/
int	pdc_scrnmode;		/* default screen mode			*/
int	pdc_font;		/* default font size			*/
bool	pdc_direct_video;	/* allow direct screen memory writes	*/
bool	pdc_bogus_adapter;	/* TRUE if adapter has insane values	*/
unsigned pdc_video_seg;		/* video base segment			*/
unsigned pdc_video_ofs;		/* video base offset			*/

static unsigned short *saved_screen = NULL;
static int saved_lines = 0;
static int saved_cols = 0;

static int saved_scrnmode[3];
static int saved_font[3];

/*man-start**************************************************************

  PDC_scr_close()	- Internal low-level binding to close the
			  physical screen

  PDCurses Description:
	May restore the screen to its state before PDC_scr_open(); 
	miscellaneous cleanup.

  PDCurses Return Value:
	This function returns OK on success, otherwise an ERR is returned.

  Portability:
	PDCurses  void PDC_scr_close(void);

**man-end****************************************************************/

void PDC_scr_close(void)
{
#if SMALL || MEDIUM
# ifndef __PACIFIC__
	struct SREGS segregs;
# endif
	int ds;
#endif
	PDC_LOG(("PDC_scr_close() - called\n"));

	if ((getenv("PDC_RESTORE_SCREEN") != NULL) && (saved_screen != NULL))
	{
#ifdef __DJGPP__
		dosmemput(saved_screen, saved_lines * saved_cols * 2,
			(unsigned long)_FAR_POINTER(pdc_video_seg,
			pdc_video_ofs));
#else
# if (SMALL || MEDIUM)
#  ifdef __PACIFIC__
		ds = FP_SEG((void far *)saved_screen);
#  else
		segread(&segregs);
		ds = segregs.ds;
#  endif
		movedata(ds, (int)saved_screen, pdc_video_seg, pdc_video_ofs,
		(saved_lines * saved_cols * 2));
# else
		memcpy((void *)_FAR_POINTER(pdc_video_seg, pdc_video_ofs),
		(void *)saved_screen, (saved_lines * saved_cols * 2));
# endif
#endif
		free(saved_screen);
		saved_screen = NULL;
	}

	reset_shell_mode();

	if (SP->visibility != 1)
		curs_set(1);

	/* Position cursor to the bottom left of the screen. */

	PDC_gotoyx(PDC_get_rows() - 2, 0);
}

void PDC_scr_exit(void)
{
	if (SP)
		free(SP);
	if (atrtab)
		free(atrtab);
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

	SP = calloc(1, sizeof(SCREEN));
	atrtab = calloc(MAX_ATRTAB, 1);

	if (!SP || !atrtab)
		return ERR;

	SP->orig_attr	= FALSE;

	PDC_get_cursor_pos(&SP->cursrow, &SP->curscol);

	pdc_direct_video = TRUE;	/* Assume that we can	      */
	pdc_video_seg	= 0xb000;	/* Base screen segment addr   */
	pdc_video_ofs	= 0x0;		/* Base screen segment ofs    */

	pdc_adapter	= PDC_query_adapter_type();
	pdc_scrnmode	= PDC_get_scrn_mode();
	pdc_font	= PDC_get_font();

	SP->lines	= PDC_get_rows();
	SP->cols	= PDC_get_columns();

	SP->orig_cursor	= PDC_get_cursor_mode();
	SP->orgcbr	= PDC_get_ctrl_break();

	/* If the environment variable PDCURSES_BIOS is set, the DOS 
	   int10() BIOS calls are used in place of direct video memory 
	   access. */

	if (getenv("PDCURSES_BIOS") != NULL)
		pdc_direct_video = FALSE;

	/* This code for preserving the current screen. */

	if (getenv("PDC_RESTORE_SCREEN") != NULL)
	{
		saved_lines = SP->lines;
		saved_cols = SP->cols;

		saved_screen = malloc(saved_lines * saved_cols * 2);

		if (!saved_screen)
		{
			SP->_preserve = FALSE;
			return OK;
		}
#ifdef __DJGPP__
		dosmemget ((unsigned long)_FAR_POINTER(pdc_video_seg,
			pdc_video_ofs), saved_lines * saved_cols * 2,
			saved_screen);
#else
# if SMALL || MEDIUM
#  ifdef __PACIFIC__
		ds = FP_SEG((void far *) saved_screen);
#  else
		segread(&segregs);
		ds = segregs.ds;
#  endif
		movedata(pdc_video_seg, pdc_video_ofs, ds, (int)saved_screen,
			(saved_lines * saved_cols * 2));
# else
		memcpy((void *)saved_screen,
			(void *)_FAR_POINTER(pdc_video_seg, pdc_video_ofs),
			(saved_lines * saved_cols * 2));
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

	switch (pdc_adapter)
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

void PDC_reset_prog_mode(void)
{
        PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
}

void PDC_reset_shell_mode(void)
{
        PDC_LOG(("PDC_reset_shell_mode() - called.\n"));
}

void PDC_restore_screen_mode(int i)
{
	if (i >= 0 && i <= 2)
	{
		pdc_font = PDC_get_font();
		PDC_set_font(saved_font[i]);

		if (PDC_get_scrn_mode() != saved_scrnmode[i])
			PDC_set_scrn_mode(saved_scrnmode[i]);
	}
}

void PDC_save_screen_mode(int i)
{
	if (i >= 0 && i <= 2)
	{
		saved_font[i] = pdc_font;
		saved_scrnmode[i] = pdc_scrnmode;
	}
}
