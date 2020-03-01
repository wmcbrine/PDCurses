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
    underline = 13 /*PDC_state.font_height*/;

    /* Render to the bytes array and then copy to the frame buffer */
    /* Loop by column */
    for (col = 0; col < len; col++)
    {
        chtype glyph = srcp[col];
        int ch;
        const unsigned char PDC_FAR *font_data;
        unsigned char lr_mask;

        /* Get the index into the font */
        ch = glyph & 0xFFFF;
        if ((glyph & A_ALTCHARSET) != 0 && (glyph & 0xff80) == 0)
            ch = acs_map[ch & 0x7f];

        /* Get the font data */
        font_data = PDC_state.font_glyph_data(FALSE, ch);

        /* Set pixels for A_LEFT and A_RIGHT */
        lr_mask = 0x00;
        if (glyph & A_LEFT)
            lr_mask |= 0x80;
        if (glyph & A_RIGHT)
            lr_mask |= 0x01;

        /* Copy font data into the bytes array */
        for (line = 0; line < PDC_state.font_height; line++)
        {
            unsigned char byte = font_data[line];
            bytes[line][col] = byte | lr_mask;
        }
        if (glyph & A_UNDERLINE)
            bytes[underline][col] = 0xFF;
        if (PDC_state.cursor_visible
        &&  lineno == PDC_state.cursor_row
        &&  col + x == PDC_state.cursor_col)
        {
            for (line = PDC_state.cursor_start; line <= PDC_state.cursor_end; ++line)
                bytes[line][col] ^= 0xFF;
        }
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
        for (line = 0; line < PDC_state.font_height; line++)
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
        for (line = 0; line < PDC_state.font_height; line++)
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
        for (line = 0; line < PDC_state.font_height; line++)
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
        for (line = 0; line < PDC_state.font_height; line++)
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
        const unsigned char PDC_FAR *font_addr;
        unsigned char lr_mask;
        unsigned char ul_mask;
        unsigned long fore;
        unsigned long back;
    } glyphs[MAX_PACKET];

    int col;
    unsigned line;
    unsigned underline;

    /* Underline will go here if requested */
    underline = 13 /*PDC_state.font_height*/;

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
    for (line = 0; line < PDC_state.font_height; line++)
    {
        unsigned bindex;

        /* Loop by column */
        bindex = 0;
        for (col = 0; col < len; col++)
        {
            /* Get glyph data */
            const unsigned char PDC_FAR *font_addr = glyphs[col].font_addr;
            unsigned long fore = glyphs[col].fore;
            unsigned long back = glyphs[col].back;
            unsigned byte, bit;

            /* Get one byte of the font data */
            byte = font_addr[line] | glyphs[col].lr_mask;
            if (line == underline)
                byte |= glyphs[col].ul_mask;
            if (PDC_state.cursor_visible
            &&  lineno == PDC_state.cursor_row
            &&  col + x == PDC_state.cursor_col
            &&  PDC_state.cursor_start <= line && line <= PDC_state.cursor_end)
                byte ^= 0xFF;

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
