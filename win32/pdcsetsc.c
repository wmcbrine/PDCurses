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
#define	INCLUDE_WINDOWS_H
#include <curses.h>

RCSID("$Id: pdcsetsc.c,v 1.22 2006/07/23 16:51:14 wmcbrine Exp $");

extern HANDLE hConOut;

/*man-start**************************************************************

  PDC_set_font()	- sets the current font size

  PDCurses Description:
	This is a private PDCurses function.

	This routine sets the current font size, if the adapter allows
	such a change.

  PDCurses Return Value:
	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
	It is an error to attempt to change the font size on a "bogus"
	adapter.  The reason for this is that we have a known video
	adapter identity problem.  e.g. Two adapters report the same
	identifying characteristics.

  Portability:
	PDCurses  int PDC_set_font(int size);

**man-end****************************************************************/

int PDC_set_font(int size)
{
	PDC_LOG(("PDC_set_font() - called\n"));

	return OK;
}

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
		cci.dwSize = 25;
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
