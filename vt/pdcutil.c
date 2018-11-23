#include <unistd.h>
#include "curses.h"

void PDC_beep(void)
{
}

void PDC_napms(int ms)
{
    usleep(1000 * ms);
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
