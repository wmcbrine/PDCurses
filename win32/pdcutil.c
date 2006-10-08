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

#include "pdcwin.h"

RCSID("$Id: pdcutil.c,v 1.8 2006/10/08 20:54:30 wmcbrine Exp $");

void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));

	fflush(stdout);
/*	MessageBeep(MB_OK); */
	MessageBeep(0XFFFFFFFF);
	fflush(stdout);
}

void PDC_napms(int ms)
{
	PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

	Sleep(ms);
}

const char *PDC_sysname(void)
{
	return "Win32";
}
