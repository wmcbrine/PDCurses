/* Public Domain Curses */

#include "pdcx11.h"

#include <string.h>

#ifdef CHTYPE_LONG

#ifdef PDC_WIDE
   #define USE_UNICODE_ACS_CHARS 1
#else
   #define USE_ISO8859_CHARSET
#endif

#include "acs_defs.h"

#endif

int PDC_display_cursor(int oldrow, int oldcol, int newrow, int newcol,
                       int visibility)
{
    char buf[30];
    int idx, pos;

    PDC_LOG(("%s:PDC_display_cursor() - called: NEW row %d col %d, vis %d\n",
             XCLOGMSG, newrow, newcol, visibility));

    if (visibility == -1)
    {
        /* Only send the CURSES_DISPLAY_CURSOR message, no data */

        idx = CURSES_DISPLAY_CURSOR;
        memcpy(buf, &idx, sizeof(int));
        idx = sizeof(int);
    }
    else
    {
        idx = CURSES_CURSOR;
        memcpy(buf, &idx, sizeof(int));

        idx = sizeof(int);
        pos = oldrow + (oldcol << 8);
        memcpy(buf + idx, &pos, sizeof(int));

        idx += sizeof(int);
        pos = newrow + (newcol << 8);
        memcpy(buf + idx, &pos, sizeof(int));

        idx += sizeof(int);
    }

    if (XC_write_socket(xc_display_sock, buf, idx) < 0)
        XCursesExitCursesProcess(1, "exiting from PDC_display_cursor");

    return OK;
}

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

    PDC_display_cursor(SP->cursrow, SP->curscol, row, col, SP->visibility);
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

    XC_get_line_lock(lineno);

    memcpy(Xcurscr + XCURSCR_Y_OFF(lineno) + (x * sizeof(chtype)), srcp,
           len * sizeof(chtype));

    *(Xcurscr + XCURSCR_START_OFF + lineno) = x;
    *(Xcurscr + XCURSCR_LENGTH_OFF + lineno) = len;

    XC_release_line_lock(lineno);

    XCursesInstructAndWait(CURSES_REFRESH);
}
