/* PDCurses */

#include "pdcdos.h"
#ifdef PDC_WIDE
# include "../common/acsuni.h"
#else
# include "../common/acs437.h"
#endif

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

static unsigned long _get_colors(chtype glyph);
static unsigned long _address_4(int row, int col);
static unsigned long _address_8(int row, int col);
static void _video_read(void *data, unsigned long addr, size_t size);
static void _video_write(unsigned long addr, const void *data, size_t size);
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
    struct
    {
        const unsigned char *font_addr;
    } glyphs[MAX_PACKET];
    int i;

    /* Colors */
    unsigned fore = colors & 0xF;
    unsigned back = (colors >> 16) & 0xF;

    /* Width of a character in bytes */
    unsigned ch_width = (PDC_state.font_width + 7) / 8;

    /* Bit to set for A_RIGHT */
    unsigned char r_mask = 0x80 >> ((PDC_state.font_width - 1) % 8);

    /* Bytes to write in a single pass */
    unsigned char bytes[1024];

    /* Which plane we're writing */
    unsigned plane;

    /* Compute these just once */
    for (i = 0; i < len; ++i)
    {
        chtype glyph = srcp[i];
        unsigned ch = glyph & A_CHARTEXT;
        if ((glyph & A_ALTCHARSET) != 0 && (ch & 0xff80) == 0)
            ch = acs_map[ch];
        glyphs[i].font_addr = PDC_state.font_glyph_data(FALSE, ch);
    }

    if (PDC_state.font_width % 8 == 0)
    {
        /* Faster algorithm for use when we're always writing whole bytes */

        /* Write data twice: once direct, once inverted */
        for (plane = 0; plane < 2; ++plane)
        {
            unsigned xpix = x * PDC_state.font_width;
            unsigned long addr = _address_4(lineno, xpix / 8);
            unsigned plane_bit;
            unsigned char invert;
            unsigned line;

            if (plane == 0)
            {
                plane_bit = fore & ~back & 0xF;
                invert = 0x00;
            }
            else
            {
                plane_bit = ~fore & back & 0xF;
                invert = 0xFF;
            }
            if (plane_bit == 0)
                continue;

            /* Select the planes */
            outportb(0x3c4, 2);
            outportb(0x3c5, plane_bit);

            /* Loop once per raster line */
            for (line = 0; line < PDC_state.font_height; ++line)
            {
                unsigned long cp;
                unsigned num_bytes = 0;
                int col;

                /* Loop once per column */
                cp = addr;
                for (col = 0; col < len; ++col)
                {
                    chtype glyph = srcp[col];
                    const unsigned char *font_data;
                    unsigned ch_byte;

                    font_data = glyphs[col].font_addr;

                    /* Loop once per byte within the font */
                    for (ch_byte = 0; ch_byte < ch_width; ++ch_byte)
                    {
                        unsigned index = line * ch_width + ch_byte;
                        unsigned char byte = font_data[index];
                        if ((glyph & A_UNDERLINE) && line == PDC_state.underline)
                            byte |= 0xFF;
                        if ((glyph & A_LEFT) != 0 && ch_byte == 0)
                            byte |= 0x80;
                        if ((glyph & A_RIGHT) != 0 && ch_byte + 1 == ch_width)
                            byte |= r_mask;
                        bytes[num_bytes++] = byte ^ invert;
                        if (num_bytes >= sizeof(bytes))
                        {
                            /* Flush the bytes array to the frame buffer */
                            _video_write(cp, bytes, num_bytes);
                            cp += num_bytes;
                            num_bytes = 0;
                        }
                    }
                }
                /* Write any remaining bytes */
                _video_write(cp, bytes, num_bytes);

                addr += PDC_state.bytes_per_line;
            }
        }

        /* Write fill bytes: once all zeros, once all ones */
        for (plane = 0; plane < 2; ++plane)
        {
            unsigned xpix = x * PDC_state.font_width;
            unsigned long addr = _address_4(lineno, xpix / 8);
            unsigned plane_bit;
            unsigned char fill;
            unsigned width;
            unsigned line;

            if (plane == 0)
            {
                plane_bit = ~fore & ~back & 0xF;
                fill = 0x00;
            }
            else
            {
                plane_bit = fore & back & 0xF;
                fill = 0xFF;
            }
            if (plane_bit == 0)
                continue;

            /* Select the planes */
            outportb(0x3c4, 2);
            outportb(0x3c5, plane_bit);

            /* Fill with bytes to be written */
            width = ch_width * len;
            memset(bytes, fill, (width < sizeof(bytes)) ? width : sizeof(bytes));

            /* Loop once per raster line */
            for (line = 0; line < PDC_state.font_height; ++line)
            {
                unsigned count = width;
                unsigned long cp = addr;
                /* Fill */
                while (count > sizeof(bytes))
                {
                    _video_write(cp, bytes, sizeof(bytes));
                    cp += sizeof(bytes);
                    count -= sizeof(bytes);
                }
                _video_write(cp, bytes, count);
                addr += PDC_state.bytes_per_line;
            }
        }
    }
    else
    {
        /* Loop once per plane */
        for (plane = 0; plane < 4; ++plane)
        {
            unsigned xpix = x * PDC_state.font_width;
            unsigned long addr = _address_4(lineno, xpix / 8);
            unsigned plane_bit = 1 << plane;
            unsigned cbits;
            unsigned line;

            /* Select the plane */
            outportb(0x3c4, 2);
            outportb(0x3c5, plane_bit);
            outportb(0x3ce, 4);
            outportb(0x3cf, plane);

            /* Select the mask for the current plane */
            cbits = ((fore & plane_bit) ? 1 : 0)
                  | ((back & plane_bit) ? 2 : 0);

            /* Loop once per raster line */
            for (line = 0; line < PDC_state.font_height; ++line)
            {
                unsigned shift = xpix % 8;
                unsigned long cp;
                unsigned num_bytes = 0;
                int col;
                unsigned ch_byte;

                if (shift != 0)
                {
                    /* We need the existing data from the frame buffer */
                    _video_read(bytes, addr, 1);
                    bytes[0] &= 0xFF00 >> shift;
                }
                else
                {
                    bytes[0] = 0;
                }

                col = 0;
                ch_byte = 0;
                cp = addr;
                while (col < len)
                {
                    /* Render one byte from the current character */
                    chtype glyph = srcp[col];
                    const unsigned char *font_data = glyphs[col].font_addr;
                    unsigned char byte;
                    unsigned num_bits;

                    /* Get one byte of pixel data for the current plane */
                    if (cbits == 1 || cbits == 2)
                    {
                        unsigned index = line * ch_width + ch_byte;
                        byte = font_data[index];
                        if ((glyph & A_UNDERLINE) && line == PDC_state.underline)
                            byte |= 0xFF;
                        if ((glyph & A_LEFT) != 0 && ch_byte == 0)
                            byte |= 0x80;
                        if ((glyph & A_RIGHT) != 0 && ch_byte + 1 == ch_width)
                            byte |= r_mask;
                    }
                    else
                    {
                        byte = 0x00;
                    }
                    if (cbits & 2)
                        byte ^= 0xFF;

                    /* Get the number of bits to render in this pass */
                    if (ch_byte + 1 < ch_width)
                        num_bits = 8;
                    else
                    {
                        num_bits = PDC_state.font_width - ch_byte * 8;
                        byte &= 0xFF00 >> num_bits;
                    }

                    /* Add the bits to the bytes array */
                    bytes[num_bytes] |= byte >> shift;
                    byte <<= 8 - shift;
                    shift += num_bits;
                    if (shift >= 8)
                    {
                        ++num_bytes;
                        shift -= 8;
                        if (num_bytes >= sizeof(bytes))
                        {
                            /* Flush the bytes array to the frame buffer */
                            _video_write(cp, bytes, num_bytes);
                            cp += num_bytes;
                            num_bytes = 0;
                        }
                        bytes[num_bytes] = byte;
                    }

                    /* Advance to the next byte of the character */
                    ++ch_byte;
                    if (ch_byte >= ch_width)
                    {
                        ch_byte = 0;
                        ++col;
                    }
                }
                if (num_bytes != 0)
                {
                    /* There are bytes remaining in the bytes array */
                    _video_write(cp, bytes, num_bytes);
                    cp += num_bytes;
                    bytes[0] = bytes[num_bytes];
                }
                if (shift != 0)
                {
                    /* There are bits remaining in the bytes array */
                    unsigned char b;
                    _video_read(&b, cp, 1);
                    bytes[0] |= b & (0xFF >> shift);
                    _video_write(cp, bytes, 1);
                }

                addr += PDC_state.bytes_per_line;
            }
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
    unsigned char bytes[1024];
    unsigned long addr = _address_8(lineno, x);
    unsigned bytes_per_pixel = (PDC_state.bits_per_pixel + 7) / 8;

    struct
    {
        const unsigned char *font_addr;
        unsigned char ul_mask;
        unsigned long fore;
        unsigned long back;
    } glyphs[MAX_PACKET];

    /* Width of a character in bytes */
    unsigned ch_width = (PDC_state.font_width + 7) / 8;

    /* Bit to set for A_RIGHT */
    unsigned char r_mask = 0x80 >> ((PDC_state.font_width - 1) % 8);

    int col;
    unsigned line;

    /* Compute basic glyph data only once per character */
    for (col = 0; col < len; col++)
    {
        chtype glyph = srcp[col];
        unsigned ch;
        unsigned long colors, fore, back;

        /* Get the index into the font */
        ch = glyph & 0xFFFF;
        if ((glyph & A_ALTCHARSET) != 0 && (glyph & 0xff80) == 0)
            ch = acs_map[ch & 0x7f];

        /* Get the address of the font data */
        glyphs[col].font_addr = PDC_state.font_glyph_data(FALSE, ch);

        /* Bit mask for underline */
        glyphs[col].ul_mask = (glyph & A_UNDERLINE) ? 0xFF : 0x00;

        /* Get the colors */
        colors = _get_colors(glyph);
        fore = colors & 0xFFFF;
        back = (colors >> 16) & 0xFFFF;
        if (PDC_state.bits_per_pixel > 8)
        {
            fore = PDC_state.colors[fore].mapped;
            back = PDC_state.colors[back].mapped;
        }
        /*
         * If this is ever run on big endian hardware, we'll need to shift
         * fore and back left as follows:
         * fore <<= 32 - (bytes_per_pixel * 8);
         * back <<= 32 - (bytes_per_pixel * 8);
         */
        glyphs[col].fore = fore;
        glyphs[col].back = back;
    }

    /* Loop by raster line */
    for (line = 0; line < PDC_state.font_height; line++)
    {
        unsigned long cp = addr;
        unsigned bindex;

        /* Loop by column */
        bindex = 0;
        for (col = 0; col < len; col++)
        {
            /* Get glyph data */
            chtype glyph = srcp[col];
            const unsigned char *font_addr = glyphs[col].font_addr;
            unsigned index = line * ch_width;
            unsigned long fore = glyphs[col].fore;
            unsigned long back = glyphs[col].back;
            unsigned ch_byte;
            unsigned byte;

            /* Loop by byte within the font */
            for (ch_byte = 0; ch_byte < ch_width; ++ch_byte)
            {
                unsigned bit, num_bits;

                /* Get one byte of the font data */
                byte = font_addr[index + ch_byte];
                if ((glyph & A_LEFT) != 0 && ch_byte == 0)
                    byte |= 0x80;
                if ((glyph & A_RIGHT) != 0 && ch_byte + 1 == ch_width)
                    byte |= r_mask;
                if (line == PDC_state.underline)
                    byte |= glyphs[col].ul_mask;
                if (PDC_state.cursor_visible
                &&  lineno == PDC_state.cursor_row
                &&  col + x == PDC_state.cursor_col
                &&  PDC_state.cursor_start <= line && line <= PDC_state.cursor_end)
                    byte ^= 0xFF;

                /* Number of pixels to render on this pass */
                if (ch_byte + 1 == ch_width)
                    num_bits = PDC_state.font_width - ch_byte * 8;
                else
                    num_bits = 8;

                /* Render one byte's worth of pixels of the glyph */
                for (bit = 0; bit < num_bits; ++bit)
                {
                    unsigned long color = (byte & (0x80 >> bit)) ? fore : back;
                    *(unsigned long *)(bytes + bindex) = color;
                    bindex += bytes_per_pixel;
                    if (bindex + sizeof(unsigned long) > sizeof(bytes))
                    {
                        /* Flush bytes to the frame buffer */
                        _video_write(cp, bytes, bindex);
                        cp += bindex;
                        bindex = 0;
                    }
                }
            }
        }
        _video_write(cp, bytes, bindex);
        addr += PDC_state.bytes_per_line;
    }
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

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

void PDC_private_cursor_off(void)
{
    /* This gets called before atrtab is set up; avoid a null dereference */
    if (!SP || !SP->atrtab)
        return;

    PDC_state.cursor_visible = FALSE;
    if (PDC_state.cursor_row < (unsigned)LINES
    &&  PDC_state.cursor_col < (unsigned)COLS)
    {
        static const chtype space = ' ';

        PDC_transform_line(PDC_state.cursor_row, PDC_state.cursor_col, 1,
                (curscr && curscr->_y)
                        ? &curscr->_y[PDC_state.cursor_row][PDC_state.cursor_col]
                        : &space);
    }
}

void PDC_private_cursor_on(int row, int col)
{
    PDC_state.cursor_visible = TRUE;
    PDC_state.cursor_row = row;
    PDC_state.cursor_col = col;
    if (row < (unsigned)LINES && col < (unsigned)COLS)
    {
        static const chtype space = ' ';

        PDC_transform_line(PDC_state.cursor_row, PDC_state.cursor_col, 1,
                (curscr && curscr->_y)
                        ? &curscr->_y[PDC_state.cursor_row][PDC_state.cursor_col]
                        : &space);
    }
}

static unsigned long _address_4(int row, int col)
{
    return (unsigned long)row * PDC_state.bytes_per_line * PDC_state.font_height + col;
}

static unsigned long _address_8(int row, int col)
{
    return (unsigned long)row * PDC_state.bytes_per_line * PDC_state.font_height
            + col * PDC_state.font_width * ((PDC_state.bits_per_pixel + 7)/8);
}

static void _video_read(void *data, unsigned long addr, size_t size)
{
#ifdef PDC_FLAT
    if (PDC_state.linear_sel)
    {
#ifdef __DJGPP__
        movedata(PDC_state.linear_sel, addr,
                 _go32_my_ds(), (unsigned)data, size);
#else /* Watcom */
        _fmemcpy(data, MK_FP(PDC_state.linear_sel, addr), size);
#endif
    }
    else
#endif
    {
        unsigned window = PDC_state.window[PDC_state.read_win];

        while (size != 0)
        {
            unsigned offset = _set_window(PDC_state.read_win, addr);
            unsigned long size2 = PDC_state.window_size - offset;
            if (size2 > size)
                size2 = size;
#ifdef __DJGPP__
            dosmemget(window * 16L + offset, size2, data);
#elif defined(__WATCOMC__) && defined(__386__)
            _fmemcpy(data, (void PDC_FAR *)(window * 16L + offset), size2);
#else
            if (size2 >= 65532)
                size2 = 65532;

            _fmemcpy(data, MK_FP(window, offset), size2);
#endif
            addr += size2;
            data = (char *)data + size2;
            size -= size2;
        }
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
