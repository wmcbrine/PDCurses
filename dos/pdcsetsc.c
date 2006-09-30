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

#include "pdcdos.h"

RCSID("$Id: pdcsetsc.c,v 1.29 2006/09/30 15:45:04 wmcbrine Exp $");

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
