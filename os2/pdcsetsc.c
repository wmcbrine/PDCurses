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

#include "pdcos2.h"

#include <string.h>

RCSID("$Id: pdcsetsc.c,v 1.33 2006/10/09 00:11:14 wmcbrine Exp $");

extern int pdc_font;

int PDC_curs_set(int visibility)
{
#ifndef EMXVIDEO
	VIOCURSORINFO pvioCursorInfo;
#endif
	int ret_vis, hidden = 0, start = 0, end = 0;

	PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

	ret_vis = SP->visibility;
	SP->visibility = visibility;

	switch(visibility)
	{
	case 0: 	/* invisible */
#ifdef EMXVIDEO
		start = end = 0;
#else
		start = pdc_font / 4;
		end = pdc_font;
		hidden = -1;
#endif
		break;

	case 2: 	/* highly visible */
		start = 2;		/* almost full-height block */
		end = pdc_font - 1;
		break;

	default:	/* normal visibility */
		start = (SP->orig_cursor >> 8) & 0xff;
		end = SP->orig_cursor & 0xff;
	}

#ifdef EMXVIDEO
	if (!visibility)
		v_hidecursor();
	else
		v_ctype(start, end);
#else
	pvioCursorInfo.yStart = (USHORT)start;
	pvioCursorInfo.cEnd = (USHORT)end;
	pvioCursorInfo.cx = (USHORT)1;
	pvioCursorInfo.attr = hidden;
	VioSetCurType((PVIOCURSORINFO)&pvioCursorInfo, 0);
#endif
	return ret_vis;
}

/*man-start**************************************************************

  PDC_set_title() - Set window title

  PDCurses Description:
	Sets the title of the window in which the curses program is 
	running. This function may not do anything on some platforms.

  PDCurses Return Value:
	N/A

  Portability:
	PDCurses void PDC_set_title(const char *title);

**man-end****************************************************************/

void PDC_set_title(const char *title)
{
	PDC_LOG(("PDC_set_title() - called:<%s>\n", title));
}
