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

/* undefine any macros for functions defined in this module */
#undef getch
#undef wgetch
#undef mvgetch
#undef mvwgetch
#undef ungetch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef wrefresh
# undef nocbreak
# undef move
# undef wmove
#endif

RCSID("$Id: getch.c,v 1.32 2006/08/01 03:16:17 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                         getch

  Synopsis:
	int getch(void);
	int wgetch(WINDOW *win);
	int mvgetch(int y, int x);
	int mvwgetch(WINDOW *win, int y, int x);
	int ungetch(int ch);

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

	NOTE: getch(), mvgetch() and mvwgetch() are implemented as macros.

  PDCurses Description:
	Given the nature of the PC, there is no such timer set for an
	incoming ESCAPE value, because function keys generate unique
	scan codes that are not prefixed with the ESCAPE character.

	Also, note that the getch() definition will conflict  with
	many DOS compiler's runtime libraries.

  X/Open Return Value:
	These functions return ERR or the value of the character, meta 
	character or function key token.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	getch					Y	Y	Y
	wgetch					Y	Y	Y
	mvgetch					Y	Y	Y
	mvwgetch				Y	Y	Y
	ungetch					Y	Y	Y

**man-end****************************************************************/

/* these defines to get around DOS library conflicts */

#define getch PDC_getch
#define ungetch PDC_ungetch

WINDOW *_getch_win_ = NULL;

/*man-start**************************************************************

  PDC_breakout()	- check for type-ahead

  PDCurses Description:
	Check if input is pending, either directly from the keyboard,
	or previously buffered.

  PDCurses Return Value:
	The PDC_breakout() routine returns TRUE if keyboard input is 
	pending otherwise FALSE is returned.

  Portability:
	PDCurses  bool PDC_breakout(void);

**man-end****************************************************************/

static bool PDC_breakout(void)
{
	bool rc;

	PDC_LOG(("PDC_breakout() - called\n"));

	/* ungotten or buffered char */
	rc = (c_ungind) || (c_pindex > c_gindex);

	if (!rc)
		rc = PDC_check_bios_key();

	PDC_LOG(("PDC_breakout() - rc %d c_ungind %d c_pindex %d c_gindex %d\n",
		 rc, c_ungind, c_pindex, c_gindex));

	return rc;
}

/*man-start**************************************************************

  PDC_rawgetch()  - Returns the next uninterpreted character (if available).

  PDCurses Description:
	Gets a character without any interpretation at all and returns 
	it. If keypad mode is active for the designated window, function 
	key translation will be performed.  Otherwise, function keys are 
	ignored.  If nodelay mode is active in the window, then 
	PDC_rawgetch() returns -1 if no character is available.

	WARNING:  It is unknown whether the FUNCTION key translation
	is performed at this level. --Frotz 911130 BUG

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_rawgetch(void);

**man-end****************************************************************/

static int PDC_rawgetch(void)
{
	int c, oldc;

	PDC_LOG(("PDC_rawgetch() - called\n"));

	if (_getch_win_ == (WINDOW *)NULL)
		return -1;

	if ((SP->delaytenths || _getch_win_->_delayms || _getch_win_->_nodelay)
	    && !PDC_breakout())
		return -1;

	for (;;)
	{
		c = PDC_get_bios_key();
		oldc = c;

		/* return the key if it is not a special key */

		if (c != KEY_MOUSE && (c = PDC_validchar(c)) >= 0)
			return c;

		if (_getch_win_->_use_keypad)
			return oldc;
	}
}

/*man-start**************************************************************

  PDC_sysgetch()  - Return a character using default system routines.

  PDCurses Description:
	This is a private PDCurses function.

	Gets a character without normal ^S, ^Q, ^P and ^C interpretation
	and returns it.  If keypad mode is active for the designated
	window, function key translation will be performed. Otherwise,
	function keys are ignored. If nodelay mode is active in the
	window, then sysgetch() returns -1 if no character is
	available.

  PDCurses Return Value:
	This function returns OK upon success otherwise ERR is returned.

  Portability:
	PDCurses  int PDC_sysgetch(void);

**man-end****************************************************************/

static int PDC_sysgetch(void)
{
	int c;

	PDC_LOG(("PDC_sysgetch() - called\n"));

	if (_getch_win_ == (WINDOW *)NULL)
		return -1;

	if ((SP->delaytenths || _getch_win_->_delayms || _getch_win_->_nodelay)
	    && !PDC_breakout())
		return -1;

	for (;;)
	{
		c = PDC_get_bios_key();

		/* return the key if it is not a special key */

		if ((unsigned int)c < 256)
			return c;

		if ((c = PDC_validchar(c)) >= 0)
			return c;
	}
}

/*man-start**************************************************************

  PDC_get_key_modifiers()  - Returns the keyboard modifier(s) at time
			     of last getch()

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the keyboard modifiers effective at the time of the last 
	getch() call only if PDC_save_key_modifiers(TRUE) has been 
	called before the getch(). Use the macros; PDC_KEY_MODIFIER_* to 
	determine which modifier(s) were set.

  PDCurses Return Value:
	This function returns the modifiers.

  Portability:
	PDCurses  int PDC_get_key_modifiers(void);

**man-end****************************************************************/

unsigned long PDC_get_key_modifiers(void)
{
	PDC_LOG(("PDC_get_key_modifiers() - called\n"));

	return pdc_key_modifiers;
}

int PDC_getch(void)
{
	PDC_LOG(("getch() - called\n"));

	return wgetch(stdscr);
}

int wgetch(WINDOW *win)
{
	static int buffer[_INBUFSIZ];	/* character buffer */
	int key, display_key, waitcount;

	PDC_LOG(("wgetch() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	display_key = 0x100;
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
			if (waitcount == 0)
				waitcount = 1;
		}

	PDC_LOG(("initial: %d delaytenths %d delayms %d\n",
		waitcount, SP->delaytenths, win->_delayms));

	/* wrs (7/31/93) -- System V curses refreshes window when wgetch 
	   is called if there have been changes to it and it is not a pad */

	if (!(win->_flags & _PAD) && is_wintouched(win))
		wrefresh(win);

	_getch_win_ = win;

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
		if (SP->raw_inp)
			key = PDC_rawgetch();	/* get a raw character */
		else
		{
			/* get a system character if break return proper */

			bool cbr = PDC_get_ctrl_break();
			PDC_set_ctrl_break(SP->orgcbr);
			key = PDC_sysgetch();
			PDC_set_ctrl_break(cbr);
		}

		/* Handle timeout() and halfdelay() */

		if (SP->delaytenths || win->_delayms)
		{
			PDC_LOG(("waitcount: %d delaytenths %d delayms %d\n",
			    waitcount, SP->delaytenths, win->_delayms));

			if (waitcount == 0 && key == -1)
				return ERR;

			if (key == -1)
			{
				waitcount--;
				napms(50);	/* sleep for 1/20th second */
				continue;
			}
		}
		else
			if ((win->_nodelay) && (key == -1))
				return ERR;

		/* translate CR */

		if ((key == '\r') && (SP->autocr) && (!SP->raw_inp))
			key = '\n';

		/* if echo is enabled */

		if (SP->echo && (key < display_key))
		{
			waddch(win, key);
			wrefresh(win);
		}

		/* if no buffering */

		if ((SP->raw_inp || SP->cbreak))
			return key;

		/* if no overflow, put data in buffer */

		if (c_pindex < _INBUFSIZ - 2)
			buffer[c_pindex++] = key;

		/* if we got a line */

		if ((key == '\n') || (key == '\r'))
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
