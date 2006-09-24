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
#include <string.h>

RCSID("$Id: insch.c,v 1.26 2006/09/24 21:22:33 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                         insch

  Synopsis:
	int insch(chtype ch);
	int winsch(WINDOW *win, chtype ch);
	int mvinsch(int y, int x, chtype ch);
	int mvwinsch(WINDOW *win, int y, int x, chtype ch);

	int PDC_chins(WINDOW* win, chtype c, bool xlat);

  X/Open Description:
	The routine insch() inserts the character ch into the default
	window at the current cursor position and the window cursor is
	advanced.  The character is of the type chtype as containing
	both data and attributes.

	The routine winsch() inserts the character ch into the specified
	window at the current cursor position.  The cursor position is
	advanced.

	The routine mvinsch() moves the cursor to the specified (y, x)
	position and inserts the character ch into the default window.
	The cursor position is advanced after the character has been
	inserted.

	The routine mvwinsch() moves the cursor to the specified (y, x)
	position and inserts the character ch into the specified
	window.  The cursor position is advanced after the character
	has been inserted.

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
	and insch().

	NOTE: All these functions are implemented as macros.

  PDCurses Description:
	Depending upon the state of the raw character output, 7- or
	8-bit characters will be output.

	PDC_chins() provides the basic functionality for [mv][w]insch().  
	The xlat flag indicates whether or not to perform normal 
	character translation. If not, then the character is output as 
	is. The 'xlat' flag is TRUE for the normal curses routines. This 
	function returns OK on success and ERR on error.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	insch					Y	Y	Y
	winsch					Y	Y	Y
	mvinsch					Y	Y	Y
	mvwinsch				Y	Y	Y
	PDC_chins				-	-	-

**man-end****************************************************************/

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

int insch(chtype ch)
{
	PDC_LOG(("insch() - called\n"));

	return winsch(stdscr, ch);
}

int winsch(WINDOW *win, chtype ch)
{
	PDC_LOG(("winsch() - called\n"));

	return PDC_chins(win, ch, (bool)(!(SP->raw_out)));
}

int mvinsch(int y, int x, chtype ch)
{
	PDC_LOG(("mvinsch() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return winsch(stdscr, ch);
}

int mvwinsch(WINDOW *win, int y, int x, chtype ch)
{
	PDC_LOG(("mvwinsch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return winsch(win, ch);
}

#ifdef PDC_WIDE
int ins_wch(const cchar_t *wch)
{
	PDC_LOG(("ins_wch() - called\n"));

	return wins_wch(stdscr, wch);
}

int wins_wch(WINDOW *win, const cchar_t *wch)
{
	PDC_LOG(("wins_wch() - called\n"));

	return wch ? PDC_chins(win, *wch, (bool)(!(SP->raw_out))) : ERR;
}

int mvins_wch(int y, int x, const cchar_t *wch)
{
	PDC_LOG(("mvins_wch() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wins_wch(stdscr, wch);
}

int mvwins_wch(WINDOW *win, int y, int x, const cchar_t *wch)
{
	PDC_LOG(("mvwins_wch() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wins_wch(win, wch);
}
#endif
