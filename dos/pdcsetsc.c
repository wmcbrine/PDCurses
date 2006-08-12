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

RCSID("$Id: pdcsetsc.c,v 1.24 2006/08/12 20:11:36 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_set_80x25()	- force a known screen state: 80x25 text mode.

  PDCurses Description:
	This is a private PDCurses function.

	Forces the appropriate 80x25 alpha mode given the display adapter.

  PDCurses Return Value:
	This function returns OK upon success otherwise ERR is returned.

  Portability:
	PDCurses  int PDC_set_80x25(void);

**man-end****************************************************************/

int PDC_set_80x25(void)
{
	PDC_LOG(("PDC_set_80x25() - called\n"));

	switch (pdc_adapter)
	{
	case _CGA:
	case _EGACOLOR:
	case _EGAMONO:
	case _VGACOLOR:
	case _VGAMONO:
	case _MCGACOLOR:
	case _MCGAMONO:
		regs.h.ah = 0x00;
		regs.h.al = 0x03;
		int86(0x10, &regs, &regs);
		break;
	case _MDA:
		regs.h.ah = 0x00;
		regs.h.al = 0x07;
		int86(0x10, &regs, &regs);
	}

	return OK;
}

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

	if (pdc_bogus_adapter)
		return ERR;

	switch (pdc_adapter)
	{
	case _CGA:
	case _MDA:
	case _MCGACOLOR:
	case _MCGAMONO:
	case _MDS_GENIUS:
		break;

	case _EGACOLOR:
	case _EGAMONO:
		if (SP->sizeable && (pdc_font != size))
		{
			switch (size)
			{
			case _FONT8:
				regs.h.ah = 0x11;
				regs.h.al = 0x12;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			case _FONT14:
				regs.h.ah = 0x11;
				regs.h.al = 0x11;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
			}
		}
		break;

	case _VGACOLOR:
	case _VGAMONO:
		if (SP->sizeable && (pdc_font != size))
		{
			switch (size)
			{
			case _FONT8:
				regs.h.ah = 0x11;
				regs.h.al = 0x12;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			case _FONT14:
				regs.h.ah = 0x11;
				regs.h.al = 0x11;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
				break;
			case _FONT16:
				regs.h.ah = 0x11;
				regs.h.al = 0x14;
				regs.h.bl = 0x00;
				int86(0x10, &regs, &regs);
			}
		}
	}

	curs_set(SP->visibility);

	pdc_font = PDC_get_font();

	return OK;
}

/*man-start**************************************************************

  PDC_set_scrn_mode()	- Set BIOS Video Mode

  PDCurses Description:
	Sets the BIOS Video Mode Number ONLY if it is different from
	the current video mode.  This routine is for DOS systems only.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_set_scrn_mode(int new_mode);

**man-end****************************************************************/

int PDC_set_scrn_mode(int new_mode)
{
	PDC_LOG(("PDC_set_scrn_mode() - called\n"));

	if (PDC_get_scrn_mode() != new_mode)
	{
		regs.h.ah = 0;
		regs.h.al = (unsigned char) new_mode;
		int86(0x10, &regs, &regs);
	}

	pdc_font = PDC_get_font();
	pdc_scrnmode = new_mode;
	LINES = PDC_get_rows();
	COLS = PDC_get_columns();

	return OK;
}

int PDC_curs_set(int visibility)
{
	int ret_vis, start, end = 7;

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
	int86(0x10, &regs, &regs);

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
