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

#define _INBUFSIZ	512	/* size of terminal input buffer */
#define NUNGETCH	256	/* max # chars to ungetch() */

RCSID("$Id: getch.c,v 1.53 2006/11/12 20:38:00 wmcbrine Exp $");

static int c_pindex = 0;	/* putter index */
static int c_gindex = 1;	/* getter index */
static int c_ungind = 0;	/* wungetch() push index */
static int c_ungch[NUNGETCH];	/* array of ungotten chars */

/*man-start**************************************************************

  Name:                                                         getch

  Synopsis:
	int getch(void);
	int wgetch(WINDOW *win);
	int mvgetch(int y, int x);
	int mvwgetch(WINDOW *win, int y, int x);
	int ungetch(int ch);
	int flushinp(void);

	int get_wch(wint_t *wch);
	int wget_wch(WINDOW *win, wint_t *wch);
	int mvget_wch(int y, int x, wint_t *wch);
	int mvwget_wch(WINDOW *win, int y, int x, wint_t *wch);
	int unget_wch(const wchar_t wch);

	int PDC_get_key_modifiers(void);

  X/Open Description:
	With the getch(), wgetch(), mvgetch(), and mvwgetch() functions, 
	a character is read from the terminal associated with the window. 
	In nodelay mode, if there is no input waiting, the value ERR is 
	returned. In delay mode, the program will hang until the system 
	passes text through to the program. Depending on the setting of 
	cbreak(), this will be after one character or after the first 
	newline.  Unless noecho() has been set, the character will also 
	be echoed into the designated window.

	If keypad() is TRUE, and a function key is pressed, the token for
	that function key will be returned instead of the raw characters.
	Possible function keys are defined in <curses.h> with integers
	beginning with 0401, whose names begin with KEY_.  If a character
	is received that could be the beginning of a function key (such as
	escape), curses will set a timer.  If the remainder of the sequence
	does not come in within the designated time, the character will be
	passed through, otherwise the function key value will be returned.
	For this reason, on many terminals, there will be a delay after a
	user presses the escape key before the escape is returned to the
	program.  (Use by a programmer of the escape key for a single
	character function is discouraged.)

	If nodelay(win,TRUE) has been called on the window and no input
	is waiting, the value ERR is returned.

	The ungetch() function places ch back onto the input queue to be
	returned by the next call to wgetch().

	The flushinp() routine throws away any type-ahead that has been
	typed by the user and has not yet been read by the program.

	NOTE: getch() and ungetch() are implemented as macros.

  PDCurses Description:
	Given the nature of the PC, there is no such timer set for an
	incoming ESCAPE value, because function keys generate unique
	scan codes that are not prefixed with the ESCAPE character.

	Also, note that the getch() definition will conflict with
	many DOS compiler's runtime libraries.

	PDC_get_key_modifiers() returns the keyboard modifiers effective 
	at the time of the last getch() call, only if 
	PDC_save_key_modifiers(TRUE) has been called before the getch(). 
	Use the macros; PDC_KEY_MODIFIER_* to determine which 
	modifier(s) were set.

  X/Open Return Value:
	These functions return ERR or the value of the character, meta 
	character or function key token.

  Portability				     X/Open    BSD    SYS V
	getch					Y	Y	Y
	wgetch					Y	Y	Y
	mvgetch					Y	Y	Y
	mvwgetch				Y	Y	Y
	ungetch					Y	Y	Y
	flushinp				Y	Y	Y
	get_wch					Y
	wget_wch				Y
	mvget_wch				Y
	mvwget_wch				Y
	unget_wch				Y
	PDC_get_key_modifiers			-	-	-

**man-end****************************************************************/

int wgetch(WINDOW *win)
{
	static int buffer[_INBUFSIZ];	/* character buffer */
	int key, waitcount;

	PDC_LOG(("wgetch() - called\n"));

	if (!win)
		return ERR;

	waitcount = 0;

	 /* set the number of 1/20th second napms() calls */

	if (SP->delaytenths)
		waitcount = 2 * SP->delaytenths;
	else
		if (win->_delayms)
		{
			/* Can't really do millisecond intervals, so 
			   delay in 1/20ths of a second (50ms) */

			waitcount = win->_delayms / 50;
			if (!waitcount)
				waitcount = 1;
		}

	/* wrs (7/31/93) -- System V curses refreshes window when wgetch 
	   is called if there have been changes to it and it is not a pad */

	if (!(win->_flags & _PAD) && is_wintouched(win))
		wrefresh(win);

	/* if ungotten char exists, remove and return it */

	if (c_ungind)
		return c_ungch[--c_ungind];

	/* if normal and data in buffer */

	if ((!SP->raw_inp && !SP->cbreak) && (c_gindex < c_pindex))
		return buffer[c_gindex++];

	/* prepare to buffer data */

	c_pindex = 0;
	c_gindex = 0;

	/* to get here, no keys are buffered. go and get one. */

	for (;;)			/* loop for any buffering */
	{
		if (!PDC_check_bios_key())
		{
			key = -1;
		}
		else
		{
			key = PDC_get_bios_key();

			/* Filter special keys if not in keypad mode */

			if (SP->key_code && !win->_use_keypad)
				key = -1;
		}

		/* translate mouse clicks in the slk area to function 
		   keys */

		if (key == KEY_MOUSE)
		{
			int fn = PDC_mouse_in_slk(pdc_mouse_status.y, 
				pdc_mouse_status.x);

			if (fn)
			{
				if (pdc_mouse_status.button[0] &
				    BUTTON_PRESSED)
					key = KEY_F(fn);
				else
					key = -1;
			}
		}

		/* handle timeout() and halfdelay() */

		if (key == -1)
		{
			if (SP->delaytenths || win->_delayms)
			{
				if (!waitcount)
					return ERR;

				waitcount--;
			}
			else
				if (win->_nodelay)
					return ERR;

			napms(50);	/* sleep for 1/20th second */
			continue;
		}

		/* translate CR */

		if (key == '\r' && SP->autocr && !SP->raw_inp)
			key = '\n';

		/* if echo is enabled */

		if (SP->echo && !SP->key_code)
		{
			waddch(win, key);
			wrefresh(win);
		}

		/* if no buffering */

		if (SP->raw_inp || SP->cbreak)
			return key;

		/* if no overflow, put data in buffer */

		if (key == '\b')
		{
			if (c_pindex > c_gindex)
				c_pindex--;
		}
		else
			if (c_pindex < _INBUFSIZ - 2)
				buffer[c_pindex++] = key;

		/* if we got a line */

		if (key == '\n' || key == '\r')
			return buffer[c_gindex++];
	}
}

int mvgetch(int y, int x)
{
	PDC_LOG(("mvgetch() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wgetch(stdscr);
}

int mvwgetch(WINDOW *win, int y, int x)
{
	PDC_LOG(("mvwgetch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wgetch(win);
}

int PDC_ungetch(int ch)
{
	PDC_LOG(("ungetch() - called\n"));

	if (c_ungind >= NUNGETCH)	/* pushback stack full */
		return ERR;

	c_ungch[c_ungind++] = ch;

	return OK;
}

int flushinp(void)
{
	PDC_LOG(("flushinp() - called\n"));

	PDC_flushinp();

	c_gindex = 1;			/* set indices to kill buffer */
	c_pindex = 0;
	c_ungind = 0;			/* clear c_ungch array */

	return OK;
}

unsigned long PDC_get_key_modifiers(void)
{
	PDC_LOG(("PDC_get_key_modifiers() - called\n"));

	return pdc_key_modifiers;
}

#ifdef PDC_WIDE
int wget_wch(WINDOW *win, wint_t *wch)
{
	int key;

	PDC_LOG(("wget_wch() - called\n"));

	if (!wch)
		return ERR;

	key = wgetch(win);

	if (key == ERR)
		return ERR;

	*wch = key;

	return SP->key_code ? KEY_CODE_YES : OK;
}

int get_wch(wint_t *wch)
{
	PDC_LOG(("get_wch() - called\n"));

	return wget_wch(stdscr, wch);
}

int mvget_wch(int y, int x, wint_t *wch)
{
	PDC_LOG(("mvget_wch() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wget_wch(stdscr, wch);
}

int mvwget_wch(WINDOW *win, int y, int x, wint_t *wch)
{
	PDC_LOG(("mvwget_wch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wget_wch(win, wch);
}

int unget_wch(const wchar_t wch)
{
	return PDC_ungetch(wch);
}
#endif
