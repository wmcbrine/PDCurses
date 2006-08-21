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

#include <string.h>

RCSID("$Id: pdcsetsc.c,v 1.24 2006/08/21 04:29:46 wmcbrine Exp $");

int PDC_curs_set(int visibility)
{
	int ret_vis = SP->visibility;

	PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", 
		visibility));

	if (visibility != -1)
		SP->visibility = visibility;

	PDC_display_cursor(SP->cursrow, SP->curscol, SP->cursrow, 
		SP->curscol, visibility);

	return ret_vis;
}

/*man-start**************************************************************

  PDC_set_title()	- Set window title

  PDCurses Description:
	Sets the title of the window in which the curses program is 
	running. This function may not do anything on some platforms.

  PDCurses Return Value:
	N/A

  Portability:
	PDCurses  void PDC_set_title(const char *title);

**man-end****************************************************************/

void PDC_set_title(const char *title)
{
	int len;

	PDC_LOG(("PDC_set_title() - called:<%s>\n", title));

	len = strlen(title) + 1;		/* write nul character */

	XCursesInstruct(CURSES_TITLE);

	if (XC_write_display_socket_int(len) >= 0)
		if (XC_write_socket(xc_display_sock, title, len) >= 0)
			return;

	XCursesExitCursesProcess(1, "exiting from PDC_set_title");
}
