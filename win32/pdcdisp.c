/* Public Domain Curses */

#include "pdcwin.h"

#include <stdlib.h>
#include <string.h>

#ifdef CHTYPE_LONG

#ifdef PDC_WIDE
   #define USE_UNICODE_ACS_CHARS 1
#else
   #define USE_UNICODE_ACS_CHARS 0
#endif

#include "acs_defs.h"

#endif

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    COORD coord;

    PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
             row, col, SP->cursrow, SP->curscol));

    coord.X = col;
    coord.Y = row;

    SetConsoleCursorPosition(pdc_con_out, coord);
}

/* update the given physical line to look like the corresponding line in
   curscr */

/* NOTE:  the original indexing into pdc_atrtab[] relied on three or five
   attribute bits in 'chtype' being adjacent to the color bits.  Such is
   not the case for 64-bit chtypes (CHTYPE_LONG == 2),  so we have to do
   additional bit-fiddling for that situation.  Code is similar in Win32
   and DOS flavors.  (BJG) */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    CHAR_INFO ci[512];
    int j;
    COORD bufSize, bufPos;
    SMALL_RECT sr;

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    bufPos.X = bufPos.Y = 0;

    bufSize.X = len;
    bufSize.Y = 1;

    sr.Top = lineno;
    sr.Bottom = lineno;
    sr.Left = x;
    sr.Right = x + len - 1;

    for (j = 0; j < len; j++)
    {
        chtype ch = srcp[j];

#if defined( CHTYPE_LONG) && (CHTYPE_LONG >= 2)
        ci[j].Attributes = pdc_atrtab[((ch >> PDC_ATTR_SHIFT) & 0x1f)
                     | (((ch >> PDC_COLOR_SHIFT) & 0xff) << 5)];
#else
        ci[j].Attributes = pdc_atrtab[ch >> PDC_ATTR_SHIFT];
#endif

#ifdef CHTYPE_LONG
        if (ch & A_ALTCHARSET && !(ch & 0xff80))
            ch = acs_map[ch & 0x7f];
#endif
        ci[j].Char.UnicodeChar = ch & A_CHARTEXT;
    }

    WriteConsoleOutput(pdc_con_out, ci, bufSize, bufPos, &sr);
}
