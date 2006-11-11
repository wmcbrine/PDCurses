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
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcx11.h"

RCSID("$Id: pdcscrn.c,v 1.47 2006/11/11 16:36:08 wmcbrine Exp $");

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
	PDC_LOG(("PDC_scr_close() - called\n"));
}

void PDC_scr_free(void)
{
	XCursesExit();
}

/*man-start**************************************************************

  PDC_scr_open()	- Internal low-level binding to open the physical
			  screen

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
	extern bool sb_started;

	PDC_LOG(("PDC_scr_open() - called\n"));

	if ((XCursesInitscr(argc, argv) == ERR) || !SP)
		return ERR;

	SP->cursrow = SP->curscol = 0;
	SP->orig_attr = FALSE;
	SP->sb_on = sb_started;
	SP->sb_total_y = 0;
	SP->sb_viewport_y = 0;
	SP->sb_cur_y = 0;
	SP->sb_total_x = 0;
	SP->sb_viewport_x = 0;
	SP->sb_cur_x = 0;
	SP->line_color = COLOR_WHITE;

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

	if (nlines || ncols || !SP->resized)
		return ERR;

	shmdt((char *)Xcurscr);
	XCursesInstructAndWait(CURSES_RESIZE);

	if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,
		SP->XcurscrSize + XCURSESSHMMIN, 0700)) < 0)
	{
		perror("Cannot allocate shared memory for curscr");
		kill(xc_otherpid, SIGKILL);
		return ERR;
	}

	XCursesLINES = SP->lines;
	XCursesCOLS = SP->cols;

	PDC_LOG(("%s:shmid_Xcurscr %d shmkey_Xcurscr %d SP->lines %d "
		"SP->cols %d\n", XCLOGMSG, shmid_Xcurscr, 
		shmkey_Xcurscr, SP->lines, SP->cols));

	Xcurscr = (unsigned char*)shmat(shmid_Xcurscr, 0, 0);
	pdc_atrtab = (unsigned char *)(Xcurscr + XCURSCR_ATRTAB_OFF);

	SP->resized = FALSE;

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
}

void PDC_save_screen_mode(int i)
{
}

bool PDC_can_change_color(void)
{
	return TRUE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
	XColor *tmp = (XColor *)(Xcurscr + XCURSCR_XCOLOR_OFF);

	tmp->pixel = color;

	XCursesInstructAndWait(CURSES_GET_COLOR);

	*red = ((double)(tmp->red) * 1000 / 65535) + 0.5;
	*green = ((double)(tmp->green) * 1000 / 65535) + 0.5;
	*blue = ((double)(tmp->blue) * 1000 / 65535) + 0.5;

	return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
	XColor *tmp = (XColor *)(Xcurscr + XCURSCR_XCOLOR_OFF);

	tmp->pixel = color;

	tmp->red = ((double)red * 65535 / 1000) + 0.5;
	tmp->green = ((double)green * 65535 / 1000) + 0.5;
	tmp->blue = ((double)blue * 65535 / 1000) + 0.5;

	XCursesInstructAndWait(CURSES_SET_COLOR);

	return OK;
}
