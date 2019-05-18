/* PDCurses */

#include "pdcx11.h"

#if defined(HAVE_POLL) && !defined(HAVE_USLEEP)
# include <poll.h>
#endif

bool pdc_dirty = FALSE;

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

    XCursesInstruct(CURSES_BELL);
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    if (pdc_dirty)
    {
        pdc_dirty = FALSE;
        XCursesInstruct(CURSES_DISPLAY_ALL);
    }

    XSync(XtDisplay(topLevel), False);

#if defined(HAVE_USLEEP)

    usleep(1000 * ms);

#elif defined(HAVE_POLL)
    {
        struct pollfd fd;
        fd.fd = -1;
        fd.events = 0;
        poll(&fd, 1, ms);
    }
#endif
}

const char *PDC_sysname(void)
{
    return "X11";
}
