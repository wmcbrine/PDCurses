/* PDCurses */

#include "pdcdos.h"
#include "pdcvesa.h"

#include <stdlib.h>
#include <string.h>

/* TODO: support 8 bit palette registers if available */

struct PDC_video_state PDC_state;

static int saved_scrnmode[3];

static void _init_palette(void);
static void _load_palette(void);
static int _get_mode_info(unsigned mode, struct ModeInfoBlock *mode_info);
static unsigned _find_video_mode(int rows, int cols);
static unsigned _find_mode(
        struct ModeInfoBlock *mode_info,
        unsigned long mode_addr,
        int rows, int cols);
#if defined(__WATCOMC__) && defined(__386__)
static int __dpmi_allocate_dos_memory(int paras, int *sel_or_max);
static int __dpmi_free_dos_memory(int sel);
static void dosmemget(unsigned long addr, size_t size, void *buf);
static void dosmemput(const void *buf, size_t size, unsigned long addr);
#endif

/* _get_font_address() -- return the address of the font in ROM */
static unsigned long _get_font_address(void)
{
    /* TODO: support compilers other than DJGPP */
    unsigned ofs = getdosmemword(0x43 * 4 + 0);
    unsigned seg = getdosmemword(0x43 * 4 + 2);
    return ((unsigned long)seg << 4) + ofs;
}

/* _get_scrn_mode() - Return the current BIOS video mode */

static int _get_scrn_mode(void)
{
    PDCREGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x4F03;
    PDCINT(0x10, regs);
    if (regs.h.ah == 0) {
        return (int)regs.W.bx;
    }

    regs.h.ah = 0x0f;
    PDCINT(0x10, regs);

    return (int)regs.h.al;
}

/* _set_scrn_mode() - Sets the BIOS Video Mode Number */

static void _set_scrn_mode(int new_mode)
{
    PDCREGS regs;

    memset(&regs, 0, sizeof(regs));
    if (new_mode >= 0x100) {
        regs.W.ax = 0x4F02;
        regs.W.bx = new_mode;
    } else {
        regs.h.ah = 0;
        regs.h.al = (unsigned char) new_mode;
    }
    PDCINT(0x10, regs);
    PDC_state.scrn_mode = new_mode & 0x3FFF;

    PDC_state.font_addr = _get_font_address();
    LINES = PDC_get_rows();
    COLS = PDC_get_columns();
}

/* close the physical screen -- may restore the screen to its state
   before PDC_scr_open(); miscellaneous cleanup */

void PDC_scr_close(void)
{
    PDCREGS regs;

    _set_scrn_mode(0x03);
    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x1114;
    regs.h.bl = 0x00;
    PDCINT(0x10, regs);
}

void PDC_scr_free(void)
{
}

/* open the physical screen -- miscellaneous initialization, may save
   the existing screen for later restoration */

int PDC_scr_open(void)
{
#if SMALL || MEDIUM
    struct SREGS segregs;
    int ds;
#endif

    PDC_LOG(("PDC_scr_open() - called\n"));

    SP = calloc(1, sizeof(SCREEN));

    if (!SP)
        return ERR;

    _init_palette();
    PDC_resize_screen(25, 80);

    SP->orig_attr = FALSE;

    SP->mouse_wait = PDC_CLICK_PERIOD;
    SP->audible = TRUE;

    SP->mono = FALSE;
    SP->termattrs = A_COLOR | A_REVERSE | A_UNDERLINE;

    SP->_preserve = FALSE;

    return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
             nlines, ncols));

    _set_scrn_mode(_find_video_mode(nlines, ncols));
    _load_palette();
    SP->orig_cursor = PDC_get_cursor_mode();
    PDC_curs_set(SP->visibility);
    if (PDC_state.bits_per_pixel <= 8)
        COLORS = 1 << PDC_state.bits_per_pixel;
    else
        COLORS = PDC_MAXCOL;

    return OK;
}

void PDC_reset_prog_mode(void)
{
    PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
    _load_palette();
}

void PDC_reset_shell_mode(void)
{
        PDC_LOG(("PDC_reset_shell_mode() - called.\n"));
}

void PDC_restore_screen_mode(int i)
{
    if (i >= 0 && i <= 2)
    {
        _set_scrn_mode(saved_scrnmode[i]);
    }
}

void PDC_save_screen_mode(int i)
{
    if (i >= 0 && i <= 2)
    {
        saved_scrnmode[i] = _get_scrn_mode();
    }
}

/* _init_palette -- set up the initial palette */

static void _init_palette(void)
{
    unsigned i, r, g, b;

    /* The basic eight colors and their bold forms */
    for (i = 0; i < 8; i++)
    {
        PDC_state.colors[i+0].r = (i & COLOR_RED)   ?  750 :   0;
        PDC_state.colors[i+0].g = (i & COLOR_GREEN) ?  750 :   0;
        PDC_state.colors[i+0].b = (i & COLOR_BLUE)  ?  750 :   0;
        PDC_state.colors[i+8].r = (i & COLOR_RED)   ? 1000 : 250;
        PDC_state.colors[i+8].g = (i & COLOR_GREEN) ? 1000 : 250;
        PDC_state.colors[i+8].b = (i & COLOR_BLUE)  ? 1000 : 250;
    }

    /* 6x6x6 color cube */
    i = 16;
    for (r = 0; r < 6; r++)
    {
        for (g = 0; g < 6; g++)
        {
            for (b = 0; b < 6; b++)
            {
                PDC_state.colors[i].r = (r ? r * 157 + 215 : 0);
                PDC_state.colors[i].g = (g ? g * 157 + 215 : 0);
                PDC_state.colors[i].b = (b ? b * 157 + 215 : 0);
                i++;
            }
        }
    }

    /* 24 shades of gray */
    for (i = 232; i < 256; i++)
    {
        r = (i - 232) * 40 + 30;
        PDC_state.colors[i].r = r;
        PDC_state.colors[i].g = r;
        PDC_state.colors[i].b = r;
    }
}

/* _load_palette -- load the current palette into the DAC */

static void _load_palette(void)
{
    if (PDC_state.bits_per_pixel <= 8)
    {
        PDCREGS regs;
        unsigned i;

        /* Load EGA palette registers as an identity map */
        for (i = 0; i < 16; i++)
        {
            memset(&regs, 0, sizeof(regs));
            regs.W.ax = 0x1000;
            regs.W.bx = i * 0x0101;
            PDCINT(0x10, regs);
        }

        /* Load the DAC registers from the current palette */
        for (i = 0; i < 1 << PDC_state.bits_per_pixel; i++)
            PDC_init_color(i, PDC_state.colors[i].r, PDC_state.colors[i].g,
                    PDC_state.colors[i].b);
    }
    else
    {
        unsigned i;

        /* Set up mappings from color index to RGB */
        for (i = 0; i < PDC_MAXCOL; i++)
            PDC_init_color(i, PDC_state.colors[i].r, PDC_state.colors[i].g,
                    PDC_state.colors[i].b);
    }
}

bool PDC_can_change_color(void)
{
    return TRUE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    *red   = PDC_state.colors[color].r;
    *green = PDC_state.colors[color].g;
    *blue  = PDC_state.colors[color].b;

    return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    /* Scale */
    unsigned r = DIVROUND((unsigned)red * PDC_state.red_max, 1000);
    unsigned g = DIVROUND((unsigned)green * PDC_state.green_max, 1000);
    unsigned b = DIVROUND((unsigned)blue * PDC_state.blue_max, 1000);

    PDC_state.colors[color].r = red;
    PDC_state.colors[color].g = green;
    PDC_state.colors[color].b = blue;

    if (PDC_state.bits_per_pixel <= 8)
    {
        PDCREGS regs;

        if (PDC_state.scrn_mode >= 0x100)
        {
            /* VESA mode in use; first try the VESA function */
            int seg, sel;
            unsigned long table;

            seg = __dpmi_allocate_dos_memory(1, &sel);
            if (seg < 0)
                return ERR;
            table = ((unsigned long)b <<  0)
                  | ((unsigned long)g <<  8)
                  | ((unsigned long)r << 16);
            setdosmemdword(seg * 16L, table);
            memset(&regs, 0, sizeof(regs));
            regs.W.ax = 0x4F09;
            regs.h.bl = 0;
            regs.W.cx = 1;
            regs.W.dx = color;
            regs.W.di = 0;
            regs.W.es = seg;
            PDCINT(0x10, regs);
            __dpmi_free_dos_memory(sel);
            if (regs.W.ax == 0x004F)
                return OK;
        }

        regs.h.dh = r;
        regs.h.ch = g;
        regs.h.cl = b;

        /* Set single DAC register */

        regs.W.ax = 0x1010;
        regs.W.bx = color;

        PDCINT(0x10, regs);
    }
    else
    {
        PDC_state.colors[color].mapped =
                  ((unsigned long)r << PDC_state.red_pos)
                | ((unsigned long)g << PDC_state.green_pos)
                | ((unsigned long)b << PDC_state.blue_pos);
    }

    return OK;
}

static unsigned _find_video_mode(int rows, int cols)
{
    int vbe_info_sel = -1; /* custodial */
    int vbe_info_seg;
    struct VbeInfoBlock vbe_info;
    PDCREGS regs;
    unsigned long mode_addr;
    struct ModeInfoBlock mode_info;
    unsigned vesa_mode;

    /* Request VESA BIOS information */
    vbe_info_seg = __dpmi_allocate_dos_memory(
            (sizeof(vbe_info) + 15) / 16,
            &vbe_info_sel);
    if (vbe_info_seg < 0)
        goto error;

    memset(&vbe_info, 0, sizeof(vbe_info));
    memcpy(vbe_info.VbeSignature, "VBE2", 4);
    dosmemput(&vbe_info, sizeof(vbe_info), vbe_info_seg * 16L);

    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x4F00;
    regs.W.di = 0;
    regs.W.es = vbe_info_seg;
    PDCINT(0x10, regs);

    /* Check for successful completion of function: is VESA BIOS present? */
    if (regs.W.ax != 0x004F)
        goto error;
    dosmemget(vbe_info_seg * 16L, sizeof(vbe_info), &vbe_info);
    if (memcmp(vbe_info.VbeSignature, "VESA", 4) != 0)
        goto error;

    /* Get the address of the mode list */
    /* The mode list may be within the DOS memory area allocated above.
       That area must remain allocated and must not be rewritten until
       we're done here. */
    mode_addr = (vbe_info.VideoModePtr >> 16) * 16L
              + (vbe_info.VideoModePtr & 0xFFFF);

    /* Look for the best-fitting mode available */
    vesa_mode = _find_mode(&mode_info, mode_addr, rows, cols);
    if (vesa_mode == 0xFFFF)
        vesa_mode = _find_mode(&mode_info, mode_addr, 0, 0);
    if (vesa_mode == 0xFFFF)
        goto error;

    /* Set up frame buffer window */
    if ((mode_info.WinAAttributes & 0x2) != 0)
        PDC_state.read_win = 0; /* Read through Window A */
    else if ((mode_info.WinBAttributes & 0x2) != 0)
        PDC_state.read_win = 1; /* Read through Window B */
    else
        goto error; /* shouldn't happen */
    if ((mode_info.WinAAttributes & 0x1) != 0)
        PDC_state.write_win = 0; /* Write through Window A */
    else if ((mode_info.WinBAttributes & 0x1) != 0)
        PDC_state.write_win = 1; /* Write through Window B */
    else
        goto error; /* shouldn't happen */
    PDC_state.window[0] = mode_info.WinASegment;
    PDC_state.window[1] = mode_info.WinBSegment;
    if (vesa_mode == 0x12)
    {
        PDC_state.offset[0] = 0;
        PDC_state.offset[1] = 0;
    }
    else
    {
        PDC_state.offset[0] = 0xFFFFFFFF; /* offset is unknown */
        PDC_state.offset[1] = 0xFFFFFFFF;
    }
    PDC_state.window_size = mode_info.WinSize * 1024L;
    PDC_state.window_gran = mode_info.WinGranularity;
    PDC_state.bytes_per_line = mode_info.BytesPerScanLine;
    PDC_state.video_width = mode_info.XResolution;
    PDC_state.video_height = mode_info.YResolution;
    PDC_state.linear_buffer = FALSE;
    PDC_state.bits_per_pixel = mode_info.BitsPerPixel;

    if (PDC_state.bits_per_pixel <= 8)
    {
        PDC_state.red_max = 63;
        PDC_state.green_max = 63;
        PDC_state.blue_max = 63;
    }
    else
    {
        if (mode_info.ModeAttributes & 0x2)
        {
            PDC_state.red_max = (1 << mode_info.RedMaskSize) - 1;
            PDC_state.red_pos = mode_info.RedFieldPosition;
            PDC_state.green_max = (1 << mode_info.GreenMaskSize) - 1;
            PDC_state.green_pos = mode_info.GreenFieldPosition;
            PDC_state.blue_max = (1 << mode_info.BlueMaskSize) - 1;
            PDC_state.blue_pos = mode_info.BlueFieldPosition;
        }
        else
        {
            switch (PDC_state.bits_per_pixel)
            {
            case 15:
                PDC_state.red_max = 31;
                PDC_state.red_pos = 10;
                PDC_state.green_max = 31;
                PDC_state.green_pos = 5;
                PDC_state.blue_max = 31;
                PDC_state.blue_pos = 0;
                break;

            case 16:
                PDC_state.red_max = 31;
                PDC_state.red_pos = 11;
                PDC_state.green_max = 63;
                PDC_state.green_pos = 5;
                PDC_state.blue_max = 31;
                PDC_state.blue_pos = 0;
                break;

            case 24:
            case 32:
                PDC_state.red_max = 255;
                PDC_state.red_pos = 16;
                PDC_state.green_max = 255;
                PDC_state.green_pos = 8;
                PDC_state.blue_max = 255;
                PDC_state.blue_pos = 0;
                break;
            }
        }
    }

    __dpmi_free_dos_memory(vbe_info_sel);
    return vesa_mode;

error:
    /* If we can't access the VESA BIOS Extensions for any reason, return the
       640x480 VGA mode */
    if (vbe_info_sel != -1)
        __dpmi_free_dos_memory(vbe_info_sel);

    PDC_state.linear_buffer = FALSE;
    PDC_state.bits_per_pixel = 4;
    PDC_state.video_width = 640;
    PDC_state.video_height = 480;
    PDC_state.bytes_per_line = 80;
    PDC_state.read_win = 0;
    PDC_state.write_win = 0;
    PDC_state.window[0] = 0xA000;
    PDC_state.window[1] = 0xA000;
    PDC_state.offset[0] = 0;
    PDC_state.offset[1] = 0;
    PDC_state.window_size = (640/8) * 480;
    PDC_state.window_gran = 1;

    return 0x12;
}

static unsigned _find_mode(
        struct ModeInfoBlock *mode_info,
        unsigned long mode_addr,
        int rows, int cols)
{
    unsigned selected_mode;
    unsigned long selected_size;
    unsigned selected_bits;

    selected_mode = 0xFFFF;
    selected_size = (rows == 0 && cols == 0) ? 0 : 0xFFFFFFFF;
    selected_bits = 0;

    if ((rows <= 30 && cols <= 80) && !(rows == 0 && cols == 0))
    {
        /* Set up a ModeInfoBlock for mode 0x0012 */
        selected_mode = 0x0012;
        selected_size = 80 * 30;
        selected_bits = 4;
        memset(mode_info, 0, sizeof(*mode_info));
        mode_info->ModeAttributes = 0x1F;
        mode_info->WinAAttributes = 0x07;
        mode_info->WinBAttributes = 0x00;
        mode_info->WinGranularity = 1;
        mode_info->WinSize = 38;
        mode_info->WinASegment = 0xA000;
        mode_info->WinBSegment = 0;
        mode_info->WinFuncPtr = 0;
        mode_info->BytesPerScanLine = 80;
        mode_info->XResolution = 640;
        mode_info->YResolution = 480;
        mode_info->NumberOfPlanes = 4;
        mode_info->BitsPerPixel = 4;
        mode_info->NumberOfBanks = 1;
        mode_info->MemoryModel = 3;
    }

    while (1)
    {
        unsigned mode;
        struct ModeInfoBlock mode_info0;
        unsigned new_rows, new_cols;
        unsigned long new_size;

        mode = getdosmemword(mode_addr);
        if (mode == 0xFFFF)
            break;
        mode_addr += 2;

        /* Query the mode info; skip if not supported */
        if (_get_mode_info(mode, &mode_info0) < 0)
            continue;

        /* Check that the mode is acceptable: */
        /* Supported, graphics mode, color, VGA compatible */
        if ((mode_info0.ModeAttributes & 0x79) != 0x19)
            continue;
        /* Bits per pixel and memory model are acceptable */
        switch (mode_info0.BitsPerPixel)
        {
        case 4:
            if (mode_info0.MemoryModel != 3) /* Planar */
                continue;
            if (mode_info0.NumberOfPlanes != 4)
                continue;
            break;

        case 8:
            if (mode_info0.MemoryModel != 4) /* Packed pixel */
                continue;
            if (mode_info0.NumberOfPlanes != 1)
                continue;
            break;

        case 15:
        case 16:
        case 24:
        case 32:
            if (mode_info0.MemoryModel != 6) /* Direct color */
                continue;
            if (mode_info0.NumberOfPlanes != 1)
                continue;
            break;

        default:
            continue;
        }

        /* At least as many rows and columns as requested */
        new_cols = mode_info0.XResolution / 8;
        new_rows = mode_info0.YResolution / _FONT16;
        if (new_cols < cols || new_rows < rows)
            continue;

        /* Among modes that are large enough for rows and cols, choose one
           with the most bits per pixel. Among modes with the same pixel depth,
           if rows == 0 and cols == 0, select the largest available size;
           otherwise, select the smallest that can hold that many rows and
           columns. */
        if (mode_info0.BitsPerPixel < selected_bits)
            continue;
        new_size = (unsigned long)new_rows * new_cols;
        if (mode_info0.BitsPerPixel == selected_bits)
        {
            if (rows == 0 && cols == 0)
            {
                if (new_size <= selected_size)
                    continue;
            }
            else
            {
                if (new_size >= selected_size)
                    continue;
            }
        }

        /* Select this mode, pending discovery of a better one */
        selected_mode = mode;
        selected_size = new_size;
        selected_bits = mode_info0.BitsPerPixel;
        *mode_info = mode_info0;
    }

    return selected_mode;
}

static int _get_mode_info(unsigned mode, struct ModeInfoBlock *mode_info)
{
    struct OldModeInfo
    {
        unsigned mode;

        unsigned short XResolution;    /* horizontal resolution in pixels or characters */
        unsigned short YResolution;    /* vertical resolution in pixels or characters */
        unsigned char  BitsPerPixel;   /* bits per pixel */
    };
    static const struct OldModeInfo old_mode_table[] =
    {
        { 0x0100,  640,  480,  4 },
        { 0x0101,  640,  480,  8 },
        { 0x0102,  800,  600,  4 },
        { 0x0103,  800,  600,  8 },
        { 0x0104, 1024,  768,  4 },
        { 0x0105, 1024,  768,  8 },
        { 0x0106, 1280, 1024,  4 },
        { 0x0107, 1280, 1024,  8 },
        { 0x010D,  320,  200, 15 },
        { 0x010E,  320,  200, 16 },
        { 0x010F,  320,  200, 24 },
        { 0x0110,  640,  480, 15 },
        { 0x0111,  640,  480, 16 },
        { 0x0112,  640,  480, 24 },
        { 0x0113,  800,  600, 15 },
        { 0x0114,  800,  600, 16 },
        { 0x0115,  800,  600, 24 },
        { 0x0116, 1024,  768, 15 },
        { 0x0117, 1024,  768, 16 },
        { 0x0118, 1024,  768, 24 },
        { 0x0119, 1280, 1024, 15 },
        { 0x011A, 1280, 1024, 16 },
        { 0x011B, 1280, 1024, 24 },
        /* sentinel */
        { 0, 0, 0, 0 }
    };

    int mode_info_sel = -1; /* custodial */
    int mode_info_seg;
    PDCREGS regs;

    mode_info_seg = __dpmi_allocate_dos_memory(
            (sizeof(*mode_info) + 15) / 16,
            &mode_info_sel);
    if (mode_info_seg < 0)
        goto error;

    memset(mode_info, 0, sizeof(*mode_info));
    dosmemput(mode_info, sizeof(*mode_info), mode_info_seg * 16L);

    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x4F01;
    regs.W.cx = mode;
    regs.W.di = 0;
    regs.W.es = mode_info_seg;
    PDCINT(0x10, regs);

    if (regs.W.ax != 0x004F)
        goto error;
    dosmemget(mode_info_seg * 16L, sizeof(*mode_info), mode_info);
    if (!(mode_info->ModeAttributes & 0x0001))
        goto error;

    if (!(mode_info->ModeAttributes & 0x0002))
    {
        /* Older VESA BIOS that did not return certain mode properties, but
           that has fixed mode numbers; search the table to find the right
           mode properties */

        unsigned i;

        for (i = 0; old_mode_table[i].mode != 0; ++i)
        {
            if (mode == old_mode_table[i].mode)
                break;
        }
        if (old_mode_table[i].mode == 0)
            goto error;

        mode_info->XResolution = old_mode_table[i].XResolution;
        mode_info->YResolution = old_mode_table[i].YResolution;
        mode_info->BitsPerPixel = old_mode_table[i].BitsPerPixel;
        mode_info->NumberOfBanks = 1;
        switch (mode_info->BitsPerPixel)
        {
        case 4:
            mode_info->NumberOfPlanes = 4;
            mode_info->MemoryModel = 3;
            break;

        case 8:
            mode_info->NumberOfPlanes = 1;
            mode_info->MemoryModel = 4;
            break;

        default:
            mode_info->NumberOfPlanes = 1;
            mode_info->MemoryModel = 6;
            break;
        }
    }

    __dpmi_free_dos_memory(mode_info_sel);
    return TRUE;

error:
    if (mode_info_sel != -1)
        __dpmi_free_dos_memory(mode_info_sel);
    return FALSE;
}

/* DOS memory allocation routines as defined by DJGPP, for use by Watcom */
#if defined(__WATCOMC__) && defined(__386__)
static int __dpmi_allocate_dos_memory(int paras, int *sel_or_max)
{
    int flags = 0, r_ax = 0, r_bx = 0, r_dx = 0;

    _asm {
        mov ebx, paras;
        mov eax, 0x0100;
        int 0x31;
        pushfd;
        pop flags;
        mov r_ax, eax;
        mov r_bx, ebx;
        mov r_dx, edx;
    }
    if (flags & 0x01)
    {
        /* carry set */
        *sel_or_max = r_bx;
        return -1;
    }
    else
    {
        /* carry clear */
        *sel_or_max = r_dx;
        return r_ax;
    }
}

static int __dpmi_free_dos_memory(int sel)
{
    int flags = 0;

    _asm {
        mov edx, sel;
        mov eax, 0x0101;
        int 0x31;
        pushfd;
        pop flags;
    }
    return (flags & 0x01) ? -1 : 0;
}

static void dosmemget(unsigned long addr, size_t size, void *buf)
{
    memcpy(buf, (const void *)addr, size);
}

static void dosmemput(const void *buf, size_t size, unsigned long addr)
{
    memcpy((void *)addr, buf, size);
}
#endif
