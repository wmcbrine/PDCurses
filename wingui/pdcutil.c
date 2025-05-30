/* Public Domain Curses */

#include "pdcwin.h"

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

/*  MessageBeep(MB_OK); */
    MessageBeep(0XFFFFFFFF);
}

void PDC_napms(int ms)     /* 'ms' = milli,  _not_ microseconds! */
{
    /* RR: keep GUI window responsive while PDCurses sleeps */
    MSG msg;
    DWORD curr_ms = GetTickCount( );
    const DWORD milliseconds_sleep_limit = ms + curr_ms;

    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    /* Pump all pending messages from WIN32 to the window handler */
    while( !PDC_bDone && curr_ms < milliseconds_sleep_limit )
    {
        const DWORD max_sleep_ms = 50;      /* check msgs 20 times/second */
        DWORD sleep_millisecs;

        while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
        }
        curr_ms = GetTickCount( );
        sleep_millisecs = milliseconds_sleep_limit - curr_ms;
        if( sleep_millisecs > max_sleep_ms)
            sleep_millisecs = max_sleep_ms;
        Sleep( sleep_millisecs);
        curr_ms += sleep_millisecs;
    }
}

const char *PDC_sysname(void)
{
   return "WinGUI";
}
