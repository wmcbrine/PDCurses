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
#  include <config.h>
#endif
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef	insch
#undef	winsch
#undef	mvinsch
#undef	mvwinsch

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	refresh
#  undef	wrefresh
#  undef	move
#  undef	wmove
#endif

#ifdef PDCDEBUG
char *rcsid_insch  = "$Id: insch.c,v 1.1 2001/01/10 08:27:10 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         insch

  Synopsis:
  	int insch(chtype ch);
  	int winsch(WINDOW *win, chtype ch);
  	int mvinsch(int y, int x, chtype ch);
  	int mvwinsch(WINDOW *win, int y, int x, chtype ch);

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
 	and insch().

 	NOTE: All these functions are implemented as macros.

  PDCurses Description:
 	Depending upon the state of the raw character output, 7- or
 	8-bit characters will be output.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      insch                                 Y        Y       Y
      winsch                                Y        Y       Y
      mvinsch                               Y        Y       Y
      mvwinsch                              Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	insch(chtype ch)
#else
int	PDC_CDECL	insch(ch)
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("insch() - called\n");
#endif

	return( PDC_chins( stdscr, ch, (bool)(!(SP->raw_out))) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	winsch(WINDOW *win, chtype ch)
#else
int	PDC_CDECL	winsch(win,ch)
WINDOW *win;
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("winsch() - called\n");
#endif

	return( PDC_chins( win, ch, (bool)(!(SP->raw_out))) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvinsch(int y, int x, chtype ch)
#else
int	PDC_CDECL	mvinsch(y,x,ch)
int y;
int x;
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvinsch() - called\n");
#endif
	if (move(y,x) == ERR)
		return(ERR);
	return( PDC_chins( stdscr, ch, (bool)(!(SP->raw_out))) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvwinsch(WINDOW *win, int y, int x, chtype ch)
#else
int	PDC_CDECL	mvwinsch(win,y,x,ch)
WINDOW *win;
int y;
int x;
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvwinsch() - called\n");
#endif
	if (wmove(win,y,x) == ERR)
		return(ERR);
	return( PDC_chins( win, ch, (bool)(!(SP->raw_out))) );
}
