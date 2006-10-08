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

#include <curspriv.h>

RCSID("$Id: border.c,v 1.36 2006/10/08 20:54:30 wmcbrine Exp $");

/*man-start**************************************************************

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
	int mvhline(int y, int x, chtype ch, int n);
	int mvvline(int y, int x, chtype ch, int n);
	int mvwhline(WINDOW *win, int y, int x, chtype ch, int n);
	int mvwvline(WINDOW *win, int y, int x, chtype ch, int n);

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

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	border					-	-      4.0
	wborder					-	-      4.0
	box					Y	Y	Y
	hline					-	-      4.0
	vline					-	-      4.0
	whline					-	-      4.0
	wvline					-	-      4.0
	mvhline
	mvvline
	mvwhline
	mvwvline

**man-end****************************************************************/

/* _attr_passthru() -- Takes a single chtype 'ch' and checks if the 
   current attribute of window 'win', as set by wattrset(), and/or the 
   current background of win, as set by wbkgd(), should by combined with 
   it. Attributes set explicitly in ch take precedence. */

static chtype _attr_passthru(WINDOW *win, chtype ch)
{
	chtype attr, bktmp;

	/* If the incoming character doesn't have its own attribute, 
	   then use the current attributes for the window. If the 
	   incoming character has attributes, but not a color 
	   component, OR the attributes to the current attributes for 
	   the window. If the incoming character has a color component, 
	   use only the attributes from the incoming character. */

	if ((ch & A_ATTRIBUTES) == 0)
		attr = win->_attrs;
	else
		if ((ch & A_COLOR) == 0)
			attr = (ch & A_ATTRIBUTES) | win->_attrs;
		else
			attr = ch & A_ATTRIBUTES;

	/* wrs (4/10/93) -- Apply the same sort of logic for the window 
	   background, in that it only takes precedence if other color 
	   attributes are not there. */

	if ((attr & A_COLOR) == 0)
		attr |= win->_bkgd & A_ATTRIBUTES;
	else
	{
		bktmp = win->_bkgd & A_COLOR;
		attr |= (win->_bkgd & A_ATTRIBUTES) ^ bktmp;
	}

	ch = (ch & A_CHARTEXT) | attr;

	return ch;
}

int border(chtype ls, chtype rs, chtype ts, chtype bs,
		     chtype tl, chtype tr, chtype bl, chtype br)
{
	PDC_LOG(("border() - called\n"));

	return wborder(stdscr, ls, rs, ts, bs, tl, tr, bl, br);
}

int wborder(WINDOW *win, chtype ls, chtype rs, chtype ts, 
	    chtype bs, chtype tl, chtype tr, chtype bl, chtype br)
{
	int i, ymax, xmax;

	PDC_LOG(("wborder() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	ymax = win->_maxy - 1;
	xmax = win->_maxx - 1;

	ls = _attr_passthru(win, ls ? ls : ACS_VLINE);
	rs = _attr_passthru(win, rs ? rs : ACS_VLINE);
	ts = _attr_passthru(win, ts ? ts : ACS_HLINE);
	bs = _attr_passthru(win, bs ? bs : ACS_HLINE);
	tl = _attr_passthru(win, tl ? tl : ACS_ULCORNER);
	tr = _attr_passthru(win, tr ? tr : ACS_URCORNER);
	bl = _attr_passthru(win, bl ? bl : ACS_LLCORNER);
	br = _attr_passthru(win, br ? br : ACS_LRCORNER);

	for (i = 1; i < xmax; i++)
	{
		win->_y[0][i] = ts;
		win->_y[ymax][i] = bs;
	}

	for (i = 1; i < ymax; i++)
	{
		win->_y[i][0] = ls;
		win->_y[i][xmax] = rs;
	}

	win->_y[0][0] = tl;
	win->_y[0][xmax] = tr;
	win->_y[ymax][0] = bl;
	win->_y[ymax][xmax] = br;

	for (i = 0; i <= ymax; i++)
	{
		win->_firstch[i] = 0;
		win->_lastch[i] = xmax;
	}

	PDC_sync(win);

	return OK;
}

int box(WINDOW *win, chtype verch, chtype horch)
{
	PDC_LOG(("box() - called\n"));

	return wborder(win, verch, verch, horch, horch, 0, 0, 0, 0);
}

int hline(chtype ch, int n)
{
	PDC_LOG(("hline() - called\n"));

	return whline(stdscr, ch, n);
}

int vline(chtype ch, int n)
{
	PDC_LOG(("vline() - called\n"));

	return wvline(stdscr, ch, n);
}

int whline(WINDOW *win, chtype ch, int n)
{
	int startpos, endpos;

	PDC_LOG(("whline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n < 1)
		return ERR;

	endpos = min(win->_curx + n, win->_maxx);

	ch = _attr_passthru(win, ch ? ch : ACS_HLINE);

	startpos = win->_curx;

	for (n = startpos; n < endpos; n++)
		win->_y[win->_cury][n] = ch;

	n = win->_cury;

	if (win->_firstch[n] == _NO_CHANGE)
	{
		win->_firstch[n] = startpos;
		win->_lastch[n] = endpos - 1;
	}
	else
	{
		win->_firstch[n] = min(win->_firstch[n], startpos);
		win->_lastch[n] = max(win->_lastch[n], endpos - 1);
	}

	PDC_sync(win);

	return OK;
}

int wvline(WINDOW *win, chtype ch, int n)
{
	int endpos;

	PDC_LOG(("wvline() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (n < 1)
		return ERR;

	endpos = min(win->_cury + n, win->_maxy);

	ch = _attr_passthru(win, ch ? ch : ACS_VLINE);

	for (n = win->_cury; n < endpos; n++)
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

int mvhline(int y, int x, chtype ch, int n)
{
	PDC_LOG(("mvhline() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return whline(stdscr, ch, n);
}

int mvvline(int y, int x, chtype ch, int n)
{
	PDC_LOG(("mvvline() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wvline(stdscr, ch, n);
}

int mvwhline(WINDOW *win, int y, int x, chtype ch, int n)
{
	PDC_LOG(("mvwhline() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return whline(win, ch, n);
}

int mvwvline(WINDOW *win, int y, int x, chtype ch, int n)
{
	PDC_LOG(("mvwvline() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wvline(win, ch, n);
}

#ifdef PDC_WIDE
int border_set(const cchar_t *ls, const cchar_t *rs, const cchar_t *ts,
	       const cchar_t *bs, const cchar_t *tl, const cchar_t *tr,
	       const cchar_t *bl, const cchar_t *br)
{
	PDC_LOG(("border_set() - called\n"));

	return wborder_set(stdscr, ls, rs, ts, bs, tl, tr, bl, br);
}

int wborder_set(WINDOW *win, const cchar_t *ls, const cchar_t *rs,
		const cchar_t *ts, const cchar_t *bs, const cchar_t *tl,
		const cchar_t *tr, const cchar_t *bl, const cchar_t *br)
{
	PDC_LOG(("wborder_set() - called\n"));

	return wborder(win,
		ls ? *ls : 0, rs ? *rs : 0, ts ? *ts : 0, bs ? *bs : 0,
		tl ? *tl : 0, tr ? *tr : 0, bl ? *bl : 0, br ? *br : 0);
}

int box_set(WINDOW *win, const cchar_t *verch, const cchar_t *horch)
{
	PDC_LOG(("box_set() - called\n"));

	return wborder_set(win, verch, verch, horch, horch,
		(const cchar_t *)NULL, (const cchar_t *)NULL,
		(const cchar_t *)NULL, (const cchar_t *)NULL);
}

int hline_set(const cchar_t *wch, int n)
{
	PDC_LOG(("hline_set() - called\n"));

	return whline_set(stdscr, wch, n);
}

int vline_set(const cchar_t *wch, int n)
{
	PDC_LOG(("vline_set() - called\n"));

	return wvline_set(stdscr, wch, n);
}

int whline_set(WINDOW *win, const cchar_t *wch, int n)
{
	PDC_LOG(("whline_set() - called\n"));

	return wch ? whline(win, *wch, n) : ERR;
}

int wvline_set(WINDOW *win, const cchar_t *wch, int n)
{
	PDC_LOG(("wvline_set() - called\n"));

	return wch ? wvline(win, *wch, n) : ERR;
}

int mvhline_set(int y, int x, const cchar_t *wch, int n)
{
	PDC_LOG(("mvhline_set() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return whline_set(stdscr, wch, n);
}

int mvvline_set(int y, int x, const cchar_t *wch, int n)
{
	PDC_LOG(("mvvline_set() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wvline_set(stdscr, wch, n);
}

int mvwhline_set(WINDOW *win, int y, int x, const cchar_t *wch, int n)
{
	PDC_LOG(("mvwhline_set() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return whline_set(win, wch, n);
}

int mvwvline_set(WINDOW *win, int y, int x, const cchar_t *wch, int n)
{
	PDC_LOG(("mvwvline_set() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wvline_set(win, wch, n);
}
#endif
