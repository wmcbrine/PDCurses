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

#include "pdcdos.h"

#ifdef __DJGPP__
# include <unistd.h>
#endif

#include <time.h>

RCSID("$Id: pdcutil.c,v 1.19 2007/01/01 21:04:27 wmcbrine Exp $");

void PDC_beep(void)
{
	PDCREGS regs;

	PDC_LOG(("PDC_beep() - called\n"));

	regs.W.ax = 0x0e07;       /* Write ^G in TTY fashion */
	regs.W.bx = 0;
	PDCINT(0x10, regs);
}

#ifdef __DJGPP__
# define PDCCLOCK_T uclock_t
# define PDCCLOCK() uclock()
#else
# define PDCCLOCK_T long
# define PDCCLOCK() getdosmemdword(0x46c)
#endif

void PDC_napms(int ms)
{
	PDCREGS regs;
	PDCCLOCK_T goal, start, current;

	PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

#ifdef __DJGPP__
	goal = (PDCCLOCK_T)ms * UCLOCKS_PER_SEC / 1000;
#else
	goal = DIVROUND((PDCCLOCK_T)ms, 50);
#endif
	if (!goal)
		goal++;

	start = PDCCLOCK();

	goal += start;

	while (goal > (current = PDCCLOCK()))
	{
		if (current < start)	/* in case of midnight reset */
			return;

		regs.W.ax = 0x1680;
		PDCINT(0x2f, regs);
		PDCINT(0x28, regs);
	}
}

const char *PDC_sysname(void)
{
	return "DOS";
}

#ifdef __DJGPP__

unsigned char getdosmembyte(int offset)
{
	unsigned char b;

	dosmemget(offset, sizeof(unsigned char), &b);
	return b;
}

unsigned short getdosmemword(int offset)
{
	unsigned short w;

	dosmemget(offset, sizeof(unsigned short), &w);
	return w;
}

unsigned long getdosmemdword(int offset)
{
	unsigned long dw;

	dosmemget(offset, sizeof(unsigned long), &dw);
	return dw;
}

void setdosmembyte(int offset, unsigned char b)
{
	dosmemput(&b, sizeof(unsigned char), offset);
}

void setdosmemword(int offset, unsigned short w)
{
	dosmemput(&w, sizeof(unsigned short), offset);
}

#endif

#if defined(__WATCOMC__) && defined(__386__)

void PDC_dpmi_int(int vector, pdc_dpmi_regs *rmregs)
{
	union REGPACK regs = {0};

	rmregs->w.ss = 0;
	rmregs->w.sp = 0;
	rmregs->w.flags = 0;

	regs.w.ax = 0x300;
	regs.h.bl = vector;
	regs.x.edi = FP_OFF(rmregs);
	regs.x.es = FP_SEG(rmregs);

	intr(0x31, &regs);
}

#endif
