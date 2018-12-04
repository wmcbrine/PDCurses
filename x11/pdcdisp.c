/* Public Domain Curses */

#include "pdcx11.h"

#include <string.h>

chtype acs_map[128] =
{
    PDC_ACS(0), PDC_ACS(1), PDC_ACS(2), PDC_ACS(3), PDC_ACS(4),
    PDC_ACS(5), PDC_ACS(6), PDC_ACS(7), PDC_ACS(8), PDC_ACS(9),
    PDC_ACS(10), PDC_ACS(11), PDC_ACS(12), PDC_ACS(13), PDC_ACS(14),
    PDC_ACS(15), PDC_ACS(16), PDC_ACS(17), PDC_ACS(18), PDC_ACS(19),
    PDC_ACS(20), PDC_ACS(21), PDC_ACS(22), PDC_ACS(23), PDC_ACS(24),
    PDC_ACS(25), PDC_ACS(26), PDC_ACS(27), PDC_ACS(28), PDC_ACS(29),
    PDC_ACS(30), PDC_ACS(31), ' ', '!', '"', '#', '$', '%', '&', '\'',
    '(', ')', '*',

#ifdef PDC_WIDE
    0x2192, 0x2190, 0x2191, 0x2193,
#else
    '>', '<', '^', 'v',
#endif

    '/',

#ifdef PDC_WIDE
    0x2588,
#else
    PDC_ACS(0),
#endif

    '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

#ifdef PDC_WIDE
    0x2666, 0x2592,
#else
    PDC_ACS(1), PDC_ACS(2),
#endif

    'b', 'c', 'd', 'e',

#ifdef PDC_WIDE
    0x00b0, 0x00b1, 0x2591, 0x00a4, 0x2518, 0x2510, 0x250c, 0x2514,
    0x253c, 0x23ba, 0x23bb, 0x2500, 0x23bc, 0x23bd, 0x251c, 0x2524,
    0x2534, 0x252c, 0x2502, 0x2264, 0x2265, 0x03c0, 0x2260, 0x00a3,
    0x00b7,
#else
    PDC_ACS(7), PDC_ACS(8), '#', 0xa4, PDC_ACS(11), PDC_ACS(12),
    PDC_ACS(13), PDC_ACS(14), PDC_ACS(15), PDC_ACS(16), PDC_ACS(17),
    PDC_ACS(18), PDC_ACS(19), PDC_ACS(20), PDC_ACS(21), PDC_ACS(22),
    PDC_ACS(23), PDC_ACS(24), PDC_ACS(25), PDC_ACS(26), PDC_ACS(27),
    PDC_ACS(28), PDC_ACS(29), PDC_ACS(30), 0xb7,
#endif

    PDC_ACS(127)
};

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
        if ((oldrow == newrow) && (oldcol == newcol))
        {
            /* Do not send a message because it will cause the blink state
               to reset. */
            return OK;
        }

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
