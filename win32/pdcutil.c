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
#define INCLUDE_WINDOWS_H
#include <curses.h>

RCSID("$Id: pdcutil.c,v 1.3 2006/07/07 05:34:05 wmcbrine Exp $");

void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));

/*	MessageBeep(MB_OK); */
	MessageBeep(0XFFFFFFFF);
}

/*man-start**************************************************************

  PDC_sysname()			- Platform/card name

  PDCurses Description:
	This is a private PDCurses routine.

	The middle part of the long terminal description -- platform 
	name, plus card type on DOS and OS/2. Called by longname().
	Note that p must point to an already-allocated buffer.

  Portability:
	PDCurses  char *PDC_sysname(char *p);

**man-end****************************************************************/

char *PDC_sysname(char *p)
{
	PDC_LOG(("PDC_sysname() - called\n"));

	p += sprintf(p, "Win32");

	return p;
}

void PDC_napms(int ms)
{
	PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

	Sleep(ms);
}
