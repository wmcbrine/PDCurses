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

#include <curspriv.h>

RCSID("$Id: beep.c,v 1.29 2007/03/16 06:33:44 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								beep

  Synopsis:
	int beep(void);
	int flash(void);

  Description:
	These routines are used to signal the terminal user.  The beep()
	function will sound the audible bell on the terminal, if possible
	and if not, will flash the screen (visible bell).

	The flash() function will flash the screen.

  Return Value:
	These functions return OK.

  Portability				     X/Open    BSD    SYS V
	beep					Y	Y	Y
	flash					Y	Y	Y

**man-end****************************************************************/

int beep(void)
{
	PDC_LOG(("beep() - called\n"));

	if (SP->audible)
		PDC_beep();
	else
		flash();

	return OK;
}

int flash(void)
{
	int z, y, x;

	PDC_LOG(("flash() - called\n"));

	/* Reverse each cell; wait; restore the screen */

	for (z = 0; z < 2; z++) {

		for (y = 0; y < LINES; y++)
			for (x = 0; x < COLS; x++)
				curscr->_y[y][x] ^= A_REVERSE;

		wrefresh(curscr);

		if (!z)
			napms(50);
	}

	return OK;
}
