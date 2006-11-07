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

RCSID("$Id: addch.c,v 1.43 2006/11/07 16:32:14 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                         addch

  Synopsis:
	int addch(const chtype ch);
	int waddch(WINDOW *win, const chtype ch);
	int mvaddch(int y, int x, const chtype ch);
	int mvwaddch(WINDOW *win, int y, int x, const chtype ch);
	int echochar(const chtype ch);
	int wechochar(WINDOW *win, const chtype ch);

	int add_wch(const cchar_t *wch);
	int wadd_wch(WINDOW *win, const cchar_t *wch);
	int mvadd_wch(int y, int x, const cchar_t *wch);
	int mvwadd_wch(WINDOW *win, int y, int x, const cchar_t *wch);
	int echo_wchar(const cchar_t *wch);
	int wecho_wchar(WINDOW *win, const cchar_t *wch);

  X/Open Description:
	The routine addch() inserts the character ch into the default
	window at the current cursor position and the window cursor is
	advanced.  The character is of the type chtype as containing
	both data and attributes.

	The routine waddch() inserts the character ch into the specified
	window at the current cursor position.  The cursor position is
	advanced.

	The routine mvaddch() moves the cursor to the specified (y, x)
	position and inserts the character ch into the default window.
	The cursor position is advanced after the character has been
	inserted.

	The routine mvwaddch() moves the cursor to the specified (y, x)
	position and inserts the character ch into the specified
	window.  The cursor position is advanced after the character
	has been inserted.

	The routine echochar() inserts the character ch into stdscr
	at the current cursor position and a refresh() is called.  
	The cursor position is advanced.

	The routine wechochar() inserts the character ch into the
	specified window at the current cursor position and a wrefresh() 
	is called. The cursor position is advanced.

	All these routines are similar to putchar().  The following
	information applies to all the routines.

	If the cursor moves on to the right margin, an automatic
	newline is performed.  If scrollok is enabled, and a character
	is added to the bottom right corner of the screen, the
	scrolling region will be scrolled up one line.  If scrolling
	is not allowed, ERR will be returned.

	If ch is a tab, newline, or backspace, the cursor will be
	moved appropriately within the window.  If ch is a newline,
	the clrtoeol routine is called before the cursor is moved to
	the beginning of the next line.  If newline mapping is off,
	the cursor will be moved to the next line, but the x
	coordinate will be unchanged.  If ch is a tab the cursor is
	moved to the next tab position within the window.  If ch is
	another control character, it will be drawn in the ^X
	notation.  Calling the inch() routine after adding a control
	character returns the representation of the control character,
	not the control character.

	Video attributes can be combined with a character by ORing
	them into the parameter.  This will result in these attributes
	being set.  The intent here is that text, including
	attributes, can be copied from one place to another using inch()
	and addch().

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
	addch					Y	Y	Y
	waddch					Y	Y	Y
	mvaddch					Y	Y	Y
	mvwaddch				Y	Y	Y
	echochar				Y	-      3.0
	wechochar				Y	-      3.0
	add_wch					Y
	wadd_wch				Y
	mvadd_wch				Y
	mvwadd_wch				Y
	echo_wchar				Y
	wecho_wchar				Y

**man-end****************************************************************/

int waddch(WINDOW *win, chtype ch)
{
	int x, y;
	chtype attr;
	bool xlat;

	PDC_LOG(("waddch() - called: win=%p ch=%x "
		"(char=%c attr=0x%x)\n", win, ch,
		ch & A_CHARTEXT, ch & A_ATTRIBUTES));

	if (!win)
		return ERR;

	x = win->_curx;
	y = win->_cury;

	if (y > win->_maxy || x > win->_maxx || y < 0 || x < 0)
		return ERR;

	xlat = !SP->raw_out && !(ch & A_ALTCHARSET);
	attr = ch & A_ATTRIBUTES;
	ch &= A_CHARTEXT;

	if (xlat && (ch < ' ' || ch == 0x7f))
	{
		int x2;

		switch (ch)
		{
		case '\t':
			for (x2 = ((x / TABSIZE) + 1) * TABSIZE; x < x2; x++)
			{
				if (waddch(win, ' ') == ERR)
					return ERR;

				/* if tab to next line, exit the loop */

				if (!win->_curx)
					break;
			}
			return OK;

		case '\n':
			/* if lf -> crlf */

			if (!SP->raw_out)
				x = 0;

			wclrtoeol(win);

			if (++y > win->_bmarg)
			{
				y--;

				if (wscrl(win, 1) == ERR)
					return ERR;
			}

			break;

		case '\b':
			/* don't back over left margin */

			if (--x < 0)
		case '\r':
				x = 0;

			break;

		case 0x7f:
			if (waddch(win, '^') == ERR)
				return ERR;

			return waddch(win, '?');

		default:
			/* handle control chars */

			if (waddch(win, '^') == ERR)
				return ERR;

			return waddch(win, ch + '@');
		}
	}
	else
	{
		/* If the incoming character doesn't have its own
		   attribute, then use the current attributes for
		   the window. If it has attributes but not a color
		   component, OR the attributes to the current
		   attributes for the window. If it has a color
		   component, use the attributes solely from the
		   incoming character. */

		if (!(attr & A_COLOR))
			attr |= win->_attrs;

		/* wrs (4/10/93): Apply the same sort of logic for the
		   window background, in that it only takes precedence
		   if other color attributes are not there and that the
		   background character will only print if the printing
		   character is blank. */

		if (!(attr & A_COLOR))
			attr |= win->_bkgd & A_ATTRIBUTES;
		else
			attr |= win->_bkgd & (A_ATTRIBUTES ^ A_COLOR);

		if (ch == ' ')
			ch = win->_bkgd & A_CHARTEXT;

		/* Add the attribute back into the character. */

		ch |= attr;

		/* Only change _firstch/_lastch if the character to be
		   added is different from the character/attribute that
		   is already in that position in the window. */

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

			win->_y[y][x] = ch;
		}

		if (++x >= win->_maxx)
		{
			/* wrap around test */

			x = 0;

			if (++y > win->_bmarg)
			{
				y--;

				if (wscrl(win, 1) == ERR)
				{
					PDC_sync(win);
					return ERR;
				}
			}
		}
	}

	win->_curx = x;
	win->_cury = y;

	if (win->_immed)
		wrefresh(win);
	if (win->_sync)
		wsyncup(win);

	return OK;
}

int addch(const chtype ch)
{
	PDC_LOG(("addch() - called: ch=%x\n", ch));

	return waddch(stdscr, ch);
}

int mvaddch(int y, int x, const chtype ch)
{
	PDC_LOG(("mvaddch() - called: y=%d x=%d ch=%x\n", y, x, ch));

	if (move(y,x) == ERR)
		return ERR;

	return waddch(stdscr, ch);
}

int mvwaddch(WINDOW *win, int y, int x, const chtype ch)
{
	PDC_LOG(("mvwaddch() - called: win=%p y=%d x=%d ch=%d\n",
		win, y, x, ch));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return waddch(win, ch);
}

int echochar(const chtype ch)
{
	PDC_LOG(("echochar() - called: ch=%x\n", ch));

	return wechochar(stdscr, ch);
}

int wechochar(WINDOW *win, const chtype ch)
{
	PDC_LOG(("wechochar() - called: win=%p ch=%x\n", win, ch));

	if (waddch(win, ch) == ERR)
		return ERR;

	return wrefresh(win);
}

#ifdef PDC_WIDE
int wadd_wch(WINDOW *win, const cchar_t *wch)
{
	PDC_LOG(("wadd_wch() - called: win=%p wch=%x\n", win, *wch));

	return wch ? waddch(win, *wch) : ERR;
}

int add_wch(const cchar_t *wch)
{
	PDC_LOG(("add_wch() - called: wch=%x\n", *wch));

	return wadd_wch(stdscr, wch);
}

int mvadd_wch(int y, int x, const cchar_t *wch)
{
	PDC_LOG(("mvaddch() - called: y=%d x=%d wch=%x\n", y, x, *wch));

	if (move(y,x) == ERR)
		return ERR;

	return wadd_wch(stdscr, wch);
}

int mvwadd_wch(WINDOW *win, int y, int x, const cchar_t *wch)
{
	PDC_LOG(("mvwaddch() - called: win=%p y=%d x=%d wch=%d\n",
		win, y, x, *wch));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wadd_wch(win, wch);
}

int echo_wchar(const cchar_t *wch)
{
	PDC_LOG(("echo_wchar() - called: wch=%x\n", *wch));

	return wecho_wchar(stdscr, wch);
}

int wecho_wchar(WINDOW *win, const cchar_t *wch)
{
	PDC_LOG(("wecho_wchar() - called: win=%p wch=%x\n", win, *wch));

	if (!wch || (wadd_wch(win, wch) == ERR))
		return ERR;

	return wrefresh(win);
}
#endif
