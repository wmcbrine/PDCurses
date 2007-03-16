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

RCSID("$Id: scroll.c,v 1.29 2007/03/16 06:33:44 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								scroll

  Synopsis:
	int scroll(WINDOW *win);
	int scrl(int n);
	int wscrl(WINDOW *win, int n);

  Description:
	scroll() causes the window to scroll up one line.  This involves 
	moving the lines in the window data strcture.
 
	With a positive n, scrl() and wscrl() scroll the window up n 
	lines (line i + n becomes i); otherwise they scroll the window 
	down n lines.
 
	For these functions to work, scrolling must be enabled via 
	scrollok(). Note also that scrolling is not allowed if the 
	supplied window is a pad.

  Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
	scroll					Y	Y	Y
	scrl					Y	-      4.0
	wscrl					Y	-      4.0

**man-end****************************************************************/

int wscrl(WINDOW *win, int n)
{
	int i, l;
	chtype blank, *ptr, *temp;

	/* Check if window scrolls. Valid for window AND pad */

	if (!win || !win->_scroll)
		return ERR;

	/* wrs (4/10/93) account for window background */

	blank = win->_bkgd;

	/* wrs -- 7/11/93 -- quick add to original scroll() routine to 
	   implement scrolling for a specified number of lines (not very 
	   efficient for more than 1 line) */

	if (n >= 0)
	{
		for (l = 0; l < n; l++) 
		{
			temp = win->_y[win->_tmarg];

			/* re-arrange line pointers */

			for (i = win->_tmarg; i < win->_bmarg; i++)
			{
				win->_y[i] = win->_y[i + 1];
				win->_firstch[i] = 0;
				win->_lastch[i] = win->_maxx - 1;
			}

			/* make a blank line */

			for (ptr = temp; (ptr - temp) < win->_maxx; ptr++)
				*ptr = blank;

			win->_y[win->_bmarg] = temp;
			win->_firstch[win->_bmarg] = 0;
			win->_lastch[win->_bmarg] = win->_maxx - 1;
		}
	}
	else 
	{
		for (l = n; l < 0; l++)
		{
			temp = win->_y[win->_bmarg];

			/* re-arrange line pointers */

			for (i = win->_bmarg; i > win->_tmarg; i--)
			{
				win->_y[i] = win->_y[i - 1];
				win->_firstch[i] = 0;
				win->_lastch[i] = win->_maxx - 1;
			}

			/* make a blank line */

			for (ptr = temp; (ptr - temp) < win->_maxx; ptr++)
				*ptr = blank;

			win->_y[win->_tmarg] = temp;
			win->_firstch[win->_tmarg] = 0;
			win->_lastch[win->_tmarg] = win->_maxx - 1;
		}
	}

	PDC_sync(win);
	return OK;
}

int scrl(int n)
{
	PDC_LOG(("scrl() - called\n"));

	return wscrl(stdscr, n);
}

int scroll(WINDOW *win)
{
	PDC_LOG(("scroll() - called\n"));

	return wscrl(win, 1);
}
