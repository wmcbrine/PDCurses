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
#undef	clearok
#undef	idlok
#undef	idcok
#undef	immedok
#undef	leaveok
#undef	setscrreg
#undef	wsetscrreg
#undef	scrollok
#undef	nl
#undef	nonl
#undef	raw_output

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_outopts  = "$Id: outopts.c,v 1.1 2001/01/10 08:27:17 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                       outopts

  Synopsis:
  	int clearok(WINDOW *win, bool bf);
  	int idlok( WINDOW *win, bool bf );
  	int idcok( WINDOW *win, bool bf );
  	int immedok( WINDOW *win, bool bf );
  	int leaveok( WINDOW *win, bool bf );
  	int setscrreg(int top, int bot);
  	int wsetscrreg(WINDOW *win, int top, int bot);
  	int scrollok(WINDOW *win, bool bf);
  	int nl(void);
  	int nonl(void);

  	int raw_output(bool bf);

  X/Open Description:
 	With the clearok() routine, if bf is TRUE, the next call to 
 	wrefresh() with this window will clear the screen completely 
 	and redraw the entire screen.

 	If idlok() is enabled (bf is TRUE), curses will use the insert/delete
 	line feature of terminals so equipped.  If disabled, curses will not
 	use this feature.  (The insert/delete character feature is always
 	used.)  This option should be enabled only if the application
 	needs insert/delete line; for example, for a screen editor.  It
 	is disabled by default because insert/delete line tends to be
 	visually annoying when used in applications where it isn't really
 	needed.  If insert/delete line cannot be used, curses will redraw
 	the changed portions of all lines.

 	With the idcok() routine, if enabled (bf is TRUE), curses considers
 	using the hardware insert/delete character feature of terminals so 
 	equipped. This is enabled by default.

 	The immedok() routine, called with a second argument of TRUE, 
 	causes an automatic wrefrsh() to be called on the window every time
 	a change is made to that window, due to calls like; waddch(),
 	wclrtoeol(), etc... Not surprisingly, this causes a severe performance
 	overhead.

 	Normally, the hardware cursor is left at the location of the
 	window being refreshed.  leaveok() allows the cursor to be
 	left wherever the update happens to leave it.  It is useful
 	for applications where the cursor is not used, since it reduces
 	the need for cursor motions.  If possible, the cursor is made
 	invisible when this option is enabled.

 	The setscrreg() and wsetscrreg() functions allow the user to set 
 	a software scrolling region in a window.  The parameters 'top' 
 	and 'bot' are the line numbers of the top and bottom margin of the 
 	scrolling region.  (Line 0 is the top line of the window.)  If this 
 	option and scrollok() are enabled, an attempt to move off the bottom 
 	margin will cause all lines in the scrolling region to scroll up one 
 	line.  Only the text of the window is scrolled.

 	The nl() function enables the translation of newline into a
 	carriage return and a line-feed on output, and a carriage return
 	is translated into a newline on input.  Initially, the translations
 	do occur.  By disabling these translations, curses is able to
 	make better use of the line-feed capability, resulting in faster
 	cursor motion.

 	The nonl() function disables the translation of newline into a
 	carriage return and a line-feed on output, and a carriage return
 	is translated into a newline on input.  Initially, the translations
 	do occur.  By disabling these translations, curses is able to
 	make better use of the line-feed capability, resulting in faster
 	cursor motion.

  PDCurses Description:
 	idlok() and idcok() are provided for portability.  
 	This may have some value for the FLEXOS platform, but there is 
 	no code in PDCurses at this time (910725) that takes advantage 
 	of this.

 	Though not explicitly stated, the cursor will be made visible
 	again, if leaveok() is passed a FALSE value.
 			(Frotz:911226 Comments?)

 	The raw_output() routine enables the output of raw characters using 
 	the 'standard' *add* and *ins* curses functions.
 	To force 8 bit output and no translation of control characters,
 	invoke raw_output() with bf as TRUE.  To force 7 bit output and
 	translation of control characters, invoke raw_output() with bf
 	as FALSE.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      clearok                               Y        Y       Y
      idlok                                 Y        Y       Y
      idcok                                 -        -      4.0
      immedok                               -        -      4.0
      leaveok                               Y        Y       Y
      setscrreg                             Y        Y       Y
      wsetscrreg                            Y        Y       Y
      scrollok                              Y        Y       Y
      nl                                    Y        Y       Y
      nonl                                  Y        Y       Y
      raw_output                            -        -       -

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	clearok(WINDOW *win, bool bf)
#else
int	PDC_CDECL	clearok(win,bf)
WINDOW *win;
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("clearok() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	win->_clear = bf;
#if 0
	if( bf )
		touchwin( win );
#endif

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	idlok( WINDOW *win, bool bf )
#else
int	PDC_CDECL	idlok(win,bf)
WINDOW *win;
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("idlok() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	win->_use_idl = bf;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	idcok( WINDOW *win, bool bf )
#else
int	PDC_CDECL	idcok(win,bf)
WINDOW *win;
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("idcok() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	win->_use_idc = bf;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	immedok( WINDOW *win, bool bf )
#else
int	PDC_CDECL	immedok(win,bf)
WINDOW *win;
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("immedok() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	win->_immed = bf;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	leaveok( WINDOW *win, bool bf )
#else
int	PDC_CDECL	leaveok(win,bf)
WINDOW *win;
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("leaveok() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( ERR );

	if  ((win->_leaveit = bf) != 0)	PDC_cursor_off();
	else				PDC_cursor_on();
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	setscrreg(int top, int bottom)
#else
int	PDC_CDECL	setscrreg(top,bottom)
int top;
int bottom;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("setscrreg() - called: top %d bottom %d\n",top,bottom);
#endif

	if (stdscr == (WINDOW *)NULL)
		return (ERR);

	if ((0 <= top) &&
	    (top <= stdscr->_cury) &&
	    (stdscr->_cury <= bottom) &&
	    (bottom < LINES))
	{
		stdscr->_tmarg = top;
		stdscr->_bmarg = bottom;
		return (OK);
	}
	else
	{
		return (ERR);
	}
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	wsetscrreg(WINDOW *win, int top, int bottom)
#else
int	PDC_CDECL	wsetscrreg(win,top,bottom)
WINDOW *win;
int top;
int bottom;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wsetscrreg() - called: top %d bottom %d\n",top,bottom);
#endif

	if (win == (WINDOW *)NULL)
		return (ERR);

	if ((0 <= top) &&
	    (top <= win->_cury) &&
	    (win->_cury <= bottom) &&
	    (bottom < win->_maxy))
	{
		win->_tmarg = top;
		win->_bmarg = bottom;
		return (OK);
	}
	else
	{
		return (ERR);
	}
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	scrollok(WINDOW *win, bool bf)
#else
int	PDC_CDECL	scrollok(win,bf)
WINDOW *win;
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("scrollok() - called\n");
#endif

	if  (win == (WINDOW *)NULL)
		return( ERR );

	win->_scroll	= bf;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	nl(void)
#else
int	PDC_CDECL	nl()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("nl() - called\n");
#endif

	SP->autocr = TRUE;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	nonl(void)
#else
int	PDC_CDECL	nonl()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("nonl() - called\n");
#endif

	SP->autocr = FALSE;
	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	raw_output(bool bf)
#else
int	PDC_CDECL	raw_output(bf)
bool bf;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("raw_output() - called\n");
#endif

	SP->raw_out = bf;
	return( OK );
}
