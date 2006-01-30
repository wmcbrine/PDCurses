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

#define	CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef getbegy
#undef getbegx
#undef getmaxy
#undef getmaxx
#undef getpary
#undef getparx

#ifdef PDCDEBUG
const char *rcsid_getyx =
	"$Id: getyx.c,v 1.9 2006/01/30 12:17:17 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                         getyx

  Synopsis:
	void getyx(WINDOW *win, int y, int x);
	void getparyx(WINDOW *win, int y, int x);
	void getbegyx(WINDOW *win, int y, int x);
	void getmaxyx(WINDOW *win, int y, int x);

	int getbegy(WINDOW *win);
	int getbegx(WINDOW *win);
	int getpary(WINDOW *win);
	int getparx(WINDOW *win);
	int getmaxy(WINDOW *win);
	int getmaxx(WINDOW *win);

  X/Open Description:
	With the getyx() macro, the cursor position of the window is placed
	in the two integer variables y and x. getbegyx() and getmaxyx()
	return the current beginning coordinates and size of the specified
	window respectively. getparyx() returns the beginning coordinates 
	of the parent's window if the specified window is a sub-window
	otherwise -1 is returned.

	These functions are implemented as macros, so their return values
	are undefined. Also as these functions are implemented as macros,
	no '&' is required before the variables y and x.

	The functions getbegy(), getbegx(), getmaxy(), getmaxx() ,
	getpary(), and getparx() do return the beginning coordinate or 
	size values, so they can be specified on the right-hand side of 
	an assignment statement. They are, however, implemented as macros.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	getyx					Y	Y	Y
	getparyx				-	-      4.0
	getbegyx				-	-      3.0
	getmaxyx				-	-      3.0
	getbegy					-	-	-
	getbegx					-	-	-
	getpary					-	-	-
	getparx					-	-	-
	getmaxy					-	-	-
	getmaxx					-	-	-

**man-end**********************************************************************/

int PDC_CDECL getbegy(WINDOW *win)
{
	PDC_LOG(("getbegy() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return win->_begy;
}

int PDC_CDECL getbegx(WINDOW *win)
{
	PDC_LOG(("getbegx() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return win->_begx;
}

int PDC_CDECL getpary(WINDOW *win)
{
	PDC_LOG(("getpary() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return win->_pary;
}

int PDC_CDECL getparx(WINDOW *win)
{
	PDC_LOG(("getparx() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return win->_parx;
}

int PDC_CDECL getmaxy(WINDOW *win)
{
	PDC_LOG(("getmaxy() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return win->_maxy;
}

int PDC_CDECL getmaxx(WINDOW *win)
{
	PDC_LOG(("getmaxx() - called\n"));

	if (win == (WINDOW *)NULL)
		return ERR;

	return win->_maxx;
}
