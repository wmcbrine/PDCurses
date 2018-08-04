#include "curses.h"

#define CURSOR_ON  "\033[?25h"
#define CURSOR_OFF "\033[?25l"

int PDC_curs_set( int visibility)
{
   printf( visibility ? CURSOR_ON : CURSOR_OFF);
   return( 0);
}

int PDC_really_blinking = FALSE;

int PDC_set_blink(bool blinkon)
{
    PDC_really_blinking = blinkon;
    return OK;
}

void PDC_set_title( const char *title)
{
}
