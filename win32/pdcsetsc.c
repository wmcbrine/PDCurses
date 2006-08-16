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

#ifdef PDC_WIDE
# define UNICODE
#endif

#include <windows.h>
#undef MOUSE_MOVED
#define CURSES_LIBRARY 1
#include <curses.h>

RCSID("$Id: pdcsetsc.c,v 1.27 2006/08/16 17:47:49 wmcbrine Exp $");

extern HANDLE hConOut;

int PDC_curs_set(int visibility)
{
	CONSOLE_CURSOR_INFO cci;
	int ret_vis;

	PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

	ret_vis = SP->visibility;

	if (GetConsoleCursorInfo(hConOut,&cci) == FALSE)
		return ERR;

	switch(visibility)
	{
	case 0:				/* invisible */
		cci.bVisible = FALSE;
		break;
	case 2:				/* highly visible */
		cci.bVisible = TRUE;
		cci.dwSize = 95;
		break;
	default:			/* normal visibility */
		cci.bVisible = TRUE;
#ifdef PDC_OLDCURSOR
		cci.dwSize = 25;
#else
		cci.dwSize = SP->orig_cursor;
#endif
		break;
	}

	if (SetConsoleCursorInfo(hConOut,&cci) == FALSE)
		return ERR;

	SP->visibility = visibility;
	return ret_vis;
}

/*man-start**************************************************************

  PDC_set_title()	- Set window title

  PDCurses Description:
	Sets the title of the window in which the curses program is 
	running. This function may not do anything on some platforms.

  Portability:
	PDCurses  void PDC_set_title(const char *title);

**man-end****************************************************************/

void PDC_set_title(const char *title)
{
	PDC_LOG(("PDC_set_title() - called:<%s>\n", title));

	SetConsoleTitleA(title);
	return;
}
