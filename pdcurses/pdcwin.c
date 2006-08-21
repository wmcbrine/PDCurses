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

RCSID("$Id: pdcwin.c,v 1.52 2006/08/21 17:49:27 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_makenew()   - Create a WINDOW* (sans line allocation)

  PDCurses Description:
	This is a private PDCurses routine.

	Allocates all data for a new WINDOW* except the actual lines
	themselves.

  PDCurses Return Value:
	This function returns a valid WINDOW* on success and NULL on 
	error.

  PDCurses Errors:
	If PDC_makenew() is unable to allocate memory for the window
	structure, it will free all allocated memory and return
	a NULL pointer.

  Portability:
	PDCurses  WINDOW *PDC_makenew(int num_lines, int num_columns,
				       int begy, int begx);

**man-end****************************************************************/

WINDOW *PDC_makenew(int num_lines, int num_columns, int begy, int begx)
{
	int i;
	WINDOW *win;

	PDC_LOG(("PDC_makenew() - called: lines %d cols %d begy %d begx %d\n",
		num_lines, num_columns, begy, begx));

	/* allocate the window structure itself */

	if ((win = calloc(1, sizeof(WINDOW))) == (WINDOW *)NULL)
		return win;

	/* allocate the line pointer array */

	if ((win->_y = calloc(num_lines, sizeof(chtype *))) == NULL)
	{
		free(win);
		return (WINDOW *)NULL;
	}

	/* allocate the minchng and maxchng arrays */

	if ((win->_firstch = calloc(num_lines, sizeof(int))) == NULL)
	{
		free(win->_y);
		free(win);
		return (WINDOW *)NULL;
	}

	if ((win->_lastch = calloc(num_lines, sizeof(int))) == NULL)
	{
		free(win->_firstch);
		free(win->_y);
		free(win);
		return (WINDOW *)NULL;
	}

	/* initialize window variables */

	win->_maxy = num_lines;		/* real max screen size */
	win->_maxx = num_columns;	/* real max screen size */
	win->_pmaxy = num_lines;	/* real max window size */
	win->_pmaxx = num_columns;	/* real max window size */
	win->_begy = begy;
	win->_begx = begx;
	win->_lastsy2 = LINES - 1;
	win->_lastsx2 = COLS - 1;
	win->_bkgd = ' '; /* wrs 4/10/93 -- initialize background to blank */
	win->_clear = (bool) ((num_lines == LINES) && (num_columns == COLS));
	win->_bmarg = num_lines - 1;
	win->_parx = win->_pary = -1;

	/* init to say window all changed */

	for (i = 0; i < num_lines; i++)
	{
		win->_firstch[i] = 0;
		win->_lastch[i] = num_columns - 1;
	}

	return win;
}

/*man-start**************************************************************

  PDC_sync()   - Cause window synchronisation to occur

  PDCurses Description:
	This is a private PDCurses routine, NOT to be called by an 
	application.

	Handles wrefresh() and wsyncup() calls when a window is changed.

  PDCurses Return Value:
	This function returns void.

  PDCurses Errors:

  Portability:
	PDCurses  WINDOW *PDC_sync(WINDOW *win);

**man-end****************************************************************/

void PDC_sync(WINDOW *win)
{
	PDC_LOG(("PDC_sync() - called:\n"));

	if (win->_immed)
		wrefresh(win);
	if (win->_sync)
		wsyncup(win);
}

/* _newline() - Does line advance and returns the new cursor line.  
   If error, return -1. */

static int _newline(WINDOW *win, int lin)
{
	if (win == (WINDOW *)NULL)
		return -1;

	if (++lin > win->_bmarg)
	{
		lin--;

		if (win->_scroll)
			scroll(win);
		else
			return -1;
	}

	return lin;
}

/*man-start**************************************************************

  PDC_chadd()      - Low level; Put a character to a window

  PDCurses Description:
	This is a private PDCurses function.

	This routine will insert the character 'c' at the current cursor
	position in the passed window.

	If 'xlat' is TRUE, PDC_chadd() will handle things in a cooked
	manner (tabs, newlines, carriage returns, etc).  If 'xlat' is
	FALSE, the characters are simply output directly.

	If 'advance' is TRUE, PDC_chadd() will move the current cusor 
	position appropriately. The *addch functions call PDC_chadd() 
	with advance TRUE, while the *insch functions call PDC_chadd() 
	with advance FALSE.

	The normal curses routines (non-raw-output-mode) call 
	PDC_chadd() with 'xlat' TRUE.

	If an alternate character is to be displayed, the character is 
	displayed without translation (minus the A_ALTCHARSET of 
	course).

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	It is an error to call this function with a NULL window pointer.

  Portability:
	PDCurses  int PDC_chadd(WINDOW *win, chtype ch,
				bool xlat, bool advance);

**man-end****************************************************************/

int PDC_chadd(WINDOW *win, chtype ch, bool xlat, bool advance)
{
	int x, y, x2, retval;
	chtype attr, bktmp;

	PDC_LOG(("PDC_chadd() - called: win=%x ch=%x "
		"(char=%c attr=0x%x) xlat=%d advance=%d\n", win, ch,
		ch & A_CHARTEXT, ch & A_ATTRIBUTES, xlat, advance));

	if (win == (WINDOW *)NULL)
		return ERR;

	x  = win->_curx;
	y  = win->_cury;

	if ((y > win->_maxy) || (x > win->_maxx) || (y < 0) || (x < 0))
		return ERR;

	/* Remove any A_ALTCHARSET attribute from the ch before any
	   further testing. If the character has A_ALTCHARSET, set xlat
	   to FALSE. */

	if (ch & A_ALTCHARSET)
	{
		xlat = FALSE;
		ch = ch & (~A_ALTCHARSET);
	}

	/* If the incoming character doesn't have its own attribute, 
	   then use the current attributes for the window. If it has 
	   attributes but not a color component, OR the attributes to 
	   the current attributes for the window. If it has a color 
	   component, use the attributes solely from the incoming 
	   character. */

	if ((ch & A_ATTRIBUTES) == 0)
		attr = win->_attrs;
	else
		if ((ch & A_COLOR) == 0)
			attr = (ch & A_ATTRIBUTES) | win->_attrs;
		else
			attr = ch & A_ATTRIBUTES;

	ch = (ch & A_CHARTEXT);

	/* wrs (4/10/93): Apply the same sort of logic for the window 
	   background, in that it only takes precedence if other color 
	   attributes are not there and that the background character 
	   will only print if the printing character is blank. */

	if ((attr & A_COLOR) == 0)
		attr |= win->_bkgd & A_ATTRIBUTES;
	else
	{
		bktmp = win->_bkgd & A_COLOR;
		attr |= (win->_bkgd & A_ATTRIBUTES) ^ bktmp;
	}

	if (ch == ' ')
		ch = win->_bkgd & A_CHARTEXT;

	if (xlat)
	{
		switch (ch)
		{
		case '\t':
			for (x2 = ((x / TABSIZE) + 1) * TABSIZE; x < x2; x++)
			{
				if (waddch(win, ' ') == ERR)
				{
					PDC_sync(win);
					return ERR;
				}

				/* if tab to next line, exit the loop */

				if (win->_curx == 0)
					break;
			}
			PDC_sync(win);
			return OK;

		case '\n':
			/* if lf -> crlf */

			if (!SP->raw_out)
				x = 0;

			wclrtoeol(win);

			if ((y = _newline(win, y)) < 0)
			{
				PDC_sync(win);
				return ERR;
			}

			if (advance)
			{
				win->_cury = y;
				win->_curx = x;
			}

			PDC_sync(win);
			return OK;

		case '\r':
			if (advance)
				win->_curx = x = 0;

			PDC_sync(win);
			return OK;

		case '\b':
			/* don't back over left margin */

			if (--x < 0)
				x = 0;

			if (advance)
				win->_curx = x;

			PDC_sync(win);
			return OK;

		case 0x7f:
			if (waddch(win, '^') == ERR)
			{
				PDC_sync(win);
				return ERR;
			}
			retval = waddch(win, '?');
			PDC_sync(win);
			return retval;
		}

		if (ch < ' ')
		{
			/* handle control chars */

			if (waddch(win, '^') == ERR)
         		{
				PDC_sync(win);
				return ERR;
			}
			retval = (waddch(win, ch + '@'));
			PDC_sync(win);
			return retval;
		}
	}

	/* Add the attribute back into the character. */

	ch |= attr;

	/* Only change _firstch/_lastch if the character to be added is 
	   different from the character/attribute that is already in 
	   that position in the window. */

	if (win->_y[y][x] != ch)
	{
		if (win->_firstch[y] == _NO_CHANGE)
			win->_firstch[y] = win->_lastch[y] = x;
		else
			if (x < win->_firstch[y])
				win->_firstch[y] = x;
			else
				if (x > win->_lastch[y])
					win->_lastch[y] = x;
	}

	win->_y[y][x++] = ch;

	if (x >= win->_maxx)
	{
		/* wrap around test */

		x = 0;

		if ((y = _newline(win, y)) < 0)
		{
			PDC_sync(win);
			return ERR;
		}
	}

	if (advance)
	{
		win->_curx = x;
		win->_cury = y;
	}

	PDC_sync(win);

	return OK;
}
