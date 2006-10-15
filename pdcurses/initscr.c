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

#include <curspriv.h>
#include <stdlib.h>

RCSID("$Id: initscr.c,v 1.79 2006/10/15 02:42:25 wmcbrine Exp $");

const char *_curses_notice = "PDCurses 3.0 - Public Domain 2006";

SCREEN *SP = (SCREEN*)NULL;		/* curses variables */
WINDOW *curscr = (WINDOW *)NULL;	/* the current screen image */
WINDOW *stdscr = (WINDOW *)NULL;	/* the default screen window */

int LINES = 0;				/* current terminal height */
int COLS = 0;				/* current terminal width */
int TABSIZE = 8;

MOUSE_STATUS Mouse_status, pdc_mouse_status;

#ifdef PDC_WIDE

/* Unicode definitions for WACS_ULCORNER through WACS_STERLING, in the 
   order listed in curses.h */

cchar_t pdc_wacs[] = {
	0x250c, 0x2514, 0x2510, 0x2518, 0x2524, 0x251c, 0x2534, 0x252c,
	0x2500, 0x2502, 0x253c, 0x23ba, 0x23bd, 0x2666, 0x2592, 0x00b0,
	0x00b1, 0x00b7, 0x2190, 0x2192, 0x2193, 0x2191, 0x2591, 0x00a4,
	0x2588, 0x23bb, 0x23bc, 0x2264, 0x2265, 0x03c0, 0x2260, 0x00a3
};

#endif

extern RIPPEDOFFLINE linesripped[5];
extern char linesrippedoff;

extern void (*PDC_initial_slk)(void);

/*man-start**************************************************************

  Name:                                                       initscr

  Synopsis:
	WINDOW *initscr(void);
	WINDOW *Xinitscr(int argc, char *argv[]);
	int endwin(void);
	bool isendwin(void);
	SCREEN *newterm(char *type, FILE *outfd, FILE *infd);
	SCREEN *set_term(SCREEN *new);
	void delscreen(SCREEN *sp);

	int resize_term(int nlines, int ncols);
	bool is_termresized(void);

  X/Open Description:
	The first curses routine called should be initscr().  This will 
	determine the terminal type and initialize all curses data 
	structures.  The initscr() function also arranges that the first 
	call to refresh() will clear the screen.  If errors occur, 
	initscr() will write an appropriate error message to standard 
	error and exit.  If the program wants an indication of error 
	conditions, newterm() should be used instead of initscr().

	A program should always call endwin() before exiting or
	escaping from curses mode temporarily.  This routine will
	restore tty modes, move the cursor to the lower left corner
	of the screen and reset the terminal into the proper non-visual
	mode.  To resume curses after a temporary escape, call refresh() 
	or doupdate().

	The isendwin() function returns TRUE if endwin() has been called
	without any subsequent calls to wrefresh(), and FALSE otherwise.

	A program which outputs to more than one terminal should use
	newterm() for each terminal instead of initscr().  The newterm()
	function should be called once for each terminal.  It returns a
	value of type SCREEN* which should be saved as a reference to 
	that terminal. The arguments are the type of terminal to be 
	used in place of TERM (environment variable), a file pointer for 
	output to the terminal and another file pointer for input from 
	the terminal. The program must also call endwin() for each 
	terminal no longer being used.

	The set_term() function is used to switch between different 
	terminals. The screen reference 'new' becomes the new current 
	terminal. The previous terminal is returned by the routine.  
	This is the only routine which manipulates SCREEN pointers; all 
	other routines affect only the current terminal.

  PDCurses Description:
	The resize_term() function is used to have PDCurses change its
	internal structures to the new, specified size.

	The is_termresized() function returns TRUE if the Curses screen 
	has been resized by external means, and requires a call to 
	resize_term().

	PDCurses supports only one terminal via newterm() or set_term(), 
	and the parameters are ignored.

  X/Open Return Value:
	All functions return NULL on error, except endwin(), which
	returns ERR on error.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	initscr					Y	Y	Y
	endwin					Y	Y	Y
	isendwin				-	-      3.0
	newterm					-	-	Y
	set_term				-	-	Y
	delscreen				-	-      4.0
	resize_term				-	-	-
	is_termresized				-	-	-

**man-end****************************************************************/

WINDOW *Xinitscr(int argc, char *argv[])
{
	int i;

	PDC_LOG(("Xinitscr() - called\n"));

	if (SP != (SCREEN *)NULL && SP->alive)
		return NULL;

	if (PDC_scr_open(argc, argv) == ERR)
	{
		fprintf(stderr, "initscr(): Unable to create SP\n");
		exit(8);
	}

	SP->autocr	= TRUE;		/* cr -> lf by default	      */
	SP->raw_out	= FALSE;	/* tty I/O modes	      */
	SP->raw_inp	= FALSE;	/* tty I/O modes	      */
	SP->cbreak	= TRUE;
	SP->save_key_modifiers = FALSE;
	SP->return_key_modifiers = FALSE;
	SP->echo	= FALSE;
	SP->visibility	= 1;
	SP->audible	= TRUE;
	SP->blank	= ' ';
	SP->resized	= FALSE;
	SP->_trap_mbe	= 0L;
	SP->_map_mbe_to_key = 0L;
	SP->linesrippedoff = 0;
	SP->linesrippedoffontop = 0;
	SP->delaytenths	= 0;

	LINES = SP->lines;
	COLS = SP->cols;

	if (LINES < 2 || COLS < 2)
	{
		fprintf(stderr, "initscr(): LINES=%d COLS=%d: too small.\n",
			LINES, COLS);
		exit(4);
	}

	if ((curscr = newwin(LINES, COLS, 0, 0)) == (WINDOW *) NULL)
	{
		fprintf(stderr, "initscr(): Unable to create curscr.\n");
		exit(2);
	}

	if (PDC_initial_slk)
	{
		(*PDC_initial_slk)();
		LINES -= SP->slklines;
	}

	/* We have to sort out ripped off lines here, and reduce the
	   height of stdscr by the number of lines ripped off */

	for (i = 0; i < linesrippedoff; i++)
	{
		if (linesripped[i].line < 0)
			(*linesripped[i].init)(newwin(1, COLS,
				LINES - 1, 0), COLS);
		else
			(*linesripped[i].init)(newwin(1, COLS,
				SP->linesrippedoffontop++, 0), COLS);

		SP->linesrippedoff++;
		LINES--;
	}

	linesrippedoff = 0;

	if ((stdscr = newwin(LINES, COLS, SP->linesrippedoffontop, 0))
		== (WINDOW *) NULL)
	{
		fprintf(stderr, "initscr(): Unable to create stdscr.\n");
		exit(1);
	}

	wclrtobot(stdscr);

	/* If preserving the existing screen, don't allow a screen clear */

	if (SP->_preserve)
	{
		untouchwin(curscr);
		untouchwin(stdscr);
		stdscr->_clear = FALSE;
	}

	curscr->_clear = FALSE;

	PDC_init_atrtab();	/* set up default colors */

	MOUSE_X_POS = MOUSE_Y_POS = (-1);
	BUTTON_STATUS(1) = BUTTON_RELEASED;
	BUTTON_STATUS(2) = BUTTON_RELEASED;
	BUTTON_STATUS(3) = BUTTON_RELEASED;
	Mouse_status.changes = 0;

	SP->alive = TRUE;

	def_shell_mode();

	return stdscr;
}

WINDOW *initscr(void)
{
	PDC_LOG(("initscr() - called\n"));

	return Xinitscr(0, NULL);
}

int endwin(void)
{
	PDC_LOG(("endwin() - called\n"));

	/* Allow temporary exit from curses using endwin() */

	PDC_scr_close();

	SP->alive = FALSE;

	return OK;
}

bool isendwin(void)
{
	PDC_LOG(("isendwin() - called\n"));

	return !(SP->alive);
}

SCREEN *newterm(char *type, FILE *outfd, FILE *infd)
{
	WINDOW *result;

	PDC_LOG(("newterm() - called\n"));

	result = Xinitscr(0, NULL);

	return result ? SP : NULL;
}

SCREEN *set_term(SCREEN *new)
{
	PDC_LOG(("set_term() - called\n"));

	/* We only support one screen */

	return (new == SP) ? SP : NULL;
}

void delscreen(SCREEN *sp)
{
	PDC_LOG(("delscreen() - called\n"));

	if (sp != SP)
		return;

	delwin(stdscr);
	delwin(curscr);
	stdscr = (WINDOW *)NULL;
	curscr = (WINDOW *)NULL;

	SP->alive = FALSE;

	PDC_scr_exit();

	SP = (SCREEN *)NULL;
	pdc_atrtab = (unsigned char *)NULL;
}

int resize_term(int nlines, int ncols)
{
	PDC_LOG(("resize_term() - called: nlines %d\n", nlines));

	if (stdscr == (WINDOW *)NULL)
		return ERR;

	if (PDC_resize_screen(nlines, ncols) == ERR)
		return ERR;

	SP->lines = PDC_get_rows();
	LINES = SP->lines - SP->linesrippedoff - SP->slklines;
	SP->cols = COLS = PDC_get_columns();

	if ((curscr = resize_window(curscr, SP->lines, SP->cols)) == NULL)
		return ERR;

	if ((stdscr = resize_window(stdscr, LINES, COLS)) == NULL)
		return ERR;

	if (SP->slk_winptr)
	{
		if ((SP->slk_winptr = resize_window(SP->slk_winptr,
		     SP->slklines, COLS)) == NULL)
			return ERR;

		wmove(SP->slk_winptr, 0, 0);
		wclrtobot(SP->slk_winptr);
		(*PDC_initial_slk)();
		slk_noutrefresh();
	}

	touchwin(stdscr);
	wnoutrefresh(stdscr);

	return OK;
}

bool is_termresized(void)
{
	PDC_LOG(("is_termresized() - called\n"));

	return SP->resized;
}
