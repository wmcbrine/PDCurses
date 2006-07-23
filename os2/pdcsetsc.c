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
#include <curses.h>
#include <string.h>

RCSID("$Id: pdcsetsc.c,v 1.24 2006/07/23 23:42:23 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_set_font()  - sets the current font size

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
#ifndef EMXVIDEO
	VIOMODEINFO modeInfo = {0};
#endif
	PDC_LOG(("PDC_set_font() - called\n"));

#ifndef EMXVIDEO
	if (SP->sizeable && (SP->font != size))
	{
		modeInfo.cb = sizeof(modeInfo);

		/* set most parameters of modeInfo */

		VioGetMode(&modeInfo, 0);
		modeInfo.cb = 8;	/* ignore horiz an vert resolution */
		modeInfo.row = modeInfo.vres / size;
		VioSetMode(&modeInfo, 0);
	}

	curs_set(SP->visibility);

	SP->font = PDC_get_font();
#endif
	return OK;
}

/*man-start**************************************************************

  PDC_set_scrn_mode()   - Set BIOS Video Mode

  PDCurses Description:
	Sets the BIOS Video Mode Number ONLY if it is different from
	the current video mode.  This routine is for DOS systems only.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_set_scrn_mode(int new_mode);

**man-end****************************************************************/

#ifndef EMXVIDEO
int PDC_set_scrn_mode(VIOMODEINFO new_mode)
#else
int PDC_set_scrn_mode(int new_mode)
#endif
{
	PDC_LOG(("PDC_set_scrn_mode() - called\n"));

#ifdef EMXVIDEO
	return OK;
#else
	if (VioSetMode(&new_mode, 0) != 0)
	{
		SP->font = PDC_get_font();
		memcpy((char *)&SP->scrnmode, (char *)&new_mode,
			sizeof(VIOMODEINFO));
		LINES = PDC_get_rows();
		COLS = PDC_get_columns();

		return OK;
	}
	else
		return ERR;
#endif
}

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
		start = SP->font / 4;
		end = SP->font;
#endif
		hidden = -1;
		break;

	case 2: 	/* highly visible */
		start = 2;		/* almost full-height block */
		end = SP->font - 1;
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
