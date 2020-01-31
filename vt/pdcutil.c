#ifdef _WIN32
   #define WIN32_LEAN_AND_MEAN
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

enum PDC_port PDC_port_val = PDC_PORT_VT;
