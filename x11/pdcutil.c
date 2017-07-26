/* Public Domain Curses */

#include "pdcx11.h"

#if defined(HAVE_POLL) && !defined(HAVE_USLEEP)
# include <poll.h>
#endif

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

    XCursesInstruct(CURSES_BELL);
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

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

PDC_version_info PDC_version = { PDC_PORT_WIN32A,
          PDC_VER_MAJOR, PDC_VER_MINOR, PDC_VER_CHANGE,
          sizeof( chtype),
#ifdef PDC_WIDE
          TRUE,
#else
          FALSE,
#endif
#ifdef PDC_FORCE_UTF8
          TRUE,
#else
          FALSE,
#endif
          };
