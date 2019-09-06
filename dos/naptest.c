#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

/* Quick,  somewhat dirty test code for the weirdnesses that occur
in the DOS PDC_napms() code.  Compiles (in Digital Mars C/C++) with

dmc -I.. -msd -DCHTYPE_32 -o+space naptest.exe pdcurses.lib naptest.c

   (not tested yet with any other compiler;  curr_ticks() may need
alteration.  See 'pdcutil.c' for code that would do the job.)

   Note that a 'special' PDC_napmsl() function,  taking a long integer
for an argument,  is used.  Otherwise,  we couldn't do any testing
past 32.767 seconds.   */

static long curr_ticks( void)
{
   return( *(long far *)0x46c);
}

#define MAX_TICK       0x1800b0ul      /* no. of IRQ 0 clock ticks per day;
                                          BIOS counter (0:0x46c) will go up
                                          to MAX_TICK - 1 before wrapping to
                                          0 at midnight */
#define MS_PER_DAY     86400000ul      /* no. of milliseconds in a day */

static char *format_ticks( char *buff, const long ticks)
{
   const double tval = (double)ticks * (double)MS_PER_DAY / (double)MAX_TICK;
   const long ms = (long)tval;

   sprintf( buff, "%02ld:%02ld:%02ld.%03ld",
               (ms / 3600000L) % 24L,   /* hours */
               (ms / 60000L) % 60L,   /* minutes */
               (ms / 1000L) % 60L, ms % 1000L);
   return( buff);
}

void PDC_napmsl( long ms);

int main( const int argc, const char **argv)
{
   long ticks1, ticks0;
   const long nap_time = atol( argv[1]);
   char tbuff[20];

   printf( "Napping %ld milliseconds\n", nap_time);
   ticks0 = curr_ticks( );
   printf( "%s Ticks : %ld\n", format_ticks( tbuff, ticks0), ticks0);
   PDC_napmsl( nap_time);
   ticks1 = curr_ticks( );
   printf( "%s Ticks : %ld\n", format_ticks( tbuff, ticks1), ticks1);
   ticks1 -= ticks0;
   if( ticks1 < 0)             /* crossed midnight */
      ticks1 += MAX_TICK;
   printf( "%s Elapsed time\n", format_ticks( tbuff, ticks1));
   printf( "Hit enter:");
   getchar( );
   return( 0);
}
