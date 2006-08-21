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

RCSID("$Id: pdcwin.c,v 1.53 2006/08/21 18:19:46 wmcbrine Exp $");

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
