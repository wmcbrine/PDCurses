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

#include "pdcos2.h"

RCSID("$Id: pdcutil.c,v 1.12 2007/06/14 13:50:26 wmcbrine Exp $")

#if defined(OS2) && !defined(__EMX__)
APIRET APIENTRY DosSleep(ULONG ulTime);
#endif

void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));

#ifdef EMXVIDEO
	putchar('\007');
#else
	DosBeep(1380, 100);
#endif
}

void PDC_napms(int ms)
{
	PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

#ifdef __EMX__
	_sleep2(ms);
#else
	DosSleep(ms);
#endif
}

const char *PDC_sysname(void)
{
	return "OS/2";
}
