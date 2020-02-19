/* PDCurses */

#include "pdcdos.h"
#include "../common/acs437.h"

/* Support cursor on graphics mode */
static unsigned char bytes_behind[4][16];
static unsigned char cursor_color = 15;
static unsigned _get_colors(chtype glyph);
static unsigned long address(int row, int col);
static void video_write_byte(unsigned long addr, unsigned char byte);
static unsigned char video_read_byte(unsigned long addr);

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    if (PDC_state.cursor_visible)
        PDC_private_cursor_off();
    PDC_private_cursor_on(row, col);
}

static void _new_packet(unsigned colors, int lineno, int x, int len, const chtype *srcp)
{
    unsigned fore, back;
    int underline;
    unsigned long addr = address(lineno, x);
    unsigned pass;

    fore = colors & 0xF;
    back = colors >> 4;

    /* Underline will go here if requested */
    underline = 13 /*_FONT16*/;

    /* Draw the text in four passes, to optimize for the memory architecture
       of the VGA in four bit pixel modes */
    for (pass = 0; pass < 4; pass++)
    {
        unsigned char vplane;
        unsigned long cp;
        int line;
        int col;

        /* Set memory planes to write */
        if (pass & 2)
            vplane = fore;
        else
            vplane = ~fore;
        if (pass & 1)
            vplane &= back;
        else
            vplane &= ~back;
        vplane &= 0xF;
        if (vplane == 0)
            continue;
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);

        /* Loop by raster line */
        cp = addr;
        for (line = 0; line < _FONT16; line++)
        {
            /* Loop by column */
            for (col = 0; col < len; col++)
            {
                chtype glyph = srcp[col];
                int ch;
                unsigned char byte;

                /* Get the index into the font */
                ch = glyph & 0xFF;
                if ((glyph & A_ALTCHARSET) != 0 && (glyph & 0xff80) == 0)
                    ch = acs_map[ch & 0x7f] & 0xff;

                /* Get one byte of the glyph to be drawn */
                if (pass == 0 || pass == 3)
                    byte = 0x00;
                else if (line == underline && (glyph & A_UNDERLINE) != 0)
                    byte = 0xFF;
                else
                    byte = getdosmembyte(PDC_state.font_addr + ch*_FONT16 + line);
                if (pass & 1)
                    byte = ~byte;

                /* Place the byte in the frame buffer */
                video_write_byte(cp + col, byte);
            }
            cp += PDC_state.bytes_per_line;
        }
    }
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    unsigned old_colors, colors;
    int i, j;

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    /* Draw runs of characters that have the same colors */
    old_colors = _get_colors(srcp[0]);

    for (i = 1, j = 1; j < len; i++, j++)
    {
        colors = _get_colors(srcp[i]);

        if (colors != old_colors)
        {
            _new_packet(old_colors, lineno, x, i, srcp);
            old_colors = colors;
            srcp += i;
            x += i;
            i = 0;
        }
    }

    _new_packet(old_colors, lineno, x, i, srcp);

    /* Redraw the cursor if it has been erased */
    if (lineno == PDC_state.cursor_row
    &&  x <= PDC_state.cursor_col && PDC_state.cursor_col < x + len)
        PDC_private_cursor_on(PDC_state.cursor_row, PDC_state.cursor_col);

    /* Reset the VGA plane register to its normal state */
    outportb(0x3c4, 2);
    outportb(0x3c5, 0xF);
}

void PDC_doupdate(void)
{
}

static unsigned _get_colors(chtype glyph)
{
    attr_t attr;
    short fore, back;

    /* Get the foreground and background colors */
    attr = glyph & (A_ATTRIBUTES ^ A_ALTCHARSET);
    pair_content(PAIR_NUMBER(attr), &fore, &back);

    if (attr & A_BOLD)
        fore |= 8;
    if (attr & A_BLINK)
        back |= 8;

    fore = PDC_state.pdc_curstoreal[fore];
    back = PDC_state.pdc_curstoreal[back];

    if (attr & A_REVERSE)
    {
        short swap = fore;
        fore = back;
        back = swap;
    }

    /* Return them as a pair */
    return (back << 4) | fore;
}

void PDC_private_cursor_off(void)
{
    if (PDC_state.cursor_visible && PDC_state.cursor_row < (unsigned)LINES
    &&  PDC_state.cursor_col < (unsigned)COLS) {
        unsigned long addr = address(PDC_state.cursor_row, PDC_state.cursor_col);
        unsigned plane;
        unsigned line;

        for (plane = 0; plane < 4; ++plane)
        {
            unsigned long p = addr;
            outportb(0x03C4, 2);
            outportb(0x03C5, 1 << plane);
            for (line = 0; line < _FONT16; ++line)
            {
                video_write_byte(p, bytes_behind[plane][line]);
                p += PDC_state.bytes_per_line;
            }
        }
    }
    PDC_state.cursor_visible = FALSE;
}

void PDC_private_cursor_on(int row, int col)
{
    unsigned long addr = address(row, col);
    unsigned long p;
    unsigned plane;
    unsigned line;

    for (plane = 0; plane < 4; ++plane)
    {
        p = addr;
        outportb(0x03CE, 4);
        outportb(0x03CF, plane);
        for (line = 0; line < _FONT16; ++line)
        {
            bytes_behind[plane][line] = video_read_byte(p);
            p += PDC_state.bytes_per_line;
        }
    }
    outportb(0x03C5, 2);
    outportb(0x03C5,  cursor_color);
    p = address(row, col);
    p += PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        video_write_byte(p, 0xFF);
        p += PDC_state.bytes_per_line;
    }
    outportb(0x03C5, 2);
    outportb(0x03C5, ~cursor_color);
    p = address(row, col);
    p += PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        video_write_byte(p, 0x00);
        p += PDC_state.bytes_per_line;
    }
    outportb(0x03C5, 2);
    outportb(0x03C5, 0xF);
    PDC_state.cursor_visible = TRUE;
    PDC_state.cursor_row = row;
    PDC_state.cursor_col = col;
}

static unsigned long address(int row, int col)
{
    return row * PDC_state.bytes_per_line * _FONT16 + col;
}

static void video_write_byte(unsigned long addr, unsigned char byte)
{
    /* TODO: support VESA modes */
    if (addr < 0x10000)
        setdosmembyte(0xA0000 + addr, byte);
}

static unsigned char video_read_byte(unsigned long addr)
{
    /* TODO: support VESA modes */
    unsigned char byte;
    if (addr < 0x10000)
        byte = getdosmembyte(0xA0000 + addr);
    else
        byte = 0;
    return byte;
}
