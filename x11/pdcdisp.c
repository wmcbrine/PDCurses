/* Public Domain Curses */

#include "pdcx11.h"

#include <string.h>

#ifdef CHTYPE_LONG

# define A(x) ((chtype)x | A_ALTCHARSET)

#ifdef PDC_WIDE
   #define USE_UNICODE_ACS_CHARS 1
#else
   #define USE_ISO8859_CHARSET
#endif

#include "acs_defs.h"

chtype acs_map[128] =
{
   A_ORDINAL, O_ORDINAL, INVERTED_QUESTION_MARK,                /* 0 1 2 */
   REVERSED_NOT_SIGN, NOT_SIGN, VULGAR_HALF,                    /* 3 4 5 */
   VULGAR_QUARTER, INVERTED_EXCLAMATION_MARK,                   /* 6 7   */
   LEFT_ANGLE_QUOTE_MARK, RIGHT_ANGLE_QUOTE_MARK,               /* 8 9   */
   UPPER_HALF_INTEGRAL_SIGN, LOWER_HALF_INTEGRAL_SIGN,          /* 10 11 */
   SUPERSCRIPT_N, CENTERED_SQUARE, PESETA_SIGN, F_WITH_HOOK,    /* 12-15 */
   RIGHT_SINGLE_DOWN_DOUBLE, RIGHT_SINGLE_UP_DOUBLE,            /* 16 17 */
   DOWN_DOUBLE_LEFT_SINGLE, UP_DOUBLE_LEFT_SINGLE,              /* 18 19 */
   VERTICAL_DOUBLE_RIGHT_SINGLE, VERTICAL_DOUBLE_LEFT_SINGLE,   /* 20 21 */
   HORIZONTAL_SINGLE_DOWN_DOUBLE, HORIZONTAL_SINGLE_UP_DOUBLE,  /* 22 23 */
   HORIZONTAL_SINGLE_VERTICAL_DOUBLE,                           /* 24    */
   RIGHT_DOUBLE_DOWN_SINGLE, RIGHT_DOUBLE_UP_SINGLE,            /* 25 26 */
   LEFT_DOUBLE_DOWN_SINGLE, UP_SINGLE_LEFT_DOUBLE,              /* 27 28 */
   VERTICAL_SINGLE_RIGHT_DOUBLE, VERTICAL_SINGLE_LEFT_DOUBLE,   /* 29 30 */
   HORIZONTAL_DOUBLE_DOWN_SINGLE, HORIZONTAL_DOUBLE_UP_SINGLE,  /* 31 ' ' */
   HORIZONTAL_DOUBLE_VERTICAL_SINGLE,                           /* !     */

    '"', '#', '$', '%', '&', '\'', '(',
    ')', '*',

   RIGHT_ARROW, LEFT_ARROW, UP_ARROW, DOWN_ARROW,                   /* +,-.  */
   '/', FULL_BLOCK,                                                 /* /0    */
   CENT_SIGN, YEN_SIGN, PESETA_SIGN, VULGAR_HALF, VULGAR_QUARTER,   /* 12345 */
   LEFT_ANGLE_QUOTE_MARK, RIGHT_ANGLE_QUOTE_MARK, DARK_SHADE,       /* 678 */
   BOX_DBL_DOWN_RIGHT, BOX_DBL_UP_RIGHT,                            /* 9: */
   BOX_DBL_DOWN_LEFT, BOX_DBL_UP_LEFT,                              /* ;< */
   BOX_DBL_TEE_LEFT, BOX_DBL_TEE_RIGHT,                             /* => */
   BOX_DBL_TEE_UP, BOX_DBL_TEE_DOWN,                                /* ?@ */
   BOX_DBL_HORIZONTAL, BOX_DBL_VERTICAL,                            /* AB */
   BOX_DBL_CROSS, SUPERSCRIPT_2,                                    /* CD */
   ALPHA, BETA, GAMMA, UPPERCASE_SIGMA, LOWERCASE_SIGMA,         /* EFGHI */
   MU, TAU, UPPERCASE_PHI, THETA, OMEGA, DELTA, INFINITY,       /* JKLMNOP */
   LOWERCASE_PHI, EPSILON, INTERSECTION, TRIPLE_BAR,            /* QRST */
   DIVISION_SIGN, APPROXIMATELY_EQUALS_SIGN, SMALL_BULLET,      /* UVW */
   SQUARE_ROOT, UPPER_HALF_BLOCK, LOWER_HALF_BLOCK,             /* XYZ */
   LEFT_HALF_BLOCK, RIGHT_HALF_BLOCK,                           /* [\ */

   ']', '^', '_',
    DIAMOND, MEDIUM_SHADE,          /* `a */

    'b', 'c', 'd', 'e',

   DEGREE_SIGN, PLUS_OR_MINUS, LIGHT_SHADE, SPLAT,          /* fghi */
   LEFT_UP, DOWN_LEFT, RIGHT_DOWN, UP_RIGHT,                /* jklm */
   HORIZONTAL_VERTICAL, HORIZ_SCAN_LINE_1, HORIZ_SCAN_LINE_3,  /* nop */
   HORIZONTAL, HORIZ_SCAN_LINE_7, HORIZ_SCAN_LINE_9,        /* qrs */
   RIGHT_VERTICAL, VERTICAL_LEFT, UP_HORIZONTAL,            /* tuv */
   DOWN_HORIZONTAL, VERTICAL, LESSER_THAN_OR_EQUAL_TO,      /* wxy */
   GREATER_THAN_OR_EQUAL_TO, PI, NOT_EQUALS_SIGN,           /* z{| */
   STERLING_SIGN, LARGE_BULLET,                             /* }~  */

    A(127)
};

# undef A

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
