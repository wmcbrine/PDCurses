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
#ifndef EMXVIDEO
# define INCL_VIO
# define INCL_KBD
# include <os2.h>
#endif
#include <curses.h>

#ifdef __EMX__
# include <stdlib.h>
#endif

#if defined(OS2) && !defined(__EMX__)
APIRET APIENTRY DosSleep(ULONG ulTime);
#endif

RCSID("$Id: pdcutil.c,v 1.6 2006/09/25 06:34:41 wmcbrine Exp $");

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
