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
#include <string.h>

RCSID("$Id: kernel.c,v 1.71 2007/04/13 08:24:20 wmcbrine Exp $");

RIPPEDOFFLINE linesripped[5];
char linesrippedoff = 0;

static struct cttyset
{
        bool    been_set;
        SCREEN  saved;
} ctty[3];

enum { PDC_SH_TTY, PDC_PR_TTY, PDC_SAVE_TTY };

/*man-start**************************************************************

  Name:								kernel

  Synopsis:
	int def_prog_mode(void);
	int def_shell_mode(void);
	int reset_prog_mode(void);
	int reset_shell_mode(void);
	int resetty(void);
	int savetty(void);
	void getsyx(int y, int x);
	void setsyx(int y, int x);
	int ripoffline(int line, int (*init)(WINDOW *, int));
	int curs_set(int visibility);
	int napms(int ms);

	int draino(int ms);
	int resetterm(void);
	int fixterm(void);
	int saveterm(void);

  Description:
	def_prog_mode() and def_shell_mode() save the current terminal
	modes as the "program" (in curses) or "shell" (not in curses)
	state for use by the reset_prog_mode() and reset_shell_mode()
	functions.  This is done automatically by initscr().

	reset_prog_mode() and reset_shell_mode() restore the terminal to
	"program" (in curses) or "shell" (not in curses) state.  These
	are done automatically by endwin() and doupdate() after an
	endwin(), so they would normally not be called before these
	functions.

	savetty() and resetty() save and restore the state of the
	terminal modes. savetty() saves the current state in a buffer,
	and resetty() restores the state to what it was at the last call
	to savetty().

	getsyx() obtains the coordinates of the virtual screen cursor.
	If leaveok() is currently TRUE, then -1, -1 is returned. If
	lines have been removed from the top of the screen with
	ripoffline(), then getsyx() includes those lines, so y and x
	should only be used by setyx(). setyx() sets the cursor position
	of the virtual screen to the y,x coordinates. If y, x are -1,
	-1, leaveok() is set TRUE. The getsyx() and setsyx() routines
	are designed to be used by a library routine that manipulates
	curses windows, but does not want to change the position of the
	cursor.

	Note that getsyx() and setsyx() are defined as macros only.

	curs_set() alters the appearance of the text cursor. A value of
	0 for visibility makes the cursor disappear; a value of 1 makes
	the cursor appear "normal" (usually an underline) and 2 makes
	the cursor "highly visible" (usually a block).

	ripoffline() allows the user to reduce the size of stdscr by 1
	line.  If the value of line is positive, the line is removed
	from the top of the screen; negative from the bottom. Up to 5
	lines can be ripped off stdscr by calling ripoffline()
	consecutively. The function argument, init, is called from
	within initscr() or newterm(), so ripoffline() must be called
	before either of these functions.  The init function is passed a
	pointer to a 1 line WINDOW, and the width of the window. Calling
	ripoffline() with a NULL initialise function pointer is not
	advised.

	The napms() function suspends the program for the specified
	number of milliseconds. draino() is an archaic equivalent.

	FYI: It is unclear whether savetty() and resetty() are meant to
	duplicate reset_prog_mode() and reset_shell_mode(), or be a
	backing store type of operation. At this time, they're
	implemented similarly to the reset_*_mode() routines.

	resetterm(), fixterm() and saveterm() are archaic equivalents
	for reset_shell_mode(), reset_prog_mode() and def_prog_mode(),
	respectively.

  Return Value:
	All functions return OK on success and ERR on error, except
	curs_set(), which returns the previous visibility.

  Portability				     X/Open    BSD    SYS V
	def_prog_mode				Y	Y	Y
	def_shell_mode				Y	Y	Y
	reset_prog_mode				Y	Y	Y
	reset_shell_mode			Y	Y	Y
	resetty					Y	Y	Y
	savetty					Y	Y	Y
	getsyx					-	-      3.0
	setsyx					-	-      3.0
	ripoffline				Y	-      3.0
	curs_set				Y	-      3.0
	napms					Y	Y	Y
	draino					-
	resetterm				-
	fixterm					-
	saveterm				-

**man-end****************************************************************/

static void _save_mode(int i)
{
	ctty[i].been_set = TRUE;

	memcpy(&(ctty[i].saved), SP, sizeof(SCREEN));

	PDC_save_screen_mode(i);
}

static int _restore_mode(int i)
{
	if (ctty[i].been_set == TRUE)
	{
		memcpy(SP, &(ctty[i].saved), sizeof(SCREEN));

		if (ctty[i].saved.raw_out)
			raw();

		PDC_restore_screen_mode(i);

		if ((LINES != ctty[i].saved.lines) ||
		    (COLS != ctty[i].saved.cols))
			resize_term(ctty[i].saved.lines, ctty[i].saved.cols);

		PDC_curs_set(ctty[i].saved.visibility);

		PDC_gotoyx(ctty[i].saved.cursrow, ctty[i].saved.curscol);
	}

	return ctty[i].been_set ? OK : ERR;
}

int def_prog_mode(void)
{
	PDC_LOG(("def_prog_mode() - called\n"));

	_save_mode(PDC_PR_TTY);

	return OK;
}

int def_shell_mode(void)
{
	PDC_LOG(("def_shell_mode() - called\n"));

	_save_mode(PDC_SH_TTY);

	return OK;
}

int reset_prog_mode(void)
{
	PDC_LOG(("reset_prog_mode() - called\n"));

	_restore_mode(PDC_PR_TTY);
	PDC_reset_prog_mode();

	return OK;
}

int reset_shell_mode(void)
{
	PDC_LOG(("reset_shell_mode() - called\n"));

	_restore_mode(PDC_SH_TTY);
	PDC_reset_shell_mode();

	return OK;
}

int resetty(void)
{
	PDC_LOG(("resetty() - called\n"));

	return _restore_mode(PDC_SAVE_TTY);
}

int savetty(void)
{
	PDC_LOG(("savetty() - called\n"));

	_save_mode(PDC_SAVE_TTY);

	return OK;
}

int curs_set(int visibility)
{
	int ret_vis;

	PDC_LOG(("curs_set() - called: visibility=%d\n", visibility));

	if ((visibility < 0) || (visibility > 2))
		return ERR;

	ret_vis = PDC_curs_set(visibility);

	/* If the cursor is changing from invisible to visible, update 
	   its position */

	if (visibility && !ret_vis)
		PDC_gotoyx(SP->cursrow, SP->curscol);

	return ret_vis;
}

int napms(int ms)
{
	PDC_LOG(("napms() - called: ms=%d\n", ms));

	if (ms)
		PDC_napms(ms);

	return OK;
}

int ripoffline(int line, int (*init)(WINDOW *, int))
{
	PDC_LOG(("ripoffline() - called: line=%d\n", line));

	if (linesrippedoff < 5 && line)
	{
		linesripped[(int)linesrippedoff].line = line;
		linesripped[(int)linesrippedoff++].init = init;
	}

	return OK;
}

int draino(int ms)
{
	PDC_LOG(("draino() - called\n"));

	return napms(ms);
}

int resetterm(void)
{
	PDC_LOG(("resetterm() - called\n"));

	return reset_shell_mode();
}

int fixterm(void)
{
	PDC_LOG(("fixterm() - called\n"));

	return reset_prog_mode();
}

int saveterm(void)
{
	PDC_LOG(("saveterm() - called\n"));

	return def_prog_mode();
}
