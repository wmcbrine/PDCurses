#include <stdio.h>
#include <curspriv.h>
#include "pdcvt.h"

// #define BLINKING_CURSOR   "\033[?12h"
#define BLINKING_BLOCK      "\033[1 q"
#define STEADY_BLOCK        "\033[2 q"
#define BLINKING_UNDERLINE  "\033[3 q"
#define STEADY_UNDERLINE    "\033[4 q"
      /* "bar" = "vertical line".  xterm only. */
#define BLINKING_BAR        "\033[5 q"
#define STEADY_BAR          "\033[6 q"
#define CURSOR_ON           "\033[?25h"
#define CURSOR_OFF          "\033[?25l"

int PDC_curs_set( int visibility)
{
    int ret_vis;
    int vis1 = visibility & 0xff, vis2 = (visibility >> 8) & 0xff;
    const char *command = CURSOR_ON;            /* our default */

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;

    if( !SP->visibility && visibility)    /* turn cursor back on */
        printf( CURSOR_ON);
    SP->visibility = visibility;
    if( vis1 && vis2)      /* show solid */
        switch( vis1)
        {
            case 1:        /* "normal" four lines at bottom */
                command = STEADY_BAR;
                break;
            case 2:        /* full block */
                command = STEADY_BLOCK;
                break;
            case 5:        /* bottom half block */
                command = STEADY_UNDERLINE;
                break;
        }
    else switch( vis1 + vis2)
        {
            case 0:        /* just turning it off */
                command = CURSOR_OFF;
                break;
            case 1:        /* "normal" four lines at bottom */
                command = BLINKING_BAR;
                break;
            case 2:        /* full block */
                command = BLINKING_BLOCK;
                break;
            case 5:        /* bottom half block */
                command = BLINKING_UNDERLINE;
                break;
        }

    printf( "%s", command);

    return ret_vis;
}


void PDC_show_changes( const short pair, const short idx, const chtype attr);

int PDC_really_blinking;

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
