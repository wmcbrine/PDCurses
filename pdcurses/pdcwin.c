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

#define CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>
#include <string.h>

#ifdef PDCDEBUG
const char *rcsid_PDCwin =
	"$Id: pdcwin.c,v 1.23 2006/02/06 01:55:00 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  PDC_copy_win() - Common routine for copywin(), overlay() and 
	overwrite() functions.

  PDCurses Description:
	This function copies the region of the source window specified
	over the specified region of the destination window. All 
	validation of limits are done by the calling function.

	Thanks to Andreas Otte (venn@uni-paderborn.de) for the code 
	changes.

  PDCurses Errors:
	ERR is returned if either src or dst windows are NULL;

  Portability:
	PDCurses  int PDC_copy_win(const WINDOW *src_w, WINDOW *dst_w,
			int src_tr, int src_tc, int src_br, int src_bc,
			int dst_tr, int dst_tc, int dst_br, int dst_bc, 
			bool overlay);

**man-end**********************************************************************/

int PDC_copy_win(const WINDOW *src_w, WINDOW *dst_w, int src_tr,
		 int src_tc, int src_br, int src_bc, int dst_tr,
		 int dst_tc, int dst_br, int dst_bc, bool overlay)
{
	int col, line, y1, fc, *minchng, *maxchng;
	chtype *w1ptr, *w2ptr;

	int lc = 0;
	int xdiff = src_bc - src_tc;
	int ydiff = src_br - src_tr;

	PDC_LOG(("PDC_copy_win() - called\n"));

	if ((src_w == (WINDOW *)NULL) || (dst_w == (WINDOW *)NULL))
		return ERR;

	minchng = dst_w->_firstch;
	maxchng = dst_w->_lastch;

	for (y1 = 0; y1 < dst_tr; y1++)
	{
		minchng++;
		maxchng++;
	}

	for (line = 0; line < ydiff; line++)
	{
		w1ptr = src_w->_y[line + src_tr] + src_tc;
		w2ptr = dst_w->_y[line + dst_tr] + dst_tc;

		fc = _NO_CHANGE;

		for (col = 0; col < xdiff; col++)
		{
			if ((*w1ptr) != (*w2ptr) && !((*w1ptr & A_CHARTEXT)
			    == src_w->_blank && overlay))
			{
				*w2ptr = *w1ptr;

				if (fc == _NO_CHANGE)
					fc = col + dst_tc;
            
				lc = col + dst_tc;
			}

			w1ptr++;
			w2ptr++;
		}

		if (*minchng == _NO_CHANGE)
		{
			*minchng = fc;
			*maxchng = lc;
		}
		else if (fc != _NO_CHANGE)
		{
			if (fc < *minchng)
				*minchng = fc;
			if (lc > *maxchng)
				*maxchng = lc;
		}

		minchng++;
		maxchng++;
	}

	return OK;
}

/*man-start*********************************************************************

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
	PDCurses  WINDOW * PDC_makenew(int num_lines, int num_columns,
				       int begy, int begx);

**man-end**********************************************************************/

WINDOW * PDC_makenew(int num_lines, int num_columns, int begy, int begx)
{
	extern void *(*mallc)(size_t);
	extern void *(*callc)(size_t, size_t);
	extern void  (*fre)(void *);

	int i;
	WINDOW *win;

	PDC_LOG(("PDC_makenew() - called: lines %d cols %d begy %d begx %d\n",
		num_lines, num_columns, begy, begx));

	/*  Use the standard runtime malloc/calloc package or use
	    the user's emalloc/ecalloc package. */

	/* allocate the window structure itself */

	if ((win = (*mallc)(sizeof(WINDOW))) == (WINDOW *)NULL)
		return win;

	/* set all fields to zero */

	memset(win, 0, sizeof(WINDOW));

	/* allocate the line pointer array */

	if ((win->_y = (*callc)(num_lines, sizeof(chtype *))) == NULL)
	{
		(*fre)(win);
		return (WINDOW *)NULL;
	}

	/* allocate the minchng and maxchng arrays */

	if ((win->_firstch = (*callc)(num_lines, sizeof(int))) == NULL)
	{
		(*fre)(win->_y);
		(*fre)(win);
		return (WINDOW *)NULL;
	}

	if ((win->_lastch = (*callc)(num_lines, sizeof(int))) == NULL)
	{
		(*fre)(win->_firstch);
		(*fre)(win->_y);
		(*fre)(win);
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
	win->_tabsize = 8;
	win->_clear = (bool) ((num_lines == LINES) && (num_columns == COLS));
	win->_use_idc = TRUE;
	win->_bmarg = num_lines - 1;
	win->_title_ofs = 1;
	win->_title_attr = win->_attrs;
	win->_blank = ' ';
	win->_parx = win->_pary = -1;

	/* init to say window unchanged */

	for (i = 0; i < num_lines; i++)
	{
		win->_firstch[i] = 0;
		win->_lastch[i] = num_columns - 1;
	}

	/* set flags for window properties */

	if ((begy + num_lines) == LINES)
	{
		win->_flags |= _ENDLINE;

		if ((begx == 0) && (num_columns == COLS) && (begy == 0))
			win->_flags |= _FULLWIN;
	}

	if (((begy + num_lines) == LINES) && ((begx + num_columns) == COLS))
		win->_flags |= _SCROLLWIN;

	return win;
}

/*man-start*********************************************************************

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

**man-end**********************************************************************/

void PDC_sync(WINDOW *win)
{
	PDC_LOG(("PDC_sync() - called:\n"));

	if (win->_immed)
		wrefresh(win);
	if (win->_sync)
		wsyncup(win);
}

/*man-start*********************************************************************

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

**man-end**********************************************************************/

int PDC_CDECL PDC_chadd(WINDOW *win, chtype ch, bool xlat, bool advance)
{
	int x, y, newx, ts, retval;
	chtype attr, bktmp;

	PDC_LOG(("PDC_chadd() - called: win=%x ch=%x "
		"(char=%c attr=0x%x) xlat=%d advance=%d\n", win, ch,
		ch & A_CHARTEXT, ch & A_ATTRIBUTES, xlat, advance));

	if (win == (WINDOW *)NULL)
		return ERR;

	x  = win->_curx;
	y  = win->_cury;
	ts = win->_tabsize;

	if ((y > win->_maxy) || (x > win->_maxx) || (y < 0) || (x < 0))
		return ERR;

	/* Remove any A_ALTCHASET attribute from the ch before any
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
			for (newx = ((x / ts) +  1) * ts; x < newx; x++)
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

			if ((y = PDC_newline(win, y)) < 0)
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

		if ((y = PDC_newline(win, y)) < 0)
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

/*man-start*********************************************************************

  PDC_chg_attrs() - Change attributes in a rectangle

  PDCurses Description:
	This routine will change the attribute(s) from a starting (y,x)
	position to an ending (y,x) position to the specified attribute.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	It is an error to call this function with a NULL window pointer.
	It is also an error to pass rectangular coordinates that lay
	outside of window.

  Portability:
	PDCurses  int PDC_chg_attrs(WINDOW *w, chtype attr,
				    int sy, int sx, int ey, int ex);

**man-end**********************************************************************/

int PDC_chg_attrs(WINDOW *win, chtype attr, int sy, int sx, int ey, int ex)
{
	chtype oldattr = win->_attrs;
	int c, l;

	PDC_LOG(("PDC_chr_attrs() - called\n"));

	if ((win == (WINDOW *)NULL) || (sy > win->_maxy) || (sx > win->_maxx))
		return ERR;

	if (ey >= win->_maxy)
		ey = win->_maxy - 1;
	if (ex >= win->_maxx)
		ex = win->_maxx - 1;

	wattrset(win, attr);

	for (l = sy; l <= ey; l++)
	{
		for (c = sx; c <= ex; c++)
			win->_y[l][c] = (win->_y[l][c] & A_CHARTEXT) | attr;

		if (win->_firstch[l] == _NO_CHANGE)
		{
			win->_firstch[l] = sx;
			win->_lastch[l] = ex;
		}
		else if (win->_firstch[l] != _NO_CHANGE)
		{
			if (sx < win->_firstch[l])
				win->_firstch[l] = sx;
			if (ex > win->_lastch[l])
				win->_lastch[l] = ex;
		}
	}

	win->_attrs = oldattr;
	PDC_sync(win);

	return OK;
}

/*man-start*********************************************************************

  PDC_chins()  - Low-level insert character in window

  PDCurses Description:
	This is a private PDCurses routine.

	This routine provides the basic functionality for the X/Open
	[mv][w]insch() routines.  The xlat flag indicates that normal
	character translation is performed or not.  If not, then the
	character is output as is.

	The 'xlat' flag is TRUE for the normal curses routines.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	It is an error to call this function with a NULL window pointer.

  Portability:
	PDCurses  int PDC_chins(WINDOW* win, chtype c, bool xlat);

**man-end**********************************************************************/

int PDC_chins(WINDOW *win, chtype c, bool xlat)
{
	int x, y, maxx, offset;
	chtype *temp1;
	char ch = (c & A_CHARTEXT);

	PDC_LOG(("PDC_chins() - called: win=%x ch=%x "
		"(char=%c attr=0x%x) xlat=%d\n", win, ch,
		ch & A_CHARTEXT, ch & A_ATTRIBUTES, xlat));

	if (win == (WINDOW *)NULL)
		return ERR;

	x = win->_curx;
	y = win->_cury;
	maxx = win->_maxx;
	offset = 1;
	temp1 = &win->_y[y][x];

	if ((ch < ' ') && xlat)
		offset++;

	memmove(temp1 + offset, temp1, (maxx - x - offset) * sizeof(chtype));

	win->_lastch[y] = maxx - 1;

	if ((win->_firstch[y] == _NO_CHANGE) || (win->_firstch[y] > x))
		win->_firstch[y] = x;

	/* PDC_chadd() fixes CTRL-chars too */

	return PDC_chadd(win, c, xlat, FALSE);
}

/*man-start*********************************************************************

  PDC_clr_scrn()  - Clears the physical screen and homes the cursor.

  PDCurses Description:
	This is an internal routine called by the doupdate() routines.

  PDCurses Return Value:
	This routine always returns OK.

  Portability:
	PDCurses  int PDC_clr_scrn(WINDOW *win);

**man-end**********************************************************************/

int PDC_clr_scrn(WINDOW *win)
{
#if !defined(XCURSES)
# if defined(UNIX_WCLR)
	chtype attrs = win->_attrs;
# else
	chtype attrs = win->_bkgd;
# endif
#endif
	PDC_LOG(("PDC_clr_scrn() - called\n"));

#if defined(XCURSES)
	XCursesInstruct(CURSES_CLEAR);
#else
	PDC_scroll(0, 0, LINES - 1, COLS - 1, 0, attrs);
	PDC_gotoxy(0, 0);
#endif
	return OK;
}

/*man-start*********************************************************************

  PDC_newline()   - Advances 1 newline from supplied line number.

  PDCurses Description:
	This is a private PDCurses routine.

	Does line advance and returns the new cursor line.  If error,
	return -1.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_newline(WINDOW *win, int lin);

**man-end**********************************************************************/

int PDC_newline(WINDOW *win, int lin)
{
	PDC_LOG(("PDC_newline() - called: line %d\n", lin));

	if (win == (WINDOW *)NULL)
		return -1;

	if (++lin > win->_bmarg)
	{
		lin--;

		if (win->_scroll)
		{
			scroll(win);

 /* wrs -- 7/11/93 ** it seems that System V Curses automatically refreshes
  *                   a window when scrolling occurs via a newline.  This
  *                   could be a feature that isn't intended, but I'll
  *                   implement it here as well for consistency.
  * fh -- 7/30/99 ** but it causes problems with panels -- a lower panel
  *                  might be refreshed without updating the obscuring
  *                  parts of other panels.
  */
			/* wrefresh(win); */
		}
		else
			return -1;

	}

	return lin;
}
