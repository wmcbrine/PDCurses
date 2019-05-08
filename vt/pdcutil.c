#ifdef _WIN32
   #include <windows.h>
   #undef MOUSE_MOVED
#else
   #include <unistd.h>
#endif
#include "curspriv.h"

void PDC_beep(void)
{
}

void PDC_napms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
#ifndef DOS
    usleep(1000 * ms);
#endif
#endif
}


const char *PDC_sysname(void)
{
   return( "VTx00");
}

PDC_version_info PDC_version = { PDC_PORT_VT,
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
