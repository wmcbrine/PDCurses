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
#undef attroff
#undef wattroff
#undef attron
#undef wattron
#undef attrset
#undef wattrset
#undef standend
#undef standout
#undef wstandend
#undef wstandout
#undef getattrs
#undef color_set
#undef wcolor_set

RCSID("$Id: attr.c,v 1.23 2006/06/19 04:40:06 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                          attr

  Synopsis:
	int attroff(chtype attrs);
	int wattroff(WINDOW *win, chtype attrs);
	int attron(chtype attrs);
	int wattron(WINDOW *win, chtype attrs);
	int attrset(chtype attrs);
	int wattrset(WINDOW *win, chtype attrs);
	int standend(void);
	int wstandend(WINDOW *win);
	int standout(void);
	int wstandout(WINDOW *win);
	chtype getattrs(WINDOW *win);
	int color_set(short color_pair, void *opts);
	int wcolor_set(WINDOW *win, short color_pair, void *opts);

	int attr_get(attr_t *attrs, short *color_pair, void *opts);
	int attr_off(attr_t attrs, void *opts);
	int attr_on(attr_t attrs, void *opts);
	int attr_set(attr_t attrs, short color_pair, void *opts);
	int wattr_get(WINDOW *win, attr_t *attrs, short *color_pair,
		void *opts);
	int wattr_off(WINDOW *win, attr_t attrs, void *opts);
	int wattr_on(WINDOW *win, attr_t attrs, void *opts);
	int wattr_set(WINDOW *win, attr_t attrs, short color_pair,
		void *opts);

	int chgat(int n, attr_t attr, short color, const void *opts);
	int mvchgat(int y, int x, int n, attr_t attr, short color,
		const void *opts);
	int mvwchgat(WINDOW *win, int y, int x, int n, attr_t attr,
		short color, const void *opts);
	int wchgat(WINDOW *win, int n, attr_t attr, short color,
		const void *opts);

  X/Open Description:
	These functions manipulate the current attributes and/or colors 
	of the named window.  These attributes can be any combination 
	of A_STANDOUT, A_REVERSE, A_BOLD, A_DIM, A_BLINK, A_UNDERLINE.

	These constants are defined in <curses.h> and can be combined
	with the bitwise-OR operator (|).

	The current attributes of a window are applied to all characters
	that are written into the window with waddch().  Attributes are
	a property of the character, and move with the character
	through any scrolling and insert/delete line/character operations.
	To the extent possible on the particular terminal, they will be
	displayed as the graphic rendition of characters put on the
	screen.

	The attrset() function sets the current attributes of the given
	window to attrs.  The attroff() function turns off the named
	attributes without turning on or off any other attributes.  The
	attron() function turns on the named attributes without affecting
	any others.  The color_set() function sets the window color to 
	the value of color_pair.

	The standout() function is the same as attron(A_STANDOUT).  
	The standend() function is the same as attrset(A_NORMAL); that 
	is, it turns off all attributes.

	NOTE:	attroff(), attron(), attrset(), standend(), standout(),
 		wstandend(), and wstandout() are defined as macros.

  PDCurses Description:
	System V compatible color support is included.
	See <curses.h> for further details.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	It is an error to call this function with a NULL window pointer.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	attroff					Y	Y	Y
	wattroff				Y	Y	Y
	attron					Y	Y	Y
	wattron					Y	Y	Y
	attrset					Y	Y	Y
	wattrset				Y	Y	Y
	standend				Y	Y	Y
	wstandend				Y	Y	Y
	standout				Y	Y	Y
	wstandout				Y	Y	Y

**man-end****************************************************************/

int attroff(chtype attrs)
{
	PDC_LOG(("attroff() - called\n"));

	return wattroff(stdscr, attrs);
}

int wattroff(WINDOW *win, chtype attrs)
{
	PDC_LOG(("wattroff() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_attrs &= (~attrs & A_ATTRIBUTES);

	return OK;
}

int attron(chtype attrs)
{
	PDC_LOG(("attron() - called\n"));

	return wattron(stdscr, attrs);
}

int wattron(WINDOW *win, chtype attrs)
{
	chtype newcolr, oldcolr;
	chtype newattr, oldattr;

	PDC_LOG(("wattron() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if ((win->_attrs & A_COLOR) && (attrs & A_COLOR)) 
	{
		oldcolr = win->_attrs & A_COLOR;
		oldattr = win->_attrs ^ oldcolr;
		newcolr = attrs & A_COLOR;
		newattr = (attrs & A_ATTRIBUTES) ^ newcolr;
		newattr |= oldattr;
		win->_attrs = newattr | newcolr;
	}
	else
		win->_attrs |= (attrs & A_ATTRIBUTES);

	return OK;
}

int attrset(chtype attrs)
{
	PDC_LOG(("attrset() - called\n"));

	return wattrset(stdscr, attrs);
}

int wattrset(WINDOW *win, chtype attrs)
{
	PDC_LOG(("wattrset() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_attrs = attrs & A_ATTRIBUTES;

	return OK;
}

int standend(void)
{
	PDC_LOG(("standend() - called\n"));

	return wattrset(stdscr, A_NORMAL);
}

int standout(void)
{
	PDC_LOG(("standout() - called\n"));

	return wattrset(stdscr, A_STANDOUT);
}

int wstandend(WINDOW *win)
{
	PDC_LOG(("wstandend() - called\n"));

	return wattrset(win, A_NORMAL);
}

int wstandout(WINDOW *win)
{
	PDC_LOG(("wstandout() - called\n"));

	return wattrset(win, A_STANDOUT);
}

chtype getattrs(WINDOW *win)
{
	return win ? win->_attrs : 0;
}

int color_set(short color_pair, void *opts)
{
	PDC_LOG(("color_set() - called\n"));

	return wcolor_set(stdscr, color_pair, opts);
}

int wcolor_set(WINDOW *win, short color_pair, void *opts)
{
	PDC_LOG(("wcolor_set() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_attrs = (win->_attrs & ~A_COLOR) | COLOR_PAIR(color_pair);

	return OK;
}

int attr_get(attr_t *attrs, short *color_pair, void *opts)
{
	PDC_LOG(("attr_get() - called\n"));

	return wattr_get(stdscr, attrs, color_pair, opts);
}

int attr_off(attr_t attrs, void *opts)
{
	PDC_LOG(("attr_off() - called\n"));

	return wattroff(stdscr, attrs);
}

int attr_on(attr_t attrs, void *opts)
{
	PDC_LOG(("attr_on() - called\n"));

	return wattron(stdscr, attrs);
}

int attr_set(attr_t attrs, short color_pair, void *opts)
{
	PDC_LOG(("attr_get() - called\n"));

	return wattr_set(stdscr, attrs, color_pair, opts);
}

int wattr_get(WINDOW *win, attr_t *attrs, short *color_pair, void *opts)
{
	PDC_LOG(("wattr_get() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	if (attrs != (attr_t *)NULL)
		*attrs = win->_attrs & (A_ATTRIBUTES & ~A_COLOR);

	if (color_pair != (short *)NULL)
		*color_pair = PAIR_NUMBER(win->_attrs);

	return OK;
}

int wattr_off(WINDOW *win, attr_t attrs, void *opts)
{
	PDC_LOG(("wattr_off() - called\n"));

	return wattroff(win, attrs);
}

int wattr_on(WINDOW *win, attr_t attrs, void *opts)
{
	PDC_LOG(("wattr_off() - called\n"));

	return wattron(win, attrs);
}

int wattr_set(WINDOW *win, attr_t attrs, short color_pair, void *opts)
{
	PDC_LOG(("wattr_set() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	win->_attrs = (attrs & (A_ATTRIBUTES & ~A_COLOR)) |
		COLOR_PAIR(color_pair);

	return OK;
}

int chgat(int n, attr_t attr, short color, const void *opts)
{
	PDC_LOG(("wchgat() - called\n"));

	return wchgat(stdscr, n, attr, color, opts);
}

int mvchgat(int y, int x, int n, attr_t attr, short color, const void *opts)
{
	PDC_LOG(("mvchgat() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wchgat(stdscr, n, attr, color, opts);
}

int mvwchgat(WINDOW *win, int y, int x, int n, attr_t attr,
	     short color, const void *opts)
{
	PDC_LOG(("mvwchgat() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wchgat(win, n, attr, color, opts);
}

int wchgat(WINDOW *win, int n, attr_t attr, short color, const void *opts)
{
	chtype newattr, tmp;
        int basey, basex, imax, ic;

	if (win == (WINDOW *)NULL)
		return ERR;

	newattr = (attr & A_ATTRIBUTES) | COLOR_PAIR(color);

        basey = win->_cury;
        basex = win->_curx;
        imax = win->_maxx - win->_curx;

        if (n > 0)
                imax = ((imax < n) ? imax : n);

        for (ic = 0; ic < imax; ic++)
	{
		tmp = (win->_y[basey][basex + ic] & A_CHARTEXT) | newattr;
		win->_y[basey][basex + ic] = tmp;
	}

	return OK;
}
