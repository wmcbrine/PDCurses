/*
***************************************************************************
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
***************************************************************************
*/
#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef	border
#undef	wborder
#undef	box
#undef	hline
#undef	whline
#undef	vline
#undef	wvline
#undef	PDC_wunderline
#undef	PDC_leftline
#undef	PDC_rightline

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_border  = "$Id: border.c,v 1.7 2006/01/13 01:17:59 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        border

  Synopsis:
	int border(chtype ls, chtype rs, chtype ts, chtype bs, chtype tl, 
		   chtype tr, chtype bl, chtype br);
	int wborder(WINDOW *win, chtype ls, chtype rs, chtype ts, 
		    chtype bs, chtype tl, chtype tr, chtype bl, chtype br);
	int box(WINDOW *win, chtype verch, chtype horch);
	int hline(chtype ch, int n);
	int vline(chtype ch, int n);
	int whline(WINDOW *win, chtype ch, int n);
	int wvline(WINDOW *win, chtype ch, int n);
	int PDC_wunderline(WINDOW *win, int n, bool state);
	int PDC_wleftline(WINDOW *win, int n, bool state);
	int PDC_wrightline(WINDOW *win, int n, bool state);

  X/Open Description:
	The border(), wborder(), and box() routines, a border is drawn
	around the edge of the window. If any of the arguments is zero,
	an appropriate default is used. These default values are specified
	in <curses.h>. The arguments and defaults to these functions are:
		ls	left side of border		ACS_VLINE
		rs	right side of border		ACS_VLINE
		ts	top side of border		ACS_HLINE
		bs	bottom side of border		ACS_HLINE
		tl	top left corner of border	ACS_ULCORNER
		tr	top right corner of border	ACS_URCORNER
		bl	bottom left corner of border	ACS_BLCORNER
		br	bottom right corner of border	ACS_BLCORNER

	The hline() and whline() functions draw a left to right line
	using ch starting from the current cursor position. The cursor
	position does not change. The line is at most n characters long
	or as many as will fit in the window.

	The vline() and wvline() functions draw a top to bottom line
	using ch starting from the current cursor position. The cursor
	position does not change. The line is at most n characters long
	or as many as will fit in the window.

	border(), box(), hline(), and vline() are implemented as macros.

  X/Open Return Value:
	These functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for these functions.

  Portability                             X/Open    BSD    SYS V  PDCurses
                                          Dec '88
      border                                -        -      4.0      Y
      wborder                               -        -      4.0      Y
      box                                   Y        Y       Y       Y
      hline                                 -        -      4.0      Y
      whline                                -        -      4.0      Y
      vline                                 -        -      4.0      Y
      wvline                                -        -      4.0      Y
      PDC_wunderline                        -        -       -       Y
      PDC_wleftline                         -        -       -       Y
      PDC_wrightline                        -        -       -       Y

**man-end**********************************************************************/

/*man-start*********************************************************************
  PDC_attr_passthru()   - Combines chtype with current and/or background 
			  attributes of a window

  PDCurses Description:
	This is a private PDCurses function.

	Takes a single chtype 'ch' and checks if the current attribute 
	of window 'win', as set by wattrset(), and/or the current 
	background of win, as set by wbkgd(), should by combined with 
	it. Attributes set explicitly in ch take precedence.
 
  PDCurses Return Value:
	Returns ch, combined (or not) with existing attributes.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses chtype PDC_attr_passthru( WINDOW *win, chtype ch );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
chtype PDC_attr_passthru(WINDOW *win, chtype ch)
#else
chtype PDC_attr_passthru(win, ch)
WINDOW *win;
chtype ch;
#endif
/***********************************************************************/
{
	chtype attr, bktmp;

   /*
    * If the incoming character doesn't have its own attribute,
    * then use the current attributes for the window.
    * If the incoming character has attributes, but not a colour
    * component, OR the attributes to the current attributes
    * for the window.
    * If the incoming character has a colour component, use only
    * the attributes from the incoming character.
    */

	if ((ch & A_ATTRIBUTES) == 0)
		attr = win->_attrs;
	else
		if ((ch & A_COLOR) == 0)
			attr = (ch & A_ATTRIBUTES) | win->_attrs;
		else
			attr = (ch & A_ATTRIBUTES);

   /* wrs (4/10/93)
    * Apply the same sort of logic for the window background, in that it 
    * only takes precedence if other color attributes are not there.
    */

	if ((attr & A_COLOR) == 0)
		attr = (attr | (win->_bkgd & A_ATTRIBUTES));
	else
	{
		bktmp = (win->_bkgd & A_COLOR);
		attr = (attr | ( (win->_bkgd & A_ATTRIBUTES) ^ bktmp ));
	}

	ch = (ch & A_CHARTEXT) | attr;

	return ch;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wborder(WINDOW *win, chtype ls, chtype rs,
				chtype ts, chtype bs, chtype tl,
				chtype tr, chtype bl, chtype br)
#else
int	PDC_CDECL	wborder(win, ls, rs, ts, bs, tl, tr, bl, br)
WINDOW *win;
chtype ls;
chtype rs;
chtype ts;
chtype bs;
chtype tl;
chtype tr;
chtype bl;
chtype br;
#endif
/***********************************************************************/
{
	int ymax, xmax;
	int ymin, xmin;
	int i;

	PDC_LOG(("wborder() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	ymax = win->_maxy - 1;
	xmax = win->_maxx - 1;
	ymin = 0;
	xmin = 0;

	ls = PDC_attr_passthru(win, ls ? ls : ACS_VLINE);
	rs = PDC_attr_passthru(win, rs ? rs : ACS_VLINE);
	ts = PDC_attr_passthru(win, ts ? ts : ACS_HLINE);
	bs = PDC_attr_passthru(win, bs ? bs : ACS_HLINE);
	tl = PDC_attr_passthru(win, tl ? tl : ACS_ULCORNER);
	tr = PDC_attr_passthru(win, tr ? tr : ACS_URCORNER);
	bl = PDC_attr_passthru(win, bl ? bl : ACS_LLCORNER);
	br = PDC_attr_passthru(win, br ? br : ACS_LRCORNER);

	for (i = xmin + 1; i <= xmax - 1; i++)
	{
		win->_y[ymin][i] = ts;
		win->_y[ymax][i] = bs;
	}

	for (i = ymin + 1; i <= ymax - 1; i++)
	{
		win->_y[i][xmin] = ls;
		win->_y[i][xmax] = rs;
	}

	win->_y[ymin][xmin] = tl;
	win->_y[ymin][xmax] = tr;
	win->_y[ymax][xmin] = bl;
	win->_y[ymax][xmax] = br;

	for (i = ymin; i <= ymax; i++)
	{
		if (win->_firstch[i] == _NO_CHANGE)
		{
			win->_firstch[i] = xmin;
			win->_lastch[i] = xmax;
		}
		else
		{
			win->_firstch[i] = min(win->_firstch[i], xmin);
			win->_lastch[i] = max(win->_lastch[i], xmax);
		}
	}

	PDC_sync(win);

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	border(chtype ls, chtype rs, chtype ts, chtype bs,
			       chtype tl, chtype tr, chtype bl, chtype br)
#else
int	PDC_CDECL	border(ls, rs, ts, bs, tl, tr, bl, br)
chtype ls;
chtype rs;
chtype ts;
chtype bs;
chtype tl;
chtype tr;
chtype bl;
chtype br;
#endif
/***********************************************************************/
{
	PDC_LOG(("border() - called\n"));

	return wborder(stdscr, ls, rs, ts, bs, tl, tr, bl, br);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	box(WINDOW *win, chtype verch, chtype horch)
#else
int	PDC_CDECL	box(win, verch, horch)
WINDOW *win;
chtype verch;
chtype horch;
#endif
/***********************************************************************/
{
	PDC_LOG(("box() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return wborder(win, verch, verch, horch, horch, 0, 0, 0, 0);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	hline(chtype ch, int n)
#else
int	PDC_CDECL	hline(ch, n)
chtype ch;
int n;
#endif
/***********************************************************************/
{
	PDC_LOG(("hline() - called\n"));

	return whline(stdscr, ch, n);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	whline(WINDOW *win, chtype ch, int n)
#else
int	PDC_CDECL	whline(win, ch, n)
WINDOW *win;
chtype ch;
int n;
#endif
/***********************************************************************/
{
	int startpos, endpos;

	PDC_LOG(("whline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n < 1)
		return ERR;

	endpos = min(win->_curx + n - 1, win->_maxx);

	ch = PDC_attr_passthru(win, ch ? ch : ACS_HLINE);

	startpos = win->_curx;

	for (n = win->_curx; n <= endpos; n++)
		win->_y[win->_cury][n] = ch;

	n = win->_cury;

	if (win->_firstch[n] == _NO_CHANGE)
	{
		win->_firstch[n] = startpos;
		win->_lastch[n] = endpos;
	}
	else
	{
		win->_firstch[n] = min(win->_firstch[n], startpos);
		win->_lastch[n] = max(win->_lastch[n], endpos);
	}

	PDC_sync(win);

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	vline(chtype ch, int n)
#else
int	PDC_CDECL	vline(ch, n)
chtype ch;
int n;
#endif
/***********************************************************************/
{
	PDC_LOG(("vline() - called\n"));

	return wvline(stdscr, ch, n);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wvline(WINDOW *win, chtype ch, int n)
#else
int	PDC_CDECL	wvline(win, ch, n)
WINDOW *win;
chtype ch;
int n;
#endif
/***********************************************************************/
{
	int endpos;

	PDC_LOG(("wvline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n < 1)
		return ERR;

	endpos = min(win->_cury + n - 1, win->_maxy);

	ch = PDC_attr_passthru(win, ch ? ch : ACS_VLINE);

	for (n = win->_cury; n <= endpos; n++)
	{
		win->_y[n][win->_curx] = ch;

		if (win->_firstch[n] == _NO_CHANGE)
		{
			win->_firstch[n] = win->_curx;
			win->_lastch[n] = win->_curx;
		}
		else
		{
			win->_firstch[n] = min(win->_firstch[n], win->_curx);
			win->_lastch[n] = max(win->_lastch[n], win->_curx);
		}
	}

	PDC_sync(win);

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	PDC_wunderline(WINDOW *win, int n, bool state)
#else
int	PDC_CDECL	PDC_wunderline(win, n, state)
WINDOW *win;
int n;
bool state;
#endif
/***********************************************************************/
{
	int endpos;

	PDC_LOG(("PDC_wunderline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n < 1)
		return ERR;

	endpos = min(win->_cury + n - 1, win->_maxy);

	for (n = win->_cury; n <= endpos; n++)
	{
		if (state) 
			win->_y[n][win->_curx] |= A_UNDERLINE;
		else
			win->_y[n][win->_curx] |= ~A_UNDERLINE;

		if (win->_firstch[n] == _NO_CHANGE)
		{
			win->_firstch[n] = win->_curx;
			win->_lastch[n] = win->_curx;
		}
		else
		{
			win->_firstch[n] = min(win->_firstch[n], win->_curx);
			win->_lastch[n] = max(win->_lastch[n], win->_curx);
		}
	}

	PDC_sync(win);

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	PDC_wleftline(WINDOW *win, int n, bool state)
#else
int	PDC_CDECL	PDC_wleftline(win, n, state)
WINDOW *win;
int n;
bool state;
#endif
/***********************************************************************/
{
	int endpos;

	PDC_LOG(("PDC_wleftline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR ;

	if (n < 1)
		return ERR;

	endpos = min(win->_cury + n - 1, win->_maxy);

	for (n = win->_cury; n <= endpos; n++)
	{
		if ( state ) 
			win->_y[n][win->_curx] |= A_LEFTLINE;
		else
			win->_y[n][win->_curx] |= ~A_LEFTLINE;

		if (win->_firstch[n] == _NO_CHANGE)
		{
			win->_firstch[n] = win->_curx;
			win->_lastch[n] = win->_curx;
		}
		else
		{
			win->_firstch[n] = min(win->_firstch[n], win->_curx);
			win->_lastch[n] = max(win->_lastch[n], win->_curx);
		}
	}

	PDC_sync(win);

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	PDC_wrightline(WINDOW *win, int n, bool state)
#else
int	PDC_CDECL	PDC_wrightline(win, n, state)
WINDOW *win;
int n;
bool state;
#endif
/***********************************************************************/
{
	int endpos;

	PDC_LOG(("PDC_wrightline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n < 1)
		return ERR;

	endpos = min(win->_cury + n - 1, win->_maxy);

	for (n = win->_cury; n <= endpos; n++)
	{
		if (state) 
			win->_y[n][win->_curx] |= A_RIGHTLINE;
		else
			win->_y[n][win->_curx] |= ~A_RIGHTLINE;

		if (win->_firstch[n] == _NO_CHANGE)
		{
			win->_firstch[n] = win->_curx;
			win->_lastch[n] = win->_curx;
		}
		else
		{
			win->_firstch[n] = min(win->_firstch[n], win->_curx);
			win->_lastch[n] = max(win->_lastch[n], win->_curx);
		}
	}

	PDC_sync(win);

	return OK;
}
