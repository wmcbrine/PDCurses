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
#include <curses.h>
#include <stdlib.h>

/* undefine any macros for functions defined in this module */
#undef initscr
#undef endwin
#undef isendwin
#undef newterm
#undef set_term
#undef delscreen
#undef resize_term
#undef is_termresized

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef move
# undef wmove
# undef resize_term
# undef touchwin
# undef wnoutrefresh
#endif

RCSID("$Id: initscr.c,v 1.46 2006/07/11 19:14:41 wmcbrine Exp $");

const char *_curses_notice = "PDCurses 2.8 - Public Domain 2006";

SCREEN *SP = (SCREEN*)NULL;		/* curses variables */
WINDOW *curscr = (WINDOW *)NULL;	/* the current screen image */
WINDOW *stdscr = (WINDOW *)NULL;	/* the default screen window */

int _default_lines = 25;		/* default terminal height */
int LINES = 0;				/* current terminal height */
int COLS = 0;				/* current terminal width */

MOUSE_STATUS Mouse_status;

int use_emalloc = FALSE;

/* Global definitions for charget routines */

int c_pindex = 0;			/* putter index */
int c_gindex = 1;			/* getter index */
int c_ungind = 0;			/* wungetch() push index */
int c_ungch[NUNGETCH];			/* array of ungotten chars */

WINDOW *_getch_win_ = NULL;

/* Global definitions for setmode routines */

struct cttyset c_sh_tty = {0};		/* tty modes for def_shell_mode */
struct cttyset c_pr_tty = {0};		/* tty modes for def_prog_mode  */
struct cttyset c_save_tty = {0};
struct cttyset c_save_trm = {0};

#ifdef CHTYPE_LONG
cchar_t _wacs_map[] = {
	ACS_ULCORNER, ACS_LLCORNER, ACS_URCORNER, ACS_LRCORNER, 
	ACS_RTEE, ACS_LTEE, ACS_BTEE, ACS_TTEE, ACS_HLINE, ACS_VLINE, 
	ACS_PLUS, ACS_S1, ACS_S9, ACS_DIAMOND, ACS_CKBOARD, ACS_DEGREE, 
	ACS_PLMINUS, ACS_BULLET, ACS_LARROW, ACS_RARROW, ACS_DARROW, 
	ACS_UARROW, ACS_BOARD, ACS_LANTERN, ACS_BLOCK, ACS_S3, ACS_S7, 
	ACS_LEQUAL, ACS_GEQUAL, ACS_PI, ACS_NEQUAL, ACS_STERLING
};
#endif

#if EMALLOC
extern void *emalloc(size_t);		/* user's emalloc(size) */
extern void *ecalloc(size_t, size_t);	/* user's ecalloc(num, size) */
extern void efree(void *);		/* user's efree(ptr) */
#endif

void* (*mallc)(size_t);			/* ptr to some malloc(size) */
void* (*callc)(size_t, size_t);		/* ptr to some ecalloc(num, size) */
void  (*fre)(void *);			/* ptr to some free(ptr) */
void* (*reallc)(void *, size_t);	/* ptr to some realloc(ptr, size) */

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

  X/Open Errors:
	No errors are defined for this function.

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

#ifdef EMXVIDEO
	v_init();
#endif

#if EMALLOC
	if (use_emalloc == EMALLOC_MAGIC)
	{
		use_emalloc = TRUE;
		mallc = emalloc;
		callc = ecalloc;
		fre = efree;
		reallc = erealloc;
	}
	else
#endif
	{
		mallc = malloc;
		callc = calloc;
		fre = free;
		reallc = realloc;
	}

#ifdef XCURSES
	if (XCursesInitscr(NULL, argc, argv) == ERR)
		exit(7);
#endif

#ifdef XCURSES
	if (SP == (SCREEN*)NULL)  /* SP already attached in XCursesInitscr() */
#else
	if ((SP = (SCREEN*)callc(1, sizeof(SCREEN))) == (SCREEN*)NULL)
#endif
	{
		fprintf(stderr, "initscr(): Unable to create SP\n");
		exit(8);
	}

	PDC_scr_open(SP, 0);

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

#ifdef EMXVIDEO
	SP->tahead = -1;
#endif

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

#if 0
	/* resetty(); */
	if (SP->orig_font != SP->font)  /* screen has not been resized */
	{
		PDC_set_font(SP->orig_font);
		resize_term(PDC_get_rows(), PDC_get_columns());
	}

	SP->visible_cursor = FALSE;   /* Force the visible cursor */
	SP->cursor = SP->orig_cursor;
#endif

#if defined(DOS) || defined(OS2)
	reset_shell_mode();
#endif

	curs_set(1);

	/* Position cursor to the bottom left of the screen. */

#ifdef WIN32
	PDC_gotoxy(PDC_get_buffer_rows() - 2, 0);
#else
	PDC_gotoxy(PDC_get_rows() - 2, 0);
#endif

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

#ifndef XCURSES
	if (SP)
	{
		fre(SP);
		SP = (SCREEN *)NULL;
	}
#endif
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

	/* change the saved prog_mode details */

	if (c_pr_tty.been_set)
	{
		c_pr_tty.saved.lines = SP->lines;
		c_pr_tty.saved.cols = SP->cols;
	}

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
