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
#undef	bkgd
#undef	bkgdset
#undef	wbkgd
#undef	wbkgdset

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_bkgd  = "$Id: bkgd.c,v 1.1 2001/01/10 08:26:50 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          bkgd

  Synopsis:
  	int bkgd(chtype ch);
  	void bkgdset(chtype ch);
  	int wbkgd(WINDOW *win, chtype ch);
  	void wbkgdset(WINDOW *win, chtype ch);

  UNIX System V (Rel 3.2 or 4) Description:
 	The bkgdset() and wbkgdset() routines manipulate the backgound of 
 	the named window.  Background is a chtype consisting of any combination 
 	of attributes and non-blank characters that are written into the window
 	with waddch().  Both the character and attribute parts of the background
 	are combined with the blank characters.  The background becomes a 
 	property of the character and moves with the character through any 
 	scrolling and insert/delete line/character operations.  To the extent
 	possible on a particular terminal, the attribute part of the background
 	is displayed as the graphic rendition of the character put on the
 	screen.

 	The bkgd() and wbkgd() routines combine the new background with every
 	position in the window.  Background is any combination of attributes
 	and a character.  Only the attribute part is used to set the background
 	of non-blank characters, while both character and attributes are used
 	for blank positions.  To the extent possible on a particular terminal,
 	the attribute part of the background is displayed as the graphic
 	rendition of the character put on the screen.

  PDCurses Description:
 	The attributes that are defined with the attrset()/attron() set of 
 	functions take precedence over the background attributes if there is a
 	conflict (e.g., different color pairs).

  System V Return Value:
 	bkgd() and wbkgd() return the integer OK, or a non-negative integer, if
 	immedok() is set (although this is likely NOT the case).

  PDCurses Return Value:
 	bkgd() and wbkgd() return the integer OK ( the function immedok() is
 	currently not supported ).

  System V Notes:
 	Note that bkgdset() and bkgd() may be macros.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      bkgd                                  -        -      4.0
      bkgdset                               -        -      4.0
      wbkgd                                 -        -      4.0
      wbkgdset                              -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	bkgd(chtype ch)
#else
int	PDC_CDECL	bkgd(ch)
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("bkgd() - called\n");
#endif
	return(wbkgd(stdscr,ch));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wbkgd(WINDOW *win, chtype ch)
#else
int	PDC_CDECL	wbkgd(win,ch)
WINDOW *win;
chtype ch;
#endif
/***********************************************************************/
{
	int x=0, y=0;
	chtype oldcolr=0, oldattr=0, oldch=0, newcolr=0, newattr=0, newch=0, colr=0, attr=0;
	chtype *winptr;
#ifdef NDP
	char tmpbuf[60];
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wbkgd() - called\n");
#endif

	if (win == NULL)
		return(ERR);

	if( win->_bkgd == ch )
		return OK;

	oldcolr = (win->_bkgd & A_COLOR);
	if( oldcolr != 0 )
		oldattr = ((win->_bkgd & A_ATTRIBUTES) ^ oldcolr);
	oldch   = (win->_bkgd & A_CHARTEXT);

	wbkgdset( win, ch );

	newcolr = (win->_bkgd & A_COLOR);
	if( newcolr != 0 )
		newattr = ((win->_bkgd & A_ATTRIBUTES) ^ newcolr);
	newch   = (win->_bkgd & A_CHARTEXT);

#ifdef NDP
/* buggy compiler fix */
	sprintf( tmpbuf, "%d %d", oldch, newch );
#endif
	
/* what follows is what seems to occur in the System V implementation of */
/* this routine */

	for ( y = 0; y < win->_maxy; y++ ) {
		for( x = 0; x < win->_maxx; x++ ) {

			winptr = win->_y[y] + x;

			ch = *winptr;

		/* determine the colors and attributes of the character */
		/* read from the window */

			colr = (ch & A_COLOR);
			attr = (ch & A_ATTRIBUTES);
			if( colr != 0 )
				attr = (attr ^ colr);

		/* if the color is the same as the old background color, then */
		/* make it the new background color, otherwise leave it */

			if( colr == oldcolr )
				colr = newcolr;

		/* remove any attributes (non color) from the character that */
		/* were part of the old background, then combine the remaining */
		/* ones with the new background */

			attr = (attr ^ oldattr);
			attr |= newattr;

		/* change character if it is there because it was the old */
		/* background character */

			ch = (ch & A_CHARTEXT);
			if ( ch == oldch )
				ch = newch;

			ch = ((ch | attr) | colr);

			*winptr = ch;

		}
	}

	touchwin(win);
	PDC_sync(win);
	return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
void	PDC_CDECL	bkgdset(chtype ch)
#else
void	PDC_CDECL	bkgdset(ch)
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("bkgdset() - called\n");
#endif
	wbkgdset(stdscr,ch);
}
/***********************************************************************/
#ifdef HAVE_PROTO
void	PDC_CDECL	wbkgdset(WINDOW *win, chtype ch)
#else
void	PDC_CDECL	wbkgdset(win,ch)
WINDOW *win;
chtype ch;
#endif
/***********************************************************************/
{
	chtype bkgdattr;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wbkgdset() - called\n");
#endif

	if (win == NULL)
		return;

	if( win->_bkgd == ch )
		return;

	if ( (ch & A_ATTRIBUTES) == 0 )
		bkgdattr = A_NORMAL;
	else
		bkgdattr = (ch & A_ATTRIBUTES);

	ch = (ch & A_CHARTEXT);
	
	if( ch == 0 )
		ch = ' ';

	win->_bkgd = (ch | bkgdattr);
}
