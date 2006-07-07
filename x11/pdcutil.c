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

#include "pdcx11.h"

#if defined(HAVE_POLL) && !defined(HAVE_USLEEP)
# include <poll.h>
#endif

RCSID("$Id: pdcutil.c,v 1.1 2006/07/07 02:53:08 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_beep()			- Low-level beep function

  PDCurses Description:
	This is a private PDCurses routine.

	Generates a "beep" sound or flashes the screen. Called by 
	beep().

  Portability:
	PDCurses  void PDC_beep(void);

**man-end****************************************************************/

void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));

	XCursesInstruct(CURSES_BELL);
}

/* napms() is documented in ../pdcurses/kernel.c */

int napms(int ms)
{
	PDC_LOG(("napms() - called: ms=%d\n", ms));

#if defined(HAVE_USLEEP)

	usleep(1000 * ms);

#elif defined(HAVE_POLL)
	{
		struct pollfd fd;
		fd.fd = -1;
		fd.events = 0;
		poll(&fd, 1, ms);
	}
#endif
	return OK;
}
