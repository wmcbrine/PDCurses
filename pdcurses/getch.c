/**************************************************************************
* This file comprises part of PDCurses. PDCurses is Public Domain software.
* You may use this code for whatever purposes you desire. This software
* is provided AS IS with NO WARRANTY whatsoever.
* Should this software be used in another application, an acknowledgement
* that PDCurses code is used would be appreciated, but is not mandatory.
*
* Any changes which you make to this software which may improve or enhance
* it, should be forwarded to the current maintainer for the benefit of 
* other users.
*
* The only restriction placed on this code is that no distribution of
* modified PDCurses code be made under the PDCurses name, by anyone
* other than the current maintainer.
* 
* See the file maintain.er for details of the current maintainer.
**************************************************************************/

#define	CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef	getch
#undef	wgetch
#undef	mvgetch
#undef	mvwgetch
#undef	ungetch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	wrefresh
#  undef	nocbreak
#  undef	move
#  undef	wmove
#endif

#ifdef PDCDEBUG
char *rcsid_getch  = "$Id: getch.c,v 1.17 2006/01/28 15:01:41 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

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
	This functions return ERR or the value of the character, meta 
	character or function key token.

  X/Open Errors:
	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      getch                                 Y        Y       Y
      wgetch                                Y        Y       Y
      mvgetch                               Y        Y       Y
      mvwgetch                              Y        Y       Y
      ungetch                               Y        Y       Y

**man-end**********************************************************************/

/* these defines to get around DOS library conflicts */

#define getch PDC_getch
#define ungetch PDC_ungetch

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	PDC_getch(void)
#else
int	PDC_CDECL	PDC_getch()
#endif
/***********************************************************************/
{
	PDC_LOG(("getch() - called\n"));

	return wgetch(stdscr);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wgetch(WINDOW *win)
#else
int	PDC_CDECL	wgetch(win)
WINDOW *win;
#endif
/***********************************************************************/
{
	extern int c_pindex;		/* putter index	*/
	extern int c_gindex;		/* getter index	*/
	extern int c_ungind;		/* ungetch() push index */
	extern int c_ungch[NUNGETCH];	/* array of ungotten chars */
	extern WINDOW *_getch_win_;

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

#ifdef XCURSES
		key = PDC_rawgetch();
		if (!(_getch_win_->_use_keypad) && (unsigned int)key > 255)
			key = -1;
#else
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
#endif
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

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvgetch(int y, int x)
#else
int	PDC_CDECL	mvgetch(y, x)
int y;
int x;
#endif
/***********************************************************************/
{
	PDC_LOG(("mvgetch() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wgetch(stdscr);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwgetch(WINDOW *win, int y, int x)
#else
int	PDC_CDECL	mvwgetch(win, y, x)
WINDOW *win;
int y;
int x;
#endif
/***********************************************************************/
{
	PDC_LOG(("mvwgetch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wgetch(win);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	PDC_ungetch(int ch)
#else
int	PDC_CDECL	PDC_ungetch(ch)
int ch;
#endif
/***********************************************************************/
{
	extern int c_ungind;		/* ungetch() push index */
	extern int c_ungch[NUNGETCH];	/* array of ungotten chars */

	PDC_LOG(("ungetch() - called\n"));

	if (c_ungind >= NUNGETCH)	/* pushback stack full */
		return ERR;

	c_ungch[c_ungind++] = ch;

	return OK;
}
