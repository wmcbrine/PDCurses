/* PDCurses */

#include "pdcdos.h"
#include "../common/acs437.h"

/* Support cursor on graphics mode */
static unsigned char bytes_behind[4][16];
static unsigned char cursor_color = 15;
static void draw_glyph(int row, int col, chtype glyph);
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

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    int i;

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    for (i = 0; i < len; i++)
    {
        draw_glyph(lineno, x + i, srcp[i]);
        if (lineno == PDC_state.cursor_row && x + i == PDC_state.cursor_col)
            PDC_private_cursor_on(PDC_state.cursor_row, PDC_state.cursor_col);
    }
    outportb(0x3c4, 2);
    outportb(0x3c5, 0xF);
}

void PDC_doupdate(void)
{
}

static void draw_glyph(int row, int col, chtype glyph)
{
    unsigned long addr = address(row, col);
    unsigned long font_addr;
    attr_t attr;
    unsigned long cp;
    unsigned char ch;
    short fore, back;
    unsigned char vplane;
    int line;
    int underline;
    unsigned char fnt;

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

    /* Set underline if requested */
    underline = (attr & A_UNDERLINE) ? 13 /*_FONT16*/ : -1;

    /* Get the index into the font */
    ch = glyph & 0xFF;
    if (glyph & A_ALTCHARSET && !(glyph & 0xff80))
        ch = acs_map[ch & 0x7f] & 0xff;

    /* Get the address of the glyph in memory */
    font_addr = PDC_state.font_addr + ch * _FONT16;

    /* Planes where fore has 0 and back has 0 */
    vplane = ~fore & ~back & 0xF;
    if (vplane != 0) {
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        for (line = 0; line < _FONT16; ++line) {
            fnt = 0x00;
            video_write_byte(cp, fnt);
            cp += PDC_state.bytes_per_line;
        }
    }
    /* Planes where fore has 1 and back has 0 */
    vplane =  fore & ~back & 0xF;
    if (vplane != 0) {
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        for (line = 0; line < _FONT16; ++line) {
            if (line == underline)
                fnt = 0xFF;
            else
                fnt = getdosmembyte(font_addr + line);
            video_write_byte(cp, fnt);
            cp += PDC_state.bytes_per_line;
        }
    }
    /* Planes where fore has 0 and back has 1 */
    vplane = ~fore &  back & 0xF;
    if (vplane != 0) {
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        for (line = 0; line < _FONT16; ++line) {
            if (line == underline)
                fnt = 0x00;
            else
                fnt = ~getdosmembyte(font_addr + line);
            video_write_byte(cp, fnt);
            cp += PDC_state.bytes_per_line;
        }
    }
    /* Planes where fore has 1 and back has 1 */
    vplane =  fore &  back & 0xF;
    if (vplane != 0) {
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        for (line = 0; line < _FONT16; ++line) {
            fnt = 0xFF;
            video_write_byte(cp, fnt);
            cp += PDC_state.bytes_per_line;
        }
    }
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
