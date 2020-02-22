/* PDCurses */

#include "pdcdos.h"
#include "../common/acs437.h"

/* Support cursor on graphics mode */
static unsigned long bytes_behind[8][16];
static unsigned char cursor_color = 15;
static unsigned long _get_colors(chtype glyph);
static unsigned long _address_4(int row, int col);
static unsigned long _address_8(int row, int col);
static void _video_write_byte(unsigned long addr, unsigned char byte);
static void _video_write_word(unsigned long addr, unsigned short word);
static void _video_write_3byte(unsigned long addr, unsigned long byte3);
static void _video_write_dword(unsigned long addr, unsigned long dword);
static unsigned char _video_read_byte(unsigned long addr);
static unsigned short _video_read_word(unsigned long addr);
static unsigned long _video_read_3byte(unsigned long addr);
static unsigned long _video_read_dword(unsigned long addr);
static unsigned _set_window(unsigned window, unsigned long addr);

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    if (PDC_state.cursor_visible)
        PDC_private_cursor_off();
    PDC_private_cursor_on(row, col);
}

/* Draw a run of characters with the same colors */
/* Used with 4 bit pixels only */

static void _new_packet(unsigned long colors, int lineno, int x, int len, const chtype *srcp)
{
    unsigned fore, back;
    int underline;
    unsigned long addr = _address_4(lineno, x);
    unsigned pass;

    fore = colors & 0xF;
    back = (colors >> 16) & 0xF;

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
                _video_write_byte(cp + col, byte);
            }
            cp += PDC_state.bytes_per_line;
        }
    }
}

/* PDC_transform_line for 4 bit pixels */
 
static void _transform_line_4(int lineno, int x, int len, const chtype *srcp)
{
    unsigned long old_colors, colors;
    int i, j;

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

    /* Reset the VGA plane register to its normal state */
    outportb(0x3c4, 2);
    outportb(0x3c5, 0xF);
}

/* PDC_transform_line for 8, 15, 16, 24 and 32 bit pixels */
 
static void _transform_line_8(int lineno, int x, int len, const chtype *srcp)
{
    unsigned long addr = _address_8(lineno, x);
    unsigned underline;
    unsigned line;

    /* Underline will go here if requested */
    underline = 13 /*_FONT16*/;

    /* Draw the entire line in pixel order, to minimize the use of the VESA
       window function */

    /* Loop by raster line */
    for (line = 0; line < _FONT16; line++)
    {
        int col;
        unsigned addr2 = addr;

        /* Loop by column */
        for (col = 0; col < len; col++)
        {
            chtype glyph = srcp[col];
            int ch;
            unsigned char byte, bit;
            unsigned long colors;
            unsigned long fore, back;

            /* Get the index into the font */
            ch = glyph & 0xFF;
            if ((glyph & A_ALTCHARSET) != 0 && (glyph & 0xff80) == 0)
                ch = acs_map[ch & 0x7f] & 0xff;

            /* Get one byte from the font */
            if ((glyph & A_UNDERLINE) != 0 && line == underline)
                byte = 0xFF;
            else
                byte = getdosmembyte(PDC_state.font_addr + ch*_FONT16 + line);

            /* Get the colors */
            colors = _get_colors(glyph);
            fore = colors & 0xFFFF;
            back = (colors >> 16) & 0xFFFF;
            if (PDC_state.bits_per_pixel > 8)
            {
                fore = PDC_state.colors[fore].mapped;
                back = PDC_state.colors[back].mapped;
            }

            /* Loop by pixel */
            switch (PDC_state.bits_per_pixel)
            {
            case 8:
                for (bit = 0x80; bit != 0; bit >>= 1)
                    _video_write_byte(addr2++, (byte & bit) ? fore : back);
                break;

            case 15:
            case 16:
                for (bit = 0x80; bit != 0; bit >>= 1)
                {
                    _video_write_word(addr2, (byte & bit) ? fore : back);
                    addr2 += 2;
                }
                break;

            case 24:
                for (bit = 0x80; bit != 0; bit >>= 1)
                {
                    _video_write_3byte(addr2, (byte & bit) ? fore : back);
                    addr2 += 3;
                }
                break;

            case 32:
                for (bit = 0x80; bit != 0; bit >>= 1)
                {
                    _video_write_dword(addr2, (byte & bit) ? fore : back);
                    addr2 += 4;
                }
                break;
            }
        }
        addr += PDC_state.bytes_per_line;
    }
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    bool redraw_cursor;

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    redraw_cursor = PDC_state.cursor_visible
                 && lineno == PDC_state.cursor_row
                 && x <= PDC_state.cursor_col
                 && PDC_state.cursor_col < x + len;
    if (redraw_cursor)
        PDC_private_cursor_off();

    if (PDC_state.bits_per_pixel == 4)
    {
        _transform_line_4(lineno, x, len, srcp);
    }
    else
    {
        _transform_line_8(lineno, x, len, srcp);
    }

    /* Redraw the cursor if it has been erased */
    if (redraw_cursor)
        PDC_private_cursor_on(PDC_state.cursor_row, PDC_state.cursor_col);

}

void PDC_doupdate(void)
{
}

static unsigned long _get_colors(chtype glyph)
{
    attr_t attr;
    short fore, back;

    /* Get the foreground and background colors */
    attr = glyph & (A_ATTRIBUTES ^ A_ALTCHARSET);
    pair_content(PAIR_NUMBER(attr), &fore, &back);

    if ((attr & A_BOLD) != 0 && fore < 16)
        fore |= 8;
    if ((attr & A_BLINK) != 0 && back < 16)
        back |= 8;

    if (attr & A_REVERSE)
    {
        short swap = fore;
        fore = back;
        back = swap;
    }

    /* Return them as a pair */
    return (back << 16) | fore;
}

static void _cursor_off_4(void)
{
    unsigned long addr = _address_4(PDC_state.cursor_row, PDC_state.cursor_col);
    unsigned plane;
    unsigned line;

    for (plane = 0; plane < 4; plane++)
    {
        unsigned long p = addr;
        outportb(0x03C4, 2);
        outportb(0x03C5, 1 << plane);
        for (line = 0; line < _FONT16; line++)
        {
            _video_write_byte(p, bytes_behind[plane][line]);
            p += PDC_state.bytes_per_line;
        }
    }
}

static void _cursor_off_8(void)
{
    unsigned long addr = _address_8(PDC_state.cursor_row, PDC_state.cursor_col);
    unsigned long p;
    unsigned line;
    unsigned i;

    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            _video_write_byte(p + i, bytes_behind[i][line]);
        p += PDC_state.bytes_per_line;
    }
}

static void _cursor_off_16(void)
{
    unsigned long addr = _address_8(PDC_state.cursor_row, PDC_state.cursor_col);
    unsigned long p;
    unsigned line;
    unsigned i;

    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            _video_write_word(p + i*2, bytes_behind[i][line]);
        p += PDC_state.bytes_per_line;
    }
}

static void _cursor_off_24(void)
{
    unsigned long addr = _address_8(PDC_state.cursor_row, PDC_state.cursor_col);
    unsigned long p;
    unsigned line;
    unsigned i;

    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            _video_write_3byte(p + i*3, bytes_behind[i][line]);
        p += PDC_state.bytes_per_line;
    }
}

static void _cursor_off_32(void)
{
    unsigned long addr = _address_8(PDC_state.cursor_row, PDC_state.cursor_col);
    unsigned long p;
    unsigned line;
    unsigned i;

    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            _video_write_dword(p + i*4, bytes_behind[i][line]);
        p += PDC_state.bytes_per_line;
    }
}

void PDC_private_cursor_off(void)
{
    if (PDC_state.cursor_visible && PDC_state.cursor_row < (unsigned)LINES
    &&  PDC_state.cursor_col < (unsigned)COLS)
    {
        switch (PDC_state.bits_per_pixel)
        {
        case 4:
            _cursor_off_4();
            break;

        case 8:
            _cursor_off_8();
            break;

        case 15:
        case 16:
            _cursor_off_16();
            break;

        case 24:
            _cursor_off_24();
            break;

        case 32:
            _cursor_off_32();
            break;
        }
    }
    PDC_state.cursor_visible = FALSE;
}

void _cursor_on_4(int row, int col)
{
    unsigned long addr = _address_4(row, col);
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
            bytes_behind[plane][line] = _video_read_byte(p);
            p += PDC_state.bytes_per_line;
        }
    }
    outportb(0x03C5, 2);
    outportb(0x03C5,  cursor_color);
    p = addr + PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        _video_write_byte(p, 0xFF);
        p += PDC_state.bytes_per_line;
    }
    outportb(0x03C5, 2);
    outportb(0x03C5, ~cursor_color);
    p = addr + PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        _video_write_byte(p, 0x00);
        p += PDC_state.bytes_per_line;
    }
    outportb(0x03C5, 2);
    outportb(0x03C5, 0xF);
}

void _cursor_on_8(int row, int col)
{
    unsigned long addr = _address_8(row, col);
    unsigned long p;
    unsigned line;
    unsigned i;

    /* Save the bytes currently in the character cell */
    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            bytes_behind[i][line] = _video_read_byte(p + i);
        p += PDC_state.bytes_per_line;
    }

    /* Write the cursor */
    p = addr + PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        for (i = 0; i < 8; i++)
            _video_write_byte(p + i, cursor_color);
        p += PDC_state.bytes_per_line;
    }
}

void _cursor_on_16(int row, int col)
{
    unsigned long addr = _address_8(row, col);
    unsigned long p;
    unsigned line;
    unsigned i;
    unsigned short color = PDC_state.colors[cursor_color].mapped;

    /* Save the bytes currently in the character cell */
    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            bytes_behind[i][line] = _video_read_word(p + i*2);
        p += PDC_state.bytes_per_line;
    }

    /* Write the cursor */
    p = addr + PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        for (i = 0; i < 8; i++)
            _video_write_word(p + i*2, color);
        p += PDC_state.bytes_per_line;
    }
}

void _cursor_on_24(int row, int col)
{
    unsigned long addr = _address_8(row, col);
    unsigned long p;
    unsigned line;
    unsigned i;
    unsigned long color = PDC_state.colors[cursor_color].mapped;

    /* Save the bytes currently in the character cell */
    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            bytes_behind[i][line] = _video_read_3byte(p + i*3);
        p += PDC_state.bytes_per_line;
    }

    /* Write the cursor */
    p = addr + PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        for (i = 0; i < 8; i++)
            _video_write_3byte(p + i*3, color);
        p += PDC_state.bytes_per_line;
    }
}

void _cursor_on_32(int row, int col)
{
    unsigned long addr = _address_8(row, col);
    unsigned long p;
    unsigned line;
    unsigned i;
    unsigned long color = PDC_state.colors[cursor_color].mapped;

    /* Save the bytes currently in the character cell */
    p = addr;
    for (line = 0; line < _FONT16; line++)
    {
        for (i = 0; i < 8; i++)
            bytes_behind[i][line] = _video_read_dword(p + i*4);
        p += PDC_state.bytes_per_line;
    }

    /* Write the cursor */
    p = addr + PDC_state.cursor_start * PDC_state.bytes_per_line;
    for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
    {
        for (i = 0; i < 8; i++)
            _video_write_dword(p + i*4, color);
        p += PDC_state.bytes_per_line;
    }
}

void PDC_private_cursor_on(int row, int col)
{
    switch (PDC_state.bits_per_pixel)
    {
    case 4:
        _cursor_on_4(row, col);
        break;

    case 8:
        _cursor_on_8(row, col);
        break;

    case 15:
    case 16:
        _cursor_on_16(row, col);
        break;

    case 24:
        _cursor_on_24(row, col);
        break;

    case 32:
        _cursor_on_32(row, col);
        break;
    }
    PDC_state.cursor_visible = TRUE;
    PDC_state.cursor_row = row;
    PDC_state.cursor_col = col;
}

static unsigned long _address_4(int row, int col)
{
    return row * PDC_state.bytes_per_line * _FONT16 + col;
}

static unsigned long _address_8(int row, int col)
{
    return row * PDC_state.bytes_per_line * _FONT16
            + col * 8 * ((PDC_state.bits_per_pixel + 7)/8);
}

static void _video_write_byte(unsigned long addr, unsigned char byte)
{
    unsigned offset = _set_window(PDC_state.write_win, addr);
    unsigned long addr2 = PDC_state.window[PDC_state.write_win]*16L + offset;
    setdosmembyte(addr2, byte);
}

static void _video_write_word(unsigned long addr, unsigned short word)
{
    unsigned offset = _set_window(PDC_state.write_win, addr);
    unsigned long addr2 = PDC_state.window[PDC_state.write_win]*16L + offset;
    setdosmemword(addr2, word);
}

static void _video_write_3byte(unsigned long addr, unsigned long byte3)
{
    /* Write a byte and a word. Split the write so that the word lies at an
     * even address, so it does not cross a window boundary */
    if (addr & 1)
    {
        _video_write_byte(addr + 0, byte3 & 0xFF);
        _video_write_word(addr + 1, byte3 >> 8);
    }
    else
    {
        _video_write_word(addr + 0, byte3 & 0xFFFF);
        _video_write_byte(addr + 2, byte3 >> 8);
    }
}

static void _video_write_dword(unsigned long addr, unsigned long dword)
{
    unsigned offset = _set_window(PDC_state.write_win, addr);
    unsigned long addr2 = PDC_state.window[PDC_state.write_win]*16L + offset;
    setdosmemdword(addr2, dword);
}

static unsigned char _video_read_byte(unsigned long addr)
{
    unsigned offset = _set_window(PDC_state.read_win, addr);
    unsigned long addr2 = PDC_state.window[PDC_state.read_win]*16L + offset;
    unsigned char byte = getdosmembyte(addr2);
    return byte;
}

static unsigned short _video_read_word(unsigned long addr)
{
    unsigned offset = _set_window(PDC_state.read_win, addr);
    unsigned long addr2 = PDC_state.window[PDC_state.read_win]*16L + offset;
    unsigned short word = getdosmembyte(addr2);
    return word;
}

static unsigned long _video_read_3byte(unsigned long addr)
{
    /* Read a byte and a word. Split the read so that the word lies at an
     * even address, so it does not cross a window boundary */
    unsigned long byte3;
    if (addr & 1)
    {
        byte3 = _video_read_byte(addr + 0);
        byte3 += (unsigned long)_video_read_word(addr + 1) << 8;
    }
    else
    {
        byte3 = _video_read_word(addr + 0);
        byte3 += (unsigned long)_video_read_byte(addr + 2) << 16;
    }
    return byte3;
}

static unsigned long _video_read_dword(unsigned long addr)
{
    unsigned offset = _set_window(PDC_state.read_win, addr);
    unsigned long addr2 = PDC_state.window[PDC_state.read_win]*16L + offset;
    unsigned long dword = getdosmemdword(addr2);
    return dword;
}

static unsigned _set_window(unsigned window, unsigned long addr)
{
    unsigned long offset = PDC_state.offset[window];
    if (addr < offset || offset + PDC_state.window_size <= addr)
    {
        /* Need to move the window */
        __dpmi_regs regs;
        unsigned long gran = PDC_state.window_gran * 1024L;

        memset(&regs, 0, sizeof(regs));
        regs.x.ax = 0x4F05;
        regs.x.bx = window;
        regs.x.dx = addr / gran;
        offset = regs.x.dx * gran;
        __dpmi_int(0x10, &regs);
        PDC_state.offset[window] = offset;
    }

    return addr - offset;
}
