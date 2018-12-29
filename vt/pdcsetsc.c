#include <stdio.h>
#include <curspriv.h>
#include "pdcvt.h"

#define CURSOR_ON  "\033[?25h"
#define CURSOR_OFF "\033[?25l"

int PDC_curs_set( int visibility)
{
    int ret_vis;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;

    SP->visibility = visibility;

    printf( visibility ? CURSOR_ON : CURSOR_OFF);

    return ret_vis;
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
    PDC_LOG(("PDC_set_title() - called:<%s>\n", title));

#ifndef DOS
    if( !PDC_is_ansi)
        printf( "\033]2;%s\a", title);
#endif
}
