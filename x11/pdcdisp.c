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

#include <string.h>

#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif

#ifndef HAVE_MEMMOVE
#  define memmove PDC_memmove
#endif

#ifdef PDCDEBUG
char *rcsid_PDCdisp  = "$Id: pdcdisp.c,v 1.2 2001/01/10 08:30:06 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_clr_update()	- Updates the screen with a full redraw.

  PDCurses Description:
 	Updates the screen by clearing it and then redraw it in its
 	entirety. If SP->refrbrk is TRUE, and there is pending
 	input characters, the update will be prematurely terminated.

  PDCurses Return Value:
 	This routine returns ERR if it is unable to accomplish it's task.
 	This return value is ONLY under FLEXOS.

 	The return value OK is returned if there were no errors.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_clr_update( WINDOW* s );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_clr_update(WINDOW *s)
#else
int	PDC_clr_update(s)
WINDOW *s;
#endif
/***********************************************************************/
{
register int	i=0;
	WINDOW*	w=NULL;
	bool rc=FALSE;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_clr_update() - called\n");
#endif

	w = curscr;
	if (w == (WINDOW *)NULL)
		return( ERR );

	s->_clear = FALSE;
	for (i = 0; i < LINES; i++)	/* update physical screen */
	{
		if (s != w)	/* copy s to curscr */
			memcpy(w->_y[i], s->_y[i], COLS * sizeof(chtype));
		XCurses_transform_line(w->_y[i],i,0,COLS);

		if (SP->refrbrk && (SP->cbreak || SP->raw_inp)) 
		{
			rc = PDC_breakout();
			if(rc) 
				break;
		}
		w->_firstch[i] = _NO_CHANGE;
		w->_lastch[i] = _NO_CHANGE;
	}
	return( OK );
}

/*man-start*********************************************************************

  PDC_cursor_on()	- Turns on the hardware cursor.

  PDCurses Description:
 	Turns on the hardware curses, it does nothing if it is already on.

  PDCurses Return Value:
 	Returns OK upon success, ERR upon failure.

  Portability:
 	PDCurses	int PDC_cursor_on( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_cursor_on(void)
#else
int	PDC_cursor_on()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_cursor_on() - called\n");
#endif
	return( OK );
}

/*man-start*********************************************************************

  PDC_cursor_off()	- Turns off the hardware cursor.

  PDCurses Description:
 	Turns off the hardware curses, it does nothing if it is already off.

  PDCurses Return Value:
 	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
 	ERR will be returned (in the case of FLEXOS) if the hardware cursor
 	can not be disabled.

  Portability:
 	PDCurses	int PDC_cursor_off( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_cursor_off(void)
#else
int	PDC_cursor_off()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_cursor_off() - called\n");
#endif
	return( OK );
}

/*man-start*********************************************************************

  PDC_gotoxy()	- position hardware cursor at (x, y)

  PDCurses Description:
 	This is a private PDCurses routine.

 	Moves the physical cursor to the desired address on the
 	screen. We don't optimize here -- on a PC, it takes more time
 	to optimize than to do things directly.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_gotoxy( int row, int col );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_gotoxy(int row, int col)
#else
int	PDC_gotoxy(row,col)
int row;
int col;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_gotoxy() - called: row %d col %d\n",row,col);
#endif

	XCurses_display_cursor(SP->cursrow,SP->curscol,row,col,SP->visibility);
	return(OK);
}

/*man-start*********************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
 	This is a private PDCurses function.

 	Updates the given physical line to look like the corresponding
 	line in _curscr.

  PDCurses Return Value:
 	This routine returns TRUE if a premature refresh end
 	is allowed, and there is an input character pending.  Otherwise,
 	FALSE is returned.

  PDCurses Errors:
 	No errors are defined for this routine.

  Portability:
 	PDCurses	bool	PDC_transform_line( int lineno );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_transform_line(register int lineno)
#else
bool	PDC_transform_line(lineno)
register int lineno;
#endif
/***********************************************************************/
{
	register chtype*	dstp=NULL;
	int	x=0;
	int	endx=0;
	int	len=0;
	bool rc=FALSE;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_transform_line() - called: line %d\n",lineno);
#endif

	if (curscr == (WINDOW *)NULL)
		return( FALSE );

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	dstp = curscr->_y[lineno] + x;
	len = endx-x+1;

		XCurses_transform_line(dstp,lineno,x,len);

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;

	if (SP->refrbrk && (SP->cbreak || SP->raw_inp)) 
	{
		rc = PDC_breakout();
		if(rc) 
			return(TRUE);
	}
	return(FALSE);
}
