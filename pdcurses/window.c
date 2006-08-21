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
#include <stdlib.h>

/* undefine any macros for functions defined in this module */
#undef derwin

RCSID("$Id: window.c,v 1.38 2006/08/21 18:38:19 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                        window

  Synopsis:
	WINDOW *newwin(int nlines, int ncols, int begy, int begx);
	int delwin(WINDOW *win);
	int mvwin(WINDOW *win, int y, int x);
	WINDOW *subwin(WINDOW* orig,int nlines,int ncols,
			int begin_y,int begin_x);
  	WINDOW *derwin(WINDOW* orig,int nlines,int ncols,
			int begin_y,int begin_x);
	int mvderwin(WINDOW *win, int par_y, int par_x);
	int dupwin(WINDOW *win);
	void wsyncup(WINDOW *win);
	int syncok(WINDOW *win, bool bf);
	void wcursyncup(WINDOW *win);
	void wsyncdown(WINDOW *win);

	WINDOW *resize_window(WINDOW *w, int lins, int cols);
	WINDOW *PDC_makenew(int num_lines, int num_columns,
			int begy, int begx);
	void PDC_sync(WINDOW *win);

  X/Open Description:
	newwin() creates a new window with the given number of lines, 
	nlines and columns, ncols. The upper left corner of the window 
	is at line begy, column begx. If either nlines or ncols is zero, 
	they will be defaulted to LINES - begy and COLS - begx. A new 
	full-screen window is created by calling newwin(0, 0, 0, 0).

	delwin() deletes the named window, freeing all memory associated 
	with it. In the case of overlapping windows, subwindows should 
	be deleted before the main window.

	mvwin() moves the window so that the upper left-hand corner is 
	at position (y,x). If the move would cause the window to be off 
	the screen, it is an error and the window is not moved. Moving 
	subwindows is allowed, but should be avoided. (I don't know why?)

	subwin() creates a new sub-window within a window.  The 
	dimensions of the sub-window are nlines lines and ncols columns.  
	The sub-window is at position (begin_y, begin_x) on the screen.  
	This position is relative to the screen, and not to the window 
	orig. The sub-window is made in the middle of the window orig, 
	so that changes made to either window will affect both.  When 
	using this routine, it will often be necessary to call 
	touchwin() before calling wrefresh().

	derwin() is the same as subwin(), except that begin_y and 
	begin_x are relative to the origin of the window orig rather 
	than the screen.  There is no difference between subwindows and 
	derived windows.

	mvderwin() moves a derived window (or subwindow) inside its 
	parent window.  The screen-relative parameters of the window are 
	not changed.  This routine is used to display different parts of 
	the parent window at the same physical position on the screen.

	dupwin() creates an exact duplicate of the window win.

	wsyncup() causes a touchwin() of all of the window's parents.

	If wsyncok() is called with a second argument of TRUE, this 
	causes a wsyncup() to be called every time the window is 
	changed.

	wcursyncup() causes the current cursor position of all of a 
	window's ancestors to reflect the current cursor position of the 
	current window.

	wsyncdown() causes a touchwin() of the current window if any of
	its parent's windows have been touched.

  PDCurses Description:
	resize_window() allows the usrer to resize an existing window.

	PDC_makenew() allocates all data for a new WINDOW* except the 
	actual lines themselves. If it's unable to allocate memory for 
	the window structure, it will free all allocated memory and 
	return a NULL pointer.

	PDC_sync() handles wrefresh() and wsyncup() calls when a window 
	is changed.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  PDCurses Errors:
	It is an error to call resize_window() before calling initscr().
	Also, an error will be generated if we fail to create a newly
	sized replacement window for curscr, or stdscr.
	This will typically happen when increasing the window size.

	NOTE:  If this happens, the previously successfully allocated
	windows are left alone.  i.e. The resize is NOT cancelled for
	those windows.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	newwin					Y	Y	Y
	delwin					Y	Y	Y
	mvwin					Y	Y	Y
	subwin					Y	Y	Y
	derwin					Y	-	Y
	mvderwin				Y	-	Y
	dupwin					-	-      4.0
	wsyncup					-	-      4.0
	syncok					-	-      4.0
	wcursyncup				-	-      4.0
	wsyncdown				-	-      4.0
	resize_window				-	-	-
	PDC_makenew				-	-	-
	PDC_sync				-	-	-

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

void PDC_sync(WINDOW *win)
{
	PDC_LOG(("PDC_sync() - called:\n"));

	if (win->_immed)
		wrefresh(win);
	if (win->_sync)
		wsyncup(win);
}

WINDOW *newwin(int nlines, int ncols, int begy, int begx)
{
	WINDOW *win;
	chtype *ptr;
	int i, j;

	PDC_LOG(("newwin() - called:lines=%d cols=%d begy=%d begx=%d\n",
		nlines, ncols, begy, begx));

	if (nlines == 0)
		nlines = LINES - begy;
	if (ncols  == 0)
		ncols  = COLS  - begx;

	if (begy + nlines > SP->lines || begx + ncols > SP->cols)
		return (WINDOW *)NULL;

	if ((win = PDC_makenew(nlines, ncols, begy, begx)) == (WINDOW *) NULL)
		return (WINDOW *)NULL;

	/* make and clear the lines */

	for (i = 0; i < nlines; i++)
	{
		if ((win->_y[i] = calloc(ncols, sizeof(chtype))) == NULL)
		{
			/* if error, free all the data */

			for (j = 0; j < i; j++)
				free(win->_y[j]);

			free(win->_firstch);
			free(win->_lastch);
			free(win->_y);
			free(win);

			return (WINDOW *)NULL;
		}
		else	/* retain the original screen attributes */

			for (ptr = win->_y[i];
			     ptr < win->_y[i] + ncols; ptr++)
				*ptr = SP->blank;
	}

	return win;
}

int delwin(WINDOW *win)
{
	int i;

	PDC_LOG(("delwin() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	/* subwindows use parents' lines */

	if (!(win->_flags & (_SUBWIN|_SUBPAD)))
		for (i = 0; i < win->_pmaxy && win->_y[i]; i++)
			if (win->_y[i] != NULL)
				free(win->_y[i]);

	free(win->_firstch);
	free(win->_lastch);
	free(win->_y);
	free(win);

	return OK;
}

int mvwin(WINDOW *win, int y, int x)
{
	PDC_LOG(("mvwin() - called\n"));

	if ((win == (WINDOW *)NULL)
	   || (y + win->_maxy > LINES || y < 0)
	   || (x + win->_maxx > COLS || x < 0))
		return ERR;

	win->_begy = y;
	win->_begx = x;
	touchwin(win);

	return OK;
}

WINDOW *subwin(WINDOW *orig, int nlines, int ncols, int begin_y, int begin_x)
{
	WINDOW *win;
	int i;
	int j = begin_y - orig->_begy;
	int k = begin_x - orig->_begx;

	PDC_LOG(("subwin() - called: lines %d cols %d begy %d begx %d\n",
		nlines, ncols, begin_y, begin_x));

	/* make sure window fits inside the original one */

	if (!orig || (begin_y < orig->_begy) || (begin_x < orig->_begx) ||
	    (begin_y + nlines) > (orig->_begy + orig->_maxy) ||
	    (begin_x + ncols) > (orig->_begx + orig->_maxx))
		return (WINDOW *)NULL;

	if (!nlines)
		nlines = orig->_maxy - 1 - j;
	if (!ncols)
		ncols  = orig->_maxx - 1 - k;

	if ((win = PDC_makenew(nlines, ncols, begin_y, begin_x))
	    == (WINDOW *) NULL)
		return (WINDOW *)NULL;

	/* initialize window variables */

	win->_attrs	= orig->_attrs;
	win->_bkgd	= orig->_bkgd;
	win->_leaveit	= orig->_leaveit;
	win->_scroll	= orig->_scroll;
	win->_nodelay	= orig->_nodelay;
	win->_use_keypad = orig->_use_keypad;
	win->_immed	= orig->_immed;
	win->_sync	= orig->_sync;
	win->_pary	= j;
	win->_parx	= k;
	win->_parent	= orig;

	for (i = 0; i < nlines; i++, j++)
		win->_y[i] = orig->_y[j] + k;

	win->_flags |= _SUBWIN;

	return win;
}

WINDOW *derwin(WINDOW *orig, int nlines, int ncols, int begin_y, int begin_x)
{
	return subwin(orig, nlines, ncols, begin_y + orig->_begy,
		begin_x + orig->_begx);
}

int mvderwin(WINDOW *win, int par_y, int par_x)
{
	int i, j;
	WINDOW *mypar;

	if ((win == (WINDOW *)NULL) || (win->_parent == NULL))
		return ERR;

	mypar = win->_parent;

	if (par_y < 0 || par_x < 0 ||
	    (par_y + win->_maxy) > mypar->_maxy ||
	    (par_x + win->_maxx) > mypar->_maxx)
		return ERR;

	j = par_y;

	for (i = 0; i < win->_maxy; i++)
		win->_y[i] = (mypar->_y[j++]) + par_x;

	win->_pary = par_y;
	win->_parx = par_x;

	return OK;
}

WINDOW *dupwin(WINDOW *win)
{
	WINDOW *new;
	chtype *ptr, *ptr1;
	int nlines, ncols, begy, begx, i, j;

	if (!win)
		return (WINDOW *)NULL;

	nlines	= win->_maxy;
	ncols	= win->_maxx;
	begy	= win->_begy;
	begx	= win->_begx;

	if ((new = PDC_makenew(nlines, ncols, begy, begx)) == (WINDOW *)NULL)
		return (WINDOW *)NULL;

	/* copy the contents of win into new */

	for (i = 0; i < nlines; i++)
	{
		if ((new->_y[i] = calloc(ncols, sizeof(chtype))) == NULL)
		{
			/* if error, free all the data */

			for (j = 0; j < i; j++)
				free(new->_y[j]);

			free(new->_firstch);
			free(new->_lastch);
			free(new->_y);
			free(new);

			return (WINDOW *)NULL;
		}
		else
		{
			for (ptr = new->_y[i], ptr1 = win->_y[i];
			     ptr < new->_y[i] + ncols; ptr++, ptr1++)
				*ptr = *ptr1;

			new->_firstch[i] = 0;
			new->_lastch[i] = ncols - 1;
		}
	}

	new->_curx = win->_curx;
	new->_cury = win->_cury;
	new->_maxy = win->_maxy;
	new->_maxx = win->_maxx;
	new->_pmaxy = win->_pmaxy;
	new->_pmaxx = win->_pmaxx;
	new->_begy = win->_begy;
	new->_begx = win->_begx;
	new->_lastpy = win->_lastpy;
	new->_lastpx = win->_lastpx;
	new->_lastsy1 = win->_lastsy1;
	new->_lastsx1 = win->_lastsx1;
	new->_lastsy2 = win->_lastsy2;
	new->_lastsx2 = win->_lastsx2;
	new->_flags = win->_flags;
	new->_attrs = win->_attrs;
	new->_clear = win->_clear;
	new->_leaveit = win->_leaveit;
	new->_scroll = win->_scroll;
	new->_nodelay = win->_nodelay;
	new->_use_keypad = win->_use_keypad;
	new->_tmarg = win->_tmarg;
	new->_bmarg = win->_bmarg;
	new->_parx = win->_parx;
	new->_pary = win->_pary;
	new->_parent = win->_parent;
	new->_bkgd = win->_bkgd;
	new->_flags = win->_flags;

	return new;

}

WINDOW *resize_window(WINDOW *win, int lins, int cols)
{
	WINDOW *new;
	int i, j, save_cury, save_curx;
	int new_begy, new_begx;

	PDC_LOG(("resize_window() - called: lins %d cols %d\n", lins, cols));

	if (win == (WINDOW *)NULL)
		return (WINDOW *)NULL;

	if (win == SP->slk_winptr)
	{
		new_begy = SP->lines - SP->slklines;
		new_begx = 0;
	}
	else
	{
		new_begy = win->_begy;
		new_begx = win->_begx;
	}

	if ((new = PDC_makenew(lins, cols, new_begy, new_begx))
	    == (WINDOW *)NULL)
		return (WINDOW *)NULL;

	new->_curx = min(win->_curx, new->_maxx);
	new->_cury = min(win->_cury, new->_maxy);
	new->_flags = win->_flags;
	new->_attrs = win->_attrs;
	new->_clear = win->_clear;
	new->_leaveit = win->_leaveit;
	new->_scroll = win->_scroll;
	new->_nodelay = win->_nodelay;
	new->_use_keypad = win->_use_keypad;
	new->_tmarg = (win->_tmarg > new->_maxy - 1) ? 0 : win->_tmarg;
	new->_bmarg = (win->_bmarg == win->_maxy - 1) ?
		new->_maxy - 1 : min(win->_bmarg, (new->_maxy - 1));
	new->_parent = win->_parent;
	new->_immed = win->_immed;
	new->_sync = win->_sync;
	new->_bkgd = win->_bkgd;

	/* make and clear the lines */

	for (i = 0; i < lins; i++)
	{
		if ((new->_y[i] = (chtype *)calloc(cols, sizeof(chtype)))
		    == NULL)
		{
			/* if error, free all the data */

			for (j = 0; j < i; j++)
				free(new->_y[j]);

			free(new->_firstch);
			free(new->_lastch);
			free(new->_y);
			free(new);

			return (WINDOW *)NULL;
		}

		new->_firstch[i] = new->_lastch[i] = _NO_CHANGE;
	}

	if (win != curscr && win != SP->slk_winptr)
	{
		copywin(win, new, 0, 0, 0, 0, min(win->_maxy, new->_maxy),
			min(win->_maxx, new->_maxx), FALSE);

		if (new->_maxy > win->_maxy)
		{
			save_cury = new->_cury;
			save_curx = new->_curx;
			wmove(new, new->_maxy - 1, 0);
			wclrtobot(new);
			new->_cury = save_cury;
			new->_curx = save_curx;
		}
	}

	delwin(win);

	return new;
}

void wsyncup(WINDOW *win)
{
	WINDOW *tmp;

	PDC_LOG(("wsyncup() - called\n"));

	for (tmp = win; tmp; tmp = tmp->_parent)
		touchwin(tmp);
}

int syncok(WINDOW *win, bool bf)
{
	PDC_LOG(("syncok() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_sync = bf;

	return OK;
}

void wcursyncup(WINDOW *win)
{
	WINDOW *tmp;

	PDC_LOG(("wcursyncup() - called\n"));

	for (tmp = win; tmp && tmp->_parent; tmp = tmp->_parent)
		wmove(tmp->_parent, tmp->_pary + tmp->_cury,
			tmp->_parx + tmp->_curx);
}

void wsyncdown(WINDOW *win)
{
	WINDOW *tmp;

	PDC_LOG(("wsyncdown() - called\n"));

	for (tmp = win; tmp; tmp = tmp->_parent)
		if (is_wintouched(tmp))
		{
			touchwin(win);
			break;
		}
}
