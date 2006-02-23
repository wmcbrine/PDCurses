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
#include <curses.h>

#ifdef PDCDEBUG
const char *rcsid_PDCsetsc =
	"$Id: pdcsetsc.c,v 1.15 2006/02/23 01:46:52 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  PDC_set_80x25()	- force a known screen state: 80x25 text mode.

  PDCurses Description:
	This is a private PDCurses function.

	Forces the appropriate 80x25 alpha mode given the display adapter.

  PDCurses Return Value:
	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  int PDC_set_80x25(void);

**man-end****************************************************************/

int PDC_set_80x25(void)
{
	PDC_LOG(("PDC_set_80x25() - called\n"));

	switch (SP->adapter)
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

  PDC_set_cursor_mode()	- Set the cursor start and stop scan lines.

  PDCurses Description:
	Sets the cursor type to begin in scan line startrow and end in
	scan line endrow.  Both values should be 0-31.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_set_cursor_mode(int startrow, int endrow);

**man-end****************************************************************/

int PDC_set_cursor_mode(int startrow, int endrow)
{
	PDC_LOG(("PDC_set_cursor_mode() - called: startrow %d endrow %d\n",
		startrow, endrow));

	regs.h.ah = 0x01;
	regs.h.ch = (unsigned char) startrow;
	regs.h.cl = (unsigned char) endrow;
	int86(0x10, &regs, &regs);

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

	if (SP->bogus_adapter)
		return ERR;

	switch (SP->adapter)
	{
	case _CGA:
	case _MDA:
	case _MCGACOLOR:
	case _MCGAMONO:
	case _MDS_GENIUS:
		break;

	case _EGACOLOR:
	case _EGAMONO:
		if (SP->sizeable && (SP->font != size))
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
		if (SP->sizeable && (SP->font != size))
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

	if (SP->visible_cursor)
		PDC_cursor_on();
	else
		PDC_cursor_off();

	SP->font = PDC_get_font();

	return OK;
}

/*man-start**************************************************************

  PDC_set_rows()	- sets the physical number of rows on screen

  PDCurses Description:
	This is a private PDCurses function.

	This routine attempts to set the number of rows on the physical
	screen to the passed value.

  PDCurses Return Value:
	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
	It is an error to attempt to change the screen size on a "bogus"
	adapter.  The reason for this is that we have a known video
	adapter identity problem, i.e., two adapters report the same
	identifying characteristics.

  Portability:
	PDCurses  int PDC_set_rows(int rows);

**man-end****************************************************************/

int PDC_set_rows(int rows)
{
	PDC_LOG(("PDC_set_rows() - called\n"));

	if (SP->bogus_adapter)
		return ERR;

	switch (SP->adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
		if (rows < 43)
			PDC_set_font(_FONT14);
		else
			PDC_set_font(_FONT8);
		break;

	case _VGACOLOR:
	case _VGAMONO:
		if (rows < 28)
			PDC_set_font(_FONT16);
		else
			if (rows < 50)
				PDC_set_font(_FONT14);
			else
				PDC_set_font(_FONT8);
	}

	SP->font = PDC_get_font();
	LINES = PDC_get_rows();
	COLS = PDC_get_columns();

	return OK;
}

/*man-start**************************************************************

  PDC_set_scrn_mode()	- Set BIOS Video Mode

  PDCurses Description:
	Sets the BIOS Video Mode Number ONLY if it is different from
	the current video mode.  This routine is for DOS systems only.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

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

	SP->font = PDC_get_font();
	SP->scrnmode = new_mode;
	LINES = PDC_get_rows();
	COLS = PDC_get_columns();

	return OK;
}

int PDC_curs_set(int visibility)
{
	int ret_vis, start = 6, end = 7;

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
#if 0
			start = 2;   /* almost full-height block */
			end = SP->font-1;
#else
			start = 0;   /* full-height block */
#endif
			break;
		default:  /* normal visibility */
			  /* start = 6, end = 7 */
#if 0
			start = SP->font - 2;
			end = SP->font-1;
#endif
			break;
	}

	/* if scrnmode is not set, some BIOSes hang */

	regs.h.ah = 0x01;
	regs.h.al = (unsigned char)SP->scrnmode; 
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

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  void PDC_set_title(char *title);

**man-end****************************************************************/

void PDC_set_title(char *title)
{
	PDC_LOG(("PDC_set_title() - called: <%s>\n", title));
}
