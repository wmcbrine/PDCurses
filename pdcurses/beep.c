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
#undef beep
#undef flash

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef wrefresh
#endif

#ifdef PDCDEBUG
const char *rcsid_beep =
	"$Id: beep.c,v 1.18 2006/03/26 01:48:52 wmcbrine Exp $";
#endif

/*man-start**************************************************************

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

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	beep					Y	Y	Y
	flash					Y	Y	Y

**man-end****************************************************************/

int beep(void)
{
	PDC_LOG(("beep() - called\n"));

	if (!SP->audible)
	{
		flash();
		return ERR;		/* We try to flash instead...*/
	}

	PDC_beep();

	return OK;
}

int flash(void)
{
	PDC_LOG(("flash() - called\n"));

#ifdef XCURSES
	XCursesInstructAndWait(CURSES_FLASH);
#else
	PDC_scroll(0, 0, LINES - 1, COLS - 1, 0, A_NORMAL);
	napms(50);
	PDC_scroll(0, 0, LINES - 1, COLS - 1, 0, A_REVERSE);
	wrefresh(curscr);
#endif
	return OK;
}
