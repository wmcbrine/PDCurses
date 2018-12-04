/* Public Domain Curses */

#include "pdcwin.h"

#include <stdlib.h>
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
    PDC_ACS(0x1a), PDC_ACS(0x1b), PDC_ACS(0x18), PDC_ACS(0x19),
#endif

    '/',

#ifdef PDC_WIDE
    0x2588,
#else
    0xdb,
#endif

    '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

#ifdef PDC_WIDE
    0x2666, 0x2592,
#else
    PDC_ACS(0x04), 0xb1,
#endif

    'b', 'c', 'd', 'e',

#ifdef PDC_WIDE
    0x00b0, 0x00b1, 0x2591, 0x00a4, 0x2518, 0x2510, 0x250c, 0x2514,
    0x253c, 0x23ba, 0x23bb, 0x2500, 0x23bc, 0x23bd, 0x251c, 0x2524,
    0x2534, 0x252c, 0x2502, 0x2264, 0x2265, 0x03c0, 0x2260, 0x00a3,
    0x00b7,
#else
    0xf8, 0xf1, 0xb0, PDC_ACS(0x0f), 0xd9, 0xbf, 0xda, 0xc0, 0xc5, 0x2d,
    0x2d, 0xc4, 0x2d, 0x5f, 0xc3, 0xb4, 0xc1, 0xc2, 0xb3, 0xf3, 0xf2,
    0xe3, 0xd8, 0x9c, 0xf9,
#endif

    PDC_ACS(127)
};

DWORD pdc_last_blink;
static bool blinked_off = FALSE;

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

void _set_ansi_color(short f, short b, attr_t attr)
{
    char esc[64], *p;
    short tmp, underline;

    if (f < 16)
        f = pdc_curstoansi[f];

    if (b < 16)
        b = pdc_curstoansi[b];

    if (attr & A_REVERSE)
    {
        tmp = f;
        f = b;
        b = tmp;
    }
    underline = !!(attr & A_UNDERLINE);

    p = esc + sprintf(esc, "\x1b[");

    if (f != pdc_oldf)
    {
        if (f < 8)
            p += sprintf(p, "%d", f + 30);
        else if (f < 16)
            p += sprintf(p, "%d", f + 82);
        else
            p += sprintf(p, "38;5;%d", f);

        pdc_oldf = f;
    }

    if (b != pdc_oldb)
    {
        if (strlen(esc) > 2)
            p += sprintf(p, ";");

        if (b < 8)
            p += sprintf(p, "%d", b + 40);
        else if (b < 16)
            p += sprintf(p, "%d", b + 92);
        else
            p += sprintf(p, "48;5;%d", b);

        pdc_oldb = b;
    }

    if (underline != pdc_oldu)
    {
        if (strlen(esc) > 2)
            p += sprintf(p, ";");

        if (underline)
            p += sprintf(p, "4");
        else
            p += sprintf(p, "24");

        pdc_oldu = underline;
    }

    if (strlen(esc) > 2)
    {
        sprintf(p, "m");
        if (!pdc_conemu)
            SetConsoleMode(pdc_con_out, 0x0015);

        WriteConsoleA(pdc_con_out, esc, strlen(esc), NULL, NULL);

        if (!pdc_conemu)
            SetConsoleMode(pdc_con_out, 0x0010);
    }
}

void _new_packet(attr_t attr, int lineno, int x, int len, const chtype *srcp)
{
    int j;
    short fore, back;
    bool blink, ansi;

    if (pdc_conemu && pdc_ansi &&
        (lineno == (SP->lines - 1)) && ((x + len) == SP->cols))
    {
        len--;
        if (len)
            _new_packet(attr, lineno, x, len, srcp);
        pdc_ansi = FALSE;
        _new_packet(attr, lineno, x + len, 1, srcp + len);
        pdc_ansi = TRUE;
        return;
    }

    PDC_pair_content(PAIR_NUMBER(attr), &fore, &back);
    ansi = pdc_ansi || (fore >= 16 || back >= 16);
    blink = (SP->termattrs & A_BLINK) && (attr & A_BLINK);

    if (blink)
    {
        attr &= ~A_BLINK;
        if (blinked_off)
            attr &= ~(A_UNDERLINE | A_RIGHT | A_LEFT);
    }

    if (attr & A_BOLD)
        fore |= 8;
    if (attr & A_BLINK)
        back |= 8;

    if (ansi)
    {
#ifdef PDC_WIDE
        WCHAR buffer[512];
#else
        char buffer[512];
#endif
        for (j = 0; j < len; j++)
        {
            chtype ch = srcp[j];

            if (ch & A_ALTCHARSET && !(ch & 0xff80))
                ch = acs_map[ch & 0x7f];

            if (blink && blinked_off)
                ch = ' ';

            buffer[j] = ch & A_CHARTEXT;
        }

        PDC_gotoyx(lineno, x);
        _set_ansi_color(fore, back, attr);
#ifdef PDC_WIDE
        WriteConsoleW(pdc_con_out, buffer, len, NULL, NULL);
#else
        WriteConsoleA(pdc_con_out, buffer, len, NULL, NULL);
#endif
    }
    else
    {
        CHAR_INFO buffer[512];
        COORD bufSize, bufPos;
        SMALL_RECT sr;
        WORD mapped_attr;

        fore = pdc_curstoreal[fore];
        back = pdc_curstoreal[back];

        if (attr & A_REVERSE)
            mapped_attr = back | (fore << 4);
        else
            mapped_attr = fore | (back << 4);

        if (attr & A_UNDERLINE)
            mapped_attr |= 0x8000; /* COMMON_LVB_UNDERSCORE */
        if (attr & A_LEFT)
            mapped_attr |= 0x0800; /* COMMON_LVB_GRID_LVERTICAL */
        if (attr & A_RIGHT)
            mapped_attr |= 0x1000; /* COMMON_LVB_GRID_RVERTICAL */

        for (j = 0; j < len; j++)
        {
            chtype ch = srcp[j];

            if (ch & A_ALTCHARSET && !(ch & 0xff80))
                ch = acs_map[ch & 0x7f];

            if (blink && blinked_off)
                ch = ' ';

            buffer[j].Attributes = mapped_attr;
            buffer[j].Char.UnicodeChar = ch & A_CHARTEXT;
        }

        bufPos.X = bufPos.Y = 0;
        bufSize.X = len;
        bufSize.Y = 1;

        sr.Top = sr.Bottom = lineno;
        sr.Left = x;
        sr.Right = x + len - 1;

        WriteConsoleOutput(pdc_con_out, buffer, bufSize, bufPos, &sr);
    }
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    attr_t old_attr, attr;
    int i, j;

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    old_attr = *srcp & (A_ATTRIBUTES ^ A_ALTCHARSET);

    for (i = 1, j = 1; j < len; i++, j++)
    {
        attr = srcp[i] & (A_ATTRIBUTES ^ A_ALTCHARSET);

        if (attr != old_attr)
        {
            _new_packet(old_attr, lineno, x, i, srcp);
            old_attr = attr;
            srcp += i;
            x += i;
            i = 0;
        }
    }

    _new_packet(old_attr, lineno, x, i, srcp);
}

void PDC_blink_text(void)
{
    int i, j, k;

    if (!(SP->termattrs & A_BLINK))
        blinked_off = FALSE;
    else
        blinked_off = !blinked_off;

    for (i = 0; i < SP->lines; i++)
    {
        const chtype *srcp = curscr->_y[i];

        for (j = 0; j < SP->cols; j++)
            if (srcp[j] & A_BLINK)
            {
                k = j;
                while (k < SP->cols && (srcp[k] & A_BLINK))
                    k++;
                PDC_transform_line(i, j, k - j, srcp + j);
                j = k;
            }
    }

    PDC_gotoyx(SP->cursrow, SP->curscol);
    pdc_last_blink = GetTickCount();
}
