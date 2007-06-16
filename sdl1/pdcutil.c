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

#include "pdcsdl.h"

RCSID("$Id: pdcutil.c,v 1.4 2007/06/16 05:16:44 wmcbrine Exp $")

void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));
}

void PDC_napms(int ms)
{
	PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

	PDC_update_rects();
	SDL_Delay(ms);
}

const char *PDC_sysname(void)
{
	return "SDL";
}
