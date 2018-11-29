#include <stdio.h>
#include "curses.h"

#define CURSOR_ON  "\033[?25h"
#define CURSOR_OFF "\033[?25l"

int PDC_curs_set( int visibility)
{
   printf( visibility ? CURSOR_ON : CURSOR_OFF);
   return( 0);
}

int PDC_really_blinking = FALSE;


void PDC_show_changes( const short pair, const short idx, const chtype attr);

int PDC_set_blink(bool blinkon)
{
    if( PDC_really_blinking != blinkon)
    {
       PDC_really_blinking = blinkon;
       PDC_show_changes( -1, -1, A_BLINK);
    }
    return OK;
}

void PDC_set_title( const char *title)
{
    printf( "\033]2;%s\a", title);
}
