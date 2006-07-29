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
#define INCL_DOSMISC
#include <curses.h>
#include <stdlib.h>

RCSID("$Id: pdcscrn.c,v 1.33 2006/07/29 06:00:54 wmcbrine Exp $");

#ifdef EMXVIDEO
static unsigned char *saved_screen = NULL;
static int saved_lines = 0;
static int saved_cols = 0;
#else
static PCH saved_screen = NULL;
static USHORT saved_lines = 0;
static USHORT saved_cols = 0;
#endif

/*man-start**************************************************************

  PDC_scr_close() - Internal low-level binding to close the physical screen

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
	char *ptr;

	PDC_LOG(("PDC_scr_close() - called\n"));

#ifdef EMXVIDEO
	ptr = getenv("PDC_RESTORE_SCREEN");
#else
	if (DosScanEnv("PDC_RESTORE_SCREEN", (PSZ *)&ptr))
		ptr = NULL;
#endif
	if (ptr != NULL)
	{
		if (saved_screen == NULL)
			return OK;
#ifdef EMXVIDEO
		v_putline(saved_screen, 0, 0, saved_lines * saved_cols);
#else
		VioWrtCellStr(saved_screen, saved_lines * saved_cols * 2,
			0, 0, (HVIO)NULL);
#endif
		free(saved_screen);
		saved_screen = NULL;
	}

	return OK;
}

/*man-start**************************************************************

  PDC_scrn_modes_equal()   - Decide if two screen modes are equal

  PDCurses Description:
	Mainly required for OS/2. It decides if two screen modes 
	(VIOMODEINFO structure) are equal. Under DOS it just compares 
	two integers.

  PDCurses Return Value:
	This function returns TRUE if equal else FALSE.

  Portability:
	PDCurses  int PDC_scrn_modes_equal(int mode1, int mode2);
    OS2 PDCurses  int PDC_scrn_modes_equal(VIOMODEINFO mode1,
					   VIOMODEINFO mode2);

**man-end****************************************************************/

#ifndef EMXVIDEO
bool PDC_scrn_modes_equal(VIOMODEINFO mode1, VIOMODEINFO mode2)
#else
bool PDC_scrn_modes_equal(int mode1, int mode2)
#endif
{
	PDC_LOG(("PDC_scrn_modes_equal() - called\n"));

#ifndef EMXVIDEO
	return ((mode1.cb == mode2.cb) && (mode1.fbType == mode2.fbType)
		&& (mode1.color == mode2.color) && (mode1.col == mode2.col)
		&& (mode1.row == mode2.row) && (mode1.hres == mode2.vres)
		&& (mode1.vres == mode2.vres));
#else
	return (mode1 == mode2);
#endif
}

/*man-start**************************************************************

  PDC_scr_open()  - Internal low-level binding to open the physical screen

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
	char *ptr;
#ifndef EMXVIDEO
	USHORT totchars;
#endif
	PDC_LOG(("PDC_scr_open() - called\n"));

	if ((SP = (SCREEN *)calloc(1, sizeof(SCREEN))) == (SCREEN *)NULL)
		return ERR;

#ifdef EMXVIDEO
        v_init();
#endif
	SP->orig_attr = FALSE;

	PDC_get_cursor_pos(&SP->cursrow, &SP->curscol);

#ifdef EMXVIDEO
	SP->tahead  = -1;
#endif

#ifndef EMXVIDEO
	PDC_query_adapter_type(&SP->adapter);
	PDC_get_scrn_mode(&SP->scrnmode);
	PDC_get_keyboard_info(&SP->kbdinfo);

	PDC_LOG(("PDC_scr_open() - after PDC_get_keyboard_info(). cb: %x, "
		"fsMask: %x, chTurnAround: %x, fsInterim: %x, fsState: %x\n",
		SP->kbdinfo.cb, SP->kbdinfo.fsMask, 
		SP->kbdinfo.chTurnAround, SP->kbdinfo.fsInterim, 
		SP->kbdinfo.fsState));

	/* Now set the keyboard into binary mode */

	PDC_set_keyboard_binary();
#else
	SP->adapter = PDC_query_adapter_type();
	if (SP->adapter == _UNIX_MONO)
		SP->mono = TRUE;
#endif
	SP->orig_font = SP->font = PDC_get_font();
	SP->lines = PDC_get_rows();
	SP->cols = PDC_get_columns();
	SP->orig_cursor = PDC_get_cursor_mode();
	SP->orgcbr = PDC_get_ctrl_break();
	SP->sizeable = TRUE;

	/* This code for preserving the current screen */

#ifdef EMXVIDEO
	ptr = getenv("PDC_RESTORE_SCREEN");
#else
	if (DosScanEnv("PDC_RESTORE_SCREEN", (PSZ *)&ptr))
		ptr = NULL;
#endif
	if (ptr != NULL)
	{
		saved_lines = SP->lines;
		saved_cols = SP->cols;
#ifdef EMXVIDEO
		if ((saved_screen = (unsigned char *)malloc(2 * saved_lines *
		     saved_cols)) == NULL)
		{
			SP->_preserve = FALSE;
			return OK;
		}

		v_getline(saved_screen, 0, 0, saved_lines * saved_cols);
#else
		if ((saved_screen = (PCH)malloc(2 * saved_lines *
		     saved_cols)) == NULL)
		{
			SP->_preserve = FALSE;
			return OK;
		}

		totchars = saved_lines * saved_cols * 2;
		VioReadCellStr((PCH)saved_screen, &totchars, 0, 0, (HVIO)NULL);
#endif
	}

#ifdef EMXVIDEO
	ptr = getenv("PDC_PRESERVE_SCREEN");
#else
	if (DosScanEnv("PDC_PRESERVE_SCREEN", (PSZ *)&ptr))
		ptr = NULL;
#endif
	SP->_preserve = (ptr != NULL);

	return OK;
}

/*man-start**************************************************************

  PDC_resize_screen()   - Internal low-level function to resize screen

  PDCurses Description:
	This function provides a means for the application program to
	resize the overall dimensions of the screen.  Under DOS and OS/2
	the application can tell PDCurses what size to make the screen;
	under X11, resizing is done by the user and this function simply
	adjusts its internal structures to fit the new size.

  PDCurses Return Value:
	This function returns OK on success, otherwise an ERR is 
	returned.

  PDCurses Errors:

  Portability:
	PDCurses  int PDC_resize_screen(int, int);

**man-end****************************************************************/

int PDC_resize_screen(int nlines, int ncols)
{
#ifndef EMXVIDEO
	VIOMODEINFO modeInfo = {0};
	USHORT result;
#endif

	PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
		nlines, ncols));

#ifdef EMXVIDEO
	return ERR;
#else
	modeInfo.cb = sizeof(modeInfo);

	/* set most parameters of modeInfo */

	VioGetMode(&modeInfo, 0);
	modeInfo.fbType = 1;
	modeInfo.row = nlines;
	modeInfo.col = ncols;
	result = VioSetMode(&modeInfo, 0);

	LINES = PDC_get_rows();
	COLS = PDC_get_columns();

	return (result == 0) ? OK : ERR;
#endif
}

#ifndef EMXVIDEO

int PDC_reset_shell_mode(void)
{
	PDC_LOG(("PDC_reset_shell_mode() - called.\n"));

	PDC_set_keyboard_default();

	return OK;
}

int PDC_reset_prog_mode(void)
{
	PDC_LOG(("PDC_reset_prog_mode() - called.\n"));

	PDC_set_keyboard_binary();

	return OK;
}

#endif
