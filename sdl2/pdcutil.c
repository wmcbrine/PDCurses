/* PDCurses */

#include "pdcsdl.h"

bool pdc_dirty = FALSE;

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    if (pdc_dirty)
    {
        pdc_dirty = FALSE;
        wrefresh(curscr);
    }

    PDC_update_rects();
    SDL_PumpEvents();
    SDL_Delay(ms);
}

const char *PDC_sysname(void)
{
    return "SDL2";
}
