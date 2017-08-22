/* Public Domain Curses */

#include "pdcos2.h"

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

PDCEX PDC_version_info PDC_version = { PDC_PORT_OS2,
          PDC_VER_MAJOR, PDC_VER_MINOR, PDC_VER_CHANGE,
          sizeof( chtype),
               /* note that thus far,  'wide' and 'UTF8' versions exist */
               /* only for SDL2, X11,  Win32,  and Win32a;  elsewhere, */
               /* these will be FALSE */
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
