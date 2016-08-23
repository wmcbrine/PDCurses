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
    DWORD milliseconds_sleep_limit = ms + GetTickCount();
    extern bool PDC_bDone;

    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    /* Pump all pending messages from WIN32 to the window handler */
    while( !PDC_bDone && GetTickCount() < milliseconds_sleep_limit )
    {
        while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
        }
        Sleep(1);
    }

    /* Sleep(ms); */
}

/* The library can be compiled with six different configurations.
It can be a DLL;  and it can be either 'wide',  'UTF-8',  or just
a plain eight-bit character version.         */

const char *PDC_sysname(void)
{
#ifdef DLL
    #ifdef WIDE
        #ifdef UTF8
             return "Win32a UTF8 DLL";
        #else
             return "Win32a Wide DLL";
        #endif
    #else
        return "Win32a DLL";
    #endif
#else
    #ifdef WIDE
        #ifdef UTF8
             return "Win32a UTF8";
        #else
             return "Win32a Wide";
        #endif
    #else
        return "Win32a";
    #endif
#endif
}
