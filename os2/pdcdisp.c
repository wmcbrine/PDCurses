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

#ifndef EMXVIDEO
# define INCL_VIO
# define INCL_KBD
# include <os2.h>
#endif
#include <curspriv.h>
#include <string.h>

RCSID("$Id: pdcdisp.c,v 1.37 2006/10/08 20:54:30 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_gotoyx()	- position hardware cursor at (y, x)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the screen. 
	We don't optimize here -- on a PC, it takes more time to 
	optimize than to do things directly.

  Portability:
	PDCurses  void PDC_gotoyx(int row, int col);

**man-end****************************************************************/

void PDC_gotoyx(int row, int col)
{
	PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

#ifdef EMXVIDEO
	v_gotoxy(col, row);
#else
	VioSetCurPos(row, col, 0);
#endif
}

/*man-start**************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  Portability:
	PDCurses  void PDC_transform_line(int lineno, int x, int len, 
					  const chtype *srcp);

**man-end****************************************************************/

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
	/* this should be enough for the maximum width of a screen. */

	unsigned short temp_line[256];
	int j;

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	/* replace the attribute part of the chtype with the 
	   actual color value for each chtype in the line */

	for (j = 0; j < len; j++)
		temp_line[j] = (chtype_attr(srcp[j]) << 8) | (srcp[j] & 0xff);

#ifdef EMXVIDEO
	v_putline((char*)temp_line, x, lineno, len);
#else
	VioWrtCellStr((PCH)temp_line, (USHORT)(len * sizeof(unsigned short)),
		(USHORT)lineno, (USHORT)x, 0);
#endif
}
