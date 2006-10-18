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
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcdos.h"

RCSID("$Id: pdcsetsc.c,v 1.32 2006/10/18 22:35:11 wmcbrine Exp $");

int PDC_curs_set(int visibility)
{
	PDCREGS regs;
	int ret_vis, start, end;

	PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

	ret_vis = SP->visibility;
	SP->visibility = visibility;

	switch(visibility)
	{
		case 0:  /* invisible */
			start = 32;
			end = 0;  /* was 32 */
			break;
		case 2:  /* highly visible */
			start = 0;   /* full-height block */
			end = 7;
			break;
		default:  /* normal visibility */
			start = (SP->orig_cursor >> 8) & 0xff;
			end = SP->orig_cursor & 0xff;
	}

	/* if scrnmode is not set, some BIOSes hang */

	regs.h.ah = 0x01;
	regs.h.al = (unsigned char)pdc_scrnmode; 
	regs.h.ch = (unsigned char)start;
	regs.h.cl = (unsigned char)end;
	PDCINT(0x10, regs);

	return ret_vis;
}

/*man-start**************************************************************

  PDC_set_title()	- Set window title

  PDCurses Description:
	Sets the title of the window in which the curses program is 
	running. This function does not do anything on DOS.

  PDCurses Return Value:
	N/A

  Portability:
	PDCurses  void PDC_set_title(const char *title);

**man-end****************************************************************/

void PDC_set_title(const char *title)
{
	PDC_LOG(("PDC_set_title() - called: <%s>\n", title));
}

int PDC_set_blink(bool blinkon)
{
	PDCREGS regs;

	switch (pdc_adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
	case _VGACOLOR:
	case _VGAMONO:
		regs.W.ax = 0x1003;
		regs.W.bx = blinkon;

		PDCINT(0x10, regs);

		if (pdc_color_started)
			COLORS = blinkon ? 8 : 16;

		break;
	default:
		COLORS = 8;
	}

	return (COLORS - (blinkon * 8) != 8) ? OK : ERR;
}
