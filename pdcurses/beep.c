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

#ifdef UNIX
#include <defs.h>
#include <term.h>
#endif

/* undefine any macros for functions defined in this module */
#undef	beep
#undef	flash

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	delay_output
#  undef	wrefresh
#endif

#ifdef PDCDEBUG
char *rcsid_beep  = "$Id: beep.c,v 1.1 2001/01/10 08:26:49 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          beep

  Synopsis:
  	int beep(void);
  	int flash(void);

  X/Open Description:
 	These routines are used to signal the terminal user.  The beep()
 	function will sound the audible bell on the terminal, if possible
 	and if not, will flash the screen (visible bell), if possible.
 	The flash() function will flash the screen, and if that is not
 	possible, will sound the audible signal.  If neither signal is
 	possible, nothing will happen.  Nearly all terminals have an
 	audible signal (bell or beep), but only some can flash the screen.

  X/Open Return Value:
 	These functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for these functions.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      beep                                  Y        Y       Y
      flash                                 Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	beep(void)
#else
int	PDC_CDECL	beep()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("beep() - called\n");
#endif

	if (!SP->audible)
	{
		flash();
		return( ERR );		/* We try to flash instead...*/
	}

	PDC_beep();

	return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	flash(void)
#else
int	PDC_CDECL	flash()
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("flash() - called\n");
#endif

#ifdef UNIX
	if (flash_screen != NULL)
		putp(flash_screen);
	return(OK);
#endif

#if defined(DOS) || defined(OS2) || defined(WIN32)
	PDC_scroll(0, 0, LINES - 1, COLS - 1, 0, A_NORMAL);
	delay_output( 50 );
	PDC_scroll(0, 0, LINES - 1, COLS - 1, 0, A_REVERSE);
	wrefresh(curscr);
	return( OK );
#endif

#if defined(XCURSES)
	XCursesInstructAndWait(CURSES_FLASH);
	return( OK );
#endif
}
