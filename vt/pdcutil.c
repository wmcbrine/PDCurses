#include <unistd.h>

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
