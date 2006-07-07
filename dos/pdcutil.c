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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#if defined(DOS) && defined(MSC)
#include <time.h>
#endif

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

RCSID("$Id: pdcutil.c,v 1.1 2006/07/07 02:53:07 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_beep()			- Low-level beep function

  PDCurses Description:
	This is a private PDCurses routine.

	Generates a "beep" sound or flashes the screen. Called by 
	beep().

  Portability:
	PDCurses  void PDC_beep(void);

**man-end****************************************************************/

void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));

	PDC_putctty((chtype) '\007', 0);
}

/* napms() is documented in ../pdcurses/kernel.c */

int napms(int ms)
{
	PDC_LOG(("napms() - called: ms=%d\n", ms));

#if defined(HAVE_USLEEP)

	usleep(1000 * ms);

#elif defined(DOS)

# if defined(__TURBOC__) || defined(__WATCOMC__)

	delay(ms);

# elif defined(MSC) || defined(MX386)
	{
		clock_t goal = (ms / 50) + clock();
		while (goal > clock())
		;
	}
# elif defined(__PACIFIC__)
	{
		/* get number of ticks since startup from address 0040:006ch
		   1 sec. = 18.2065  */

		volatile far long *ticks = MK_FP(0x0040, 0x006c);
		long goal = (ms / 50) + *ticks;
		while (goal > *ticks)
		;
	}
# endif
#endif
	return OK;
}
