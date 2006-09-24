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

RCSID("$Id: getstr.c,v 1.30 2006/09/24 21:22:33 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                        getstr

  Synopsis:
	int getstr(char *str);
	int wgetstr(WINDOW *win, char *str);
	int mvgetstr(int y, int x, char *str);
	int mvwgetstr(WINDOW *win, int y, int x, char *str);
	int getnstr(char *str, int n);
	int wgetnstr(WINDOW *win, char *str, int n);
	int mvgetnstr(int y, int x, char *str, int n);
	int mvwgetnstr(WINDOW *win, int y, int x, char *str, int n);

  X/Open Description:
	The effect of getstr() is as though a series of calls to getch()
	were made, until a newline or carriage return is received. The
	resulting value is placed in the area pointed to by *str. The 
	user's erase and kill characters are interpreted, as well as any 
	special keys; such as function keys.

	With wgetnstr(), a series of characters are read until a newline 
	or carriage return is received.  The resulting value is placed 
	in the area pointed to by the character pointer str.  The user's 
	erase and kill characters are interpreted.  This differs from 
	wgetstr() in that the number of characters read is limited by a 
	passed argument.

	NOTE: getstr(), getnstr(), mvgetstr(), mvwgetstr(), mvgetnstr() 
	and mvwgetnstr() are implemented as macros.

	WARNING: There is no way to know how long the buffer passed to 
	wgetstr() is, so it is possible to overwrite wrong memory or 
	code!! This is the reason for the wgetnstr() function :-)

  X/Open Return Value:
	This functions return ERR on failure or any other value on 
	success.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	getstr					Y	Y	Y
	wgetstr					Y	Y	Y
	mvgetstr				Y	Y	Y
	mvwgetstr				Y	Y	Y
	getnstr					-	-      4.0
	wgetnstr				-	-      4.0
	mvwgetnstr				-	-     ncurses

**man-end****************************************************************/

#define MAXLINE 255

int getstr(char *str)
{
	PDC_LOG(("getstr() - called\n"));

	return wgetnstr(stdscr, str, MAXLINE);
}

int wgetstr(WINDOW *win, char *str)
{
	PDC_LOG(("wgetstr() - called\n"));

	return wgetnstr(win, str, MAXLINE);
}

int mvgetstr(int y, int x, char *str)
{
	PDC_LOG(("mvgetstr() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wgetnstr(stdscr, str, MAXLINE);
}

int mvwgetstr(WINDOW *win, int y, int x, char *str)
{
	PDC_LOG(("mvwgetstr() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wgetnstr(win, str, MAXLINE);
}

int getnstr(char *str, int n)
{
	PDC_LOG(("getnstr() - called\n"));

	return wgetnstr(stdscr, str, n);
}

int wgetnstr(WINDOW *win, char *str, int n)
{
	int ch, i, num, x, chars;
	char *p;
	bool stop, oldecho, oldcbreak, oldnodelay;

	PDC_LOG(("wgetnstr() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	chars = 0;
	p = str;
	stop = FALSE;

	x = win->_curx;

	oldcbreak = SP->cbreak;		/* remember states */
	oldecho = SP->echo;
	oldnodelay = win->_nodelay;

	SP->echo = FALSE;		/* we do echo ourselves */
	cbreak();			/* ensure each key is returned 
					   immediately */
	win->_nodelay = FALSE;		/* don't return -1 */

	wrefresh(win);

	while (!stop)
	{
		ch = wgetch(win);

		/* ignore modifier keys on their own */

		if (ch == KEY_SHIFT_L   || ch == KEY_SHIFT_R ||
		    ch == KEY_CONTROL_L || ch == KEY_CONTROL_R ||
		    ch == KEY_ALT_L     || ch == KEY_ALT_R)
		         continue;

		ch = ch & A_CHARTEXT;

		switch (ch)
		{

		case '\t':
			ch = ' ';
			num = TABSIZE - (win->_curx - x) % TABSIZE;
			for (i = 0; i < num; i++)
			{
				if (chars < n)
				{
					if (oldecho) 
						waddch(win, ch);
					*p++ = ch;
					++chars;
				}
				else
					beep();
			}
			break;

		case _ECHAR:		/* CTRL-H -- Delete character */
			if (p > str)
			{
				if (oldecho) 
					waddstr(win, "\b \b");
				ch = *--p;
				if ((ch < ' ') && (oldecho))
					waddstr(win, "\b \b");
				chars--;
			}
			break;

		case _DLCHAR:		/* CTRL-U -- Delete line */
			while (p > str)
			{
				if (oldecho) 
					waddstr(win, "\b \b");
				ch = *--p;
				if ((ch < ' ') && (oldecho))
					waddstr(win, "\b \b");
			}
			chars = 0;
			break;

		case _DWCHAR:		/* CTRL-W -- Delete word */

			while ((p > str) && (*(p - 1) == ' '))
			{
				if (oldecho) 
					waddstr(win, "\b \b");

				--p;	/* remove space */
				chars--;
			}
			while ((p > str) && (*(p - 1) != ' '))
			{
				if (oldecho) 
					waddstr(win, "\b \b");

				ch = *--p;
				if ((ch < ' ') && (oldecho))
					waddstr(win, "\b \b");
				chars--;
			}
			break;

		case '\n':
		case '\r':
			stop = TRUE;
			if (oldecho) 
				waddch(win, '\n');
			break;

		default:
			if (chars < n)
			{
				*p++ = ch;
				if (oldecho) 
					waddch(win, ch);
				chars ++;
			}
			else
				beep();

			break;
      
		}
		wrefresh(win);
	}

	*p = '\0';

	SP->echo = oldecho;		/* restore old settings */
	SP->cbreak = oldcbreak;
	win->_nodelay = oldnodelay;

	return OK;
}

int mvgetnstr(int y, int x, char *str, int n)
{
	PDC_LOG(("mvgetnstr() - called\n"));

	if (move(y, x) == ERR)
		return ERR;

	return wgetnstr(stdscr, str, n);
}

int mvwgetnstr(WINDOW *win, int y, int x, char *str, int n)
{
	PDC_LOG(("mvwgetnstr() - called\n"));

	if (wmove(win, y, x) == ERR)
		return ERR;

	return wgetnstr(win, str, n);
}
