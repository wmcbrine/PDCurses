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

RCSID("$Id: pdcutil.c,v 1.32 2006/03/29 21:39:42 wmcbrine Exp $");

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

  PDCurses Description:
	Check if input is pending, either directly from the keyboard,
	or previously buffered.

  PDCurses Return Value:
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
