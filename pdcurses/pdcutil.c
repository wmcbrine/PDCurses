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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifndef XCURSES
# define INCLUDE_WINDOWS_H
#endif
#include <curses.h>

RCSID("$Id: pdcutil.c,v 1.31 2006/03/29 20:06:41 wmcbrine Exp $");

void PDC_beep(void)
{
#if defined(XCURSES)
	XCursesInstruct(CURSES_BELL);

#elif defined(DOS)
	PDC_putctty((chtype) '\007', 0);

#elif defined(OS2)
# if defined(EMXVIDEO)
	putchar('\007');
# else
	DosBeep(1380, 100);
# endif

#elif defined(WIN32)
/*	MessageBeep(MB_OK); */
	MessageBeep(0XFFFFFFFF);
#endif
}

/*man-start**************************************************************

  PDC_breakout()	- check for type-ahead

  X/Open Description:
	The curses package does the "line-breakout optimisation" by
	looking for type-ahead periodically while updating the screen.
	If input is found, the current update will be postponed until
	refresh() or doupdate() are called again.  This allows faster
	response to commands typed in advance.  Normally, the input FILE
	pointer passed to newterm(), or stdin in the case when initscr()
	was called, will be used to do this type-ahead checking.  This 
	routine will do the actual check for PDcurses to see if input is 
	pending.

  PDCurses Description:
	This routine does what the PDcurses typeahead routine used to 
	do.  But to make that routine consistent with its System V 
	counterpart, all it does now is set whether or not typeahead 
	checking is performed.

  X/Open Return Value:
	The PDC_breakout() routine returns TRUE if keyboard input is 
	pending otherwise FALSE is returned.

  Portability:
	PDCurses  bool PDC_breakout(void);

**man-end****************************************************************/

bool PDC_breakout(void)
{
	extern int c_pindex;	/* putter index */
	extern int c_gindex;	/* getter index */
	extern int c_ungind;	/* wungetch() push index */
	bool rc;

	PDC_LOG(("PDC_breakout() - called\n"));

	/* ungotten or buffered char */
	rc = (c_ungind) || (c_pindex > c_gindex);

	if (!rc)
		rc = PDC_check_bios_key();

	PDC_LOG(("PDC_breakout() - rc %d c_ungind %d c_pindex %d c_gindex %d\n",
		 rc, c_ungind, c_pindex, c_gindex));

	return rc;
}
