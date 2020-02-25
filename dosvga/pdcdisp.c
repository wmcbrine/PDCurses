/* PDCurses */

#include "pdcdos.h"
#define USE_UNICODE_ACS_CHARS 0
#include "../common/acs_defs.h"

#ifdef __DJGPP__
#include <go32.h>
#include <sys/farptr.h>
#endif

#ifdef __WATCOMC__
/* For Watcom, we need conio.h, but the Curses macro for getch fouls it up */
#undef getch
#include <conio.h>
#define outportb(a, b) outp((a), (b))
#endif

/* Maximum run of characters per call to _transform_line_[48] */
#ifdef PDC_FLAT
#   define MAX_PACKET 512 /* optimize for speed */
#else
#   define MAX_PACKET 32  /* keep stack usage reasonable */
#endif

/* Support cursor on graphics mode */
static unsigned long bytes_behind[8][16];
static unsigned char cursor_color = 15;
extern unsigned char *PDC_font_bytes;
extern int PDC_font_height, PDC_font_width;

static unsigned long _get_colors(chtype glyph);
static unsigned long _address_4(int row, int col);
static unsigned long _address_8(int row, int col);
static void _video_write_byte(unsigned long addr, unsigned char byte);
static void _video_write_word(unsigned long addr, unsigned short word);
static void _video_write_3byte(unsigned long addr, unsigned long byte3);
static void _video_write_dword(unsigned long addr, unsigned long dword);
static void _video_write(unsigned long addr, const void *data, size_t size);
static unsigned char _video_read_byte(unsigned long addr);
static unsigned short _video_read_word(unsigned long addr);
static unsigned long _video_read_3byte(unsigned long addr);
static unsigned long _video_read_dword(unsigned long addr);
static unsigned _set_window(unsigned window, unsigned long addr);
#if defined(__WATCOMC__) && defined(__386__)
static unsigned char _farpeekb(unsigned seg, unsigned long off);
static unsigned short _farpeekw(unsigned seg, unsigned long off);
static unsigned long _farpeekl(unsigned seg, unsigned long off);
static void _farpokeb(unsigned seg, unsigned long off, unsigned char byte);
static void _farpokew(unsigned seg, unsigned long off, unsigned short word);
static void _farpokel(unsigned seg, unsigned long off, unsigned long dword);
#endif

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
    unsigned char bytes[16][MAX_PACKET];
    unsigned fore, back;
    int underline;
    unsigned long addr = _address_4(lineno, x);
    unsigned long cp;
    int col;
    unsigned line;
    unsigned vplane;

    fore = colors & 0xF;
    back = (colors >> 16) & 0xF;

    /* Underline will go here if requested */
    underline = 13 /* PDC_font_height */;

    /* Render to the bytes array and then copy to the frame buffer */
    /* Loop by column */
    for (col = 0; col < len; col++)
    {
        chtype glyph = srcp[col];
        int ch;
        unsigned char lr_mask;
        unsigned char *font_data;

        /* Get the index into the font */
        ch = glyph & 0xFF;
        if ((glyph & A_ALTCHARSET) != 0 && (glyph & 0xff80) == 0)
            ch = acs_map[ch & 0x7f] & 0xff;

        font_data = (unsigned char *)PDC_font_bytes;

        /* Set pixels for A_LEFT and A_RIGHT */
        lr_mask = 0x00;
        if (glyph & A_LEFT)
            lr_mask |= 0x80;
        if (glyph & A_RIGHT)
            lr_mask |= 0x01;

        font_data = PDC_font_bytes + ch * PDC_font_height;

        /* Copy font data into the bytes array */
        for (line = 0; line < PDC_font_height; line++)
        {
            unsigned char byte = font_data[line];
            bytes[line][col] = byte | lr_mask;
        }
        if (glyph & A_UNDERLINE)
            bytes[underline][col] = 0xFF;
    }

    /* Draw the text in four passes, to optimize for the memory architecture
       of the VGA in four bit pixel modes */

    vplane =  fore & ~back;
    if (vplane != 0)
    {
        /* fore has 1, back has 0: copy bytes as rendered */
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        for (line = 0; line < PDC_font_height; line++)
        {
            _video_write(cp, bytes[line], len);
            cp += PDC_state.bytes_per_line;
        }
    }

    vplane = ~fore &  back;
    if (vplane != 0)
    {
        /* fore has 0, back has 1: copy bytes inverted */
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        for (line = 0; line < PDC_font_height; line++)
        {
            for (col = 0; col < len; col++)
                bytes[line][col] ^= 0xFF;
            _video_write(cp, bytes[line], len);
            cp += PDC_state.bytes_per_line;
        }
    }

    vplane = ~fore & ~back;
    if (vplane != 0)
    {
        /* fore has 0, back has 0: write 0x00 */
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        memset(bytes[0], 0x00, len);
        for (line = 0; line < PDC_font_height; line++)
        {
            _video_write(cp, bytes[0], len);
            cp += PDC_state.bytes_per_line;
        }
    }

    vplane =  fore &  back;
    if (vplane != 0)
    {
        /* fore has 1, back has 1: write 0xFF */
        outportb(0x3c4, 2);
        outportb(0x3c5, vplane);
        cp = addr;
        memset(bytes[0], 0xFF, len);
        for (line = 0; line < PDC_font_height; line++)
        {
            _video_write(cp, bytes[0], len);
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

        if (colors != old_colors || i >= MAX_PACKET)
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
    unsigned char bytes[MAX_PACKET*4*8];
    unsigned long addr = _address_8(lineno, x);
    unsigned bytes_per_pixel = (PDC_state.bits_per_pixel + 7) / 8;
    unsigned long cp;

    struct
    {
        unsigned char *font_ptr;
        unsigned char lr_mask;
        unsigned char ul_mask;
        unsigned long fore;
        unsigned long back;
    } glyphs[MAX_PACKET];

    int col;
    unsigned line;
    unsigned underline;

    /* Underline will go here if requested */
    underline = 13 /*PDC_font_height*/;

    /* Compute basic glyph data only once per character */
    for (col = 0; col < len; col++)
    {
        chtype glyph = srcp[col];
        unsigned ch;
        unsigned long colors, fore, back;

        /* Get the index into the font */
        ch = glyph & 0xFF;
        if ((glyph & A_ALTCHARSET) != 0 && (glyph & 0xff80) == 0)
            ch = acs_map[ch & 0x7f] & 0xff;

        /* Get the address of the font data */
        glyphs[col].font_ptr = PDC_font_bytes + ch*PDC_font_height;

        /* Bit mask for underline */
        glyphs[col].ul_mask = (glyph & A_UNDERLINE) ? 0xFF : 0x00;

        /* Bit mask for A_LEFT and A_RIGHT */
        glyphs[col].lr_mask = 0x00;
        if (glyph & A_LEFT)
            glyphs[col].lr_mask |= 0x80;
        if (glyph & A_RIGHT)
            glyphs[col].lr_mask |= 0x01;

        /* Get the colors */
        colors = _get_colors(glyph);
        fore = colors & 0xFFFF;
        back = (colors >> 16) & 0xFFFF;
        if (PDC_state.bits_per_pixel > 8)
        {
            fore = PDC_state.colors[fore].mapped;
            back = PDC_state.colors[back].mapped;
        }
        glyphs[col].fore = fore;
        glyphs[col].back = back;
    }

    /* Loop by raster line */
    cp = addr;
    for (line = 0; line < PDC_font_height; line++)
    {
        unsigned bindex;

        /* Loop by column */
        bindex = 0;
        for (col = 0; col < len; col++)
        {
            /* Get glyph data */
            unsigned char *font_ptr = glyphs[col].font_ptr;
            unsigned long fore = glyphs[col].fore;
            unsigned long back = glyphs[col].back;
            unsigned byte, bit;

            /* Get one byte of the font data */
            byte = font_ptr[line] | glyphs[col].lr_mask;
            if (line == underline)
                byte |= glyphs[col].ul_mask;

            /* Render one raster line of the glyph */
            switch (bytes_per_pixel)
            {
            case 1:
                for (bit = 0x80; bit != 0; bit >>= 1)
                {
                    unsigned long color = (byte & bit) ? fore : back;
                    bytes[bindex++] = (unsigned char)color;
                }
                break;

            case 2:
                for (bit = 0x80; bit != 0; bit >>= 1)
                {
                    unsigned long color = (byte & bit) ? fore : back;
                    *(unsigned short *)(bytes+bindex) = (unsigned short)color;
                    bindex += 2;
                }
                break;

            default:
                /* In 24 bit color mode, each pass writes one extra byte at
                   the end, which is overwritten by the next pixel and leaves
                   an extra unused byte. This will need to change if this
                   code is ever used on a big endian machine. */
                for (bit = 0x80; bit != 0; bit >>= 1)
                {
                    unsigned long color = (byte & bit) ? fore : back;
                    *(unsigned long *)(bytes+bindex) = color;
                    bindex += bytes_per_pixel;
                }
                break;
            }
        }
        _video_write(cp, bytes, bindex);
        cp += PDC_state.bytes_per_line;
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
        while (len > MAX_PACKET)
        {
            _transform_line_8(lineno, x, MAX_PACKET, srcp);
            x += MAX_PACKET;
            len -= MAX_PACKET;
            srcp += MAX_PACKET;
        }
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
    return ((unsigned long)back << 16) | fore;
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
        for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
        for (line = 0; line < PDC_font_height; ++line)
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
    for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
    for (line = 0; line < PDC_font_height; line++)
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
    return (unsigned long)row * PDC_state.bytes_per_line * PDC_font_height + col;
}

static unsigned long _address_8(int row, int col)
{
    return (unsigned long)row * PDC_state.bytes_per_line * PDC_font_height
            + col * 8 * ((PDC_state.bits_per_pixel + 7)/8);
}

static void _video_write_byte(unsigned long addr, unsigned char byte)
{
#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
        _farpokeb(PDC_state.linear_sel, addr, byte);
    }
    else
#endif
    {
        unsigned offset = _set_window(PDC_state.write_win, addr);
        unsigned long addr2 = (unsigned long)_FAR_POINTER(PDC_state.window[PDC_state.write_win], offset);
        setdosmembyte(addr2, byte);
    }
}

static void _video_write_word(unsigned long addr, unsigned short word)
{
#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
        _farpokew(PDC_state.linear_sel, addr, word);
    }
    else
#endif
    {
        unsigned offset = _set_window(PDC_state.write_win, addr);
        unsigned long addr2 = (unsigned long)_FAR_POINTER(PDC_state.window[PDC_state.write_win], offset);
        setdosmemword(addr2, word);
    }
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
        _video_write_byte(addr + 2, byte3 >> 16);
    }
}

static void _video_write_dword(unsigned long addr, unsigned long dword)
{
#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
        _farpokel(PDC_state.linear_sel, addr, dword);
    }
    else
#endif
    {
        unsigned offset = _set_window(PDC_state.write_win, addr);
        unsigned long addr2 = (unsigned long)_FAR_POINTER(PDC_state.window[PDC_state.write_win], offset);
        setdosmemdword(addr2, dword);
    }
}

static void _video_write(unsigned long addr, const void *data, size_t size)
{
#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
#ifdef __DJGPP__
        movedata(_go32_my_ds(), (unsigned)data,
                 PDC_state.linear_sel, addr, size);
#else /* Watcom */
        _fmemcpy(MK_FP(PDC_state.linear_sel, addr), data, size);
#endif
    }
    else
#endif
    {
        unsigned window = PDC_state.window[PDC_state.write_win];

        while (size != 0)
        {
            unsigned offset = _set_window(PDC_state.write_win, addr);
            unsigned long size2 = PDC_state.window_size - offset;
            if (size2 > size)
                size2 = size;
#ifdef __DJGPP__
            dosmemput(data, size2, window * 16L + offset);
#elif defined(__WATCOMC__) && defined(__386__)
            _fmemcpy((void PDC_FAR *)(window * 16L + offset), data, size2);
#else
            if (size2 >= 65532)
                size2 = 65532;

            _fmemcpy(MK_FP(window, offset), data, size2);
#endif
            addr += size2;
            data = (const char *)data + size2;
            size -= size2;
        }
    }
}

static unsigned char _video_read_byte(unsigned long addr)
{
    unsigned char byte;

#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
        byte = _farpeekb(PDC_state.linear_sel, addr);
    }
    else
#endif
    {
        unsigned offset = _set_window(PDC_state.read_win, addr);
        unsigned long addr2 = (unsigned long)_FAR_POINTER(PDC_state.window[PDC_state.read_win], offset);
        byte = getdosmembyte(addr2);
    }
    return byte;
}

static unsigned short _video_read_word(unsigned long addr)
{
    unsigned short word;

#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
        word = _farpeekw(PDC_state.linear_sel, addr);
    }
    else
#endif
    {
        unsigned offset = _set_window(PDC_state.read_win, addr);
        unsigned long addr2 = (unsigned long)_FAR_POINTER(PDC_state.window[PDC_state.read_win], offset);
        word = getdosmemword(addr2);
    }
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
    unsigned long dword;

#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
        dword = _farpeekl(PDC_state.linear_sel, addr);
    }
    else
#endif
    {
        unsigned offset = _set_window(PDC_state.read_win, addr);
        unsigned long addr2 = (unsigned long)_FAR_POINTER(PDC_state.window[PDC_state.read_win], offset);
        dword = getdosmemdword(addr2);
    }
    return dword;
}

static unsigned _set_window(unsigned window, unsigned long addr)
{
    unsigned long offset = PDC_state.offset[window];
    if (addr < offset || offset + PDC_state.window_size <= addr)
    {
        /* Need to move the window */
        PDCREGS regs;
        unsigned long gran = PDC_state.window_gran * 1024L;

        memset(&regs, 0, sizeof(regs));
        regs.W.ax = 0x4F05;
        regs.W.bx = window;
        regs.W.dx = addr / gran;
        offset = regs.W.dx * gran;
        PDCINT(0x10, regs);
        PDC_state.offset[window] = offset;
    }

    return addr - offset;
}

#if defined(__WATCOMC__) && defined(__386__)
/* Far peek and poke, as defined for DJGPP, made available for Watcom */
static unsigned char _farpeekb(unsigned seg, unsigned long off)
{
    return *(unsigned char __far *)MK_FP(seg, off);
}

static unsigned short _farpeekw(unsigned seg, unsigned long off)
{
    return *(unsigned short __far *)MK_FP(seg, off);
}

static unsigned long _farpeekl(unsigned seg, unsigned long off)
{
    return *(unsigned long __far *)MK_FP(seg, off);
}

static void _farpokeb(unsigned seg, unsigned long off, unsigned char byte)
{
    *(unsigned char __far *)MK_FP(seg, off) = byte;
}

static void _farpokew(unsigned seg, unsigned long off, unsigned short word)
{
    *(unsigned short __far *)MK_FP(seg, off) = word;
}

static void _farpokel(unsigned seg, unsigned long off, unsigned long dword)
{
    *(unsigned long __far *)MK_FP(seg, off) = dword;
}
#endif
