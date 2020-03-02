/* PDCurses */

#include "pdcdos.h"
#include "pdcvesa.h"
#include "font.h"

#include <stdlib.h>
#include <string.h>

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
#ifdef PDC_FLAT
static unsigned long _map_frame_buffer(unsigned long phys_addr,
        unsigned long size);
static void _unmap_frame_buffer(unsigned long phys_addr);
#endif
#if defined(__WATCOMC__) && defined(__386__)
static int __dpmi_allocate_dos_memory(int paras, int *sel_or_max);
static int __dpmi_free_dos_memory(int sel);
static int __dpmi_allocate_ldt_descriptors(int count);
static int __dpmi_free_ldt_descriptor(int sel);
static int __dpmi_set_segment_base_address(int sel, unsigned long addr);
static int __dpmi_set_segment_limit(int sel, unsigned long limit);
static void dosmemget(unsigned long addr, size_t size, void *buf);
static void dosmemput(const void *buf, size_t size, unsigned long addr);
#endif
static int _psf_font_open(const char *name, bool bold);
static int _vgafont_open(void);

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
    PDC_state.scrn_mode = new_mode;

    LINES = PDC_get_rows();
    COLS = PDC_get_columns();
}

/* close the physical screen -- may restore the screen to its state
   before PDC_scr_open(); miscellaneous cleanup */

void PDC_scr_close(void)
{
#ifdef PDC_FLAT
    _unmap_frame_buffer(PDC_state.linear_addr);
    __dpmi_free_ldt_descriptor(PDC_state.linear_sel);
#endif

    _set_scrn_mode(0x03);
}

void PDC_scr_free(void)
{
    if (PDC_state.font_close != NULL)
    {
        PDC_state.font_close(FALSE);
        PDC_state.font_close(TRUE);
    }
}

/* Set the location of the underline */
static void _set_underline(void)
{
    unsigned width = (PDC_state.font_width + 7) / 8;
    unsigned height = PDC_state.font_height;
    unsigned size = width * height;
    unsigned i;
    const unsigned char *underscore = PDC_state.font_glyph_data(FALSE, 0x5F);

    /* Set underscore at first line of U+005F that is not blank */
    for (i = 0; i < size; ++i)
    {
        if (underscore[i] != 0)
        {
            PDC_state.underline = i / width;
            return;
        }
    }

    /* How peculiar; U+005F is blank. Make a guess. */
    PDC_state.underline = PDC_state.font_height * 3 / 4;
}

/* open the physical screen -- miscellaneous initialization, may save
   the existing screen for later restoration */

int PDC_scr_open(void)
{
    const char *normal_font, *bold_font;
    PDCREGS regs;

    PDC_LOG(("PDC_scr_open() - called\n"));

    /* Check for VGA and bail out if we don't find one */
    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x1A00;
    PDCINT(0x10, regs);
    if (regs.h.bl != 8)
        return ERR;

    SP = calloc(1, sizeof(SCREEN));

    if (!SP)
        return ERR;

    _init_palette();

    /* Set up fonts:
     * * first try PDC_FONT and PDC_FONT_BOLD
     * * Fall back to the ROM font if PDC_FONT is not specified
     * * Width must be 8; height must be not greater than 16; the bold font, if
     *   specified, must have the same size as the normal font
     */
    normal_font = getenv("PDC_FONT");
    bold_font = getenv("PDC_FONT_BOLD");
    if (normal_font != NULL && normal_font[0] != '\0')
    {
        if (_psf_font_open(normal_font, FALSE) == 0)
        {
            /* Opened the normal font; try the bold font */
            if (_psf_font_open(bold_font, TRUE) == 0
                && PDC_state.font_char_width(FALSE) == PDC_state.font_char_width(TRUE)
                && PDC_state.font_char_height(FALSE) == PDC_state.font_char_height(TRUE))
                PDC_state.have_bold_font = TRUE;
        }
    }
    if (PDC_state.font_glyph_data == NULL)
    {
        if (_vgafont_open() < 0)
            return ERR;
        PDC_state.have_bold_font = FALSE;
    }
    PDC_state.font_width = PDC_state.font_char_width(FALSE);
    PDC_state.font_height = PDC_state.font_char_height(FALSE);
    _set_underline();

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

        /* Set 8 bit DACs if available */
        /* This doesn't seem to work with 4 bit color */
        if (PDC_state.bits_per_pixel == 8)
        {
            unsigned dacmax;

            memset(&regs, 0, sizeof(regs));
            regs.W.ax = 0x4F08;
            regs.W.bx = 0x0800;
            PDCINT(0x10, regs);
            dacmax = (regs.W.ax == 0x004F) ? (1 << regs.h.bh) - 1 : 63;
            PDC_state.red_max = dacmax;
            PDC_state.green_max = dacmax;
            PDC_state.blue_max = dacmax;
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
    unsigned r = DIVROUND((unsigned long)red * PDC_state.red_max, 1000);
    unsigned g = DIVROUND((unsigned long)green * PDC_state.green_max, 1000);
    unsigned b = DIVROUND((unsigned long)blue * PDC_state.blue_max, 1000);

    PDC_state.colors[color].r = red;
    PDC_state.colors[color].g = green;
    PDC_state.colors[color].b = blue;

    if (PDC_state.bits_per_pixel <= 8)
    {
        PDCREGS regs;
#if !defined(PDC_FLAT) && !defined(__WATCOMC__)
        struct SREGS sregs;
#endif

        if (PDC_state.scrn_mode >= 0x100)
        {
            /* VESA mode in use; first try the VESA function */
#ifdef PDC_FLAT
            int seg, sel;
#endif
            unsigned long table;

            table = ((unsigned long)b <<  0)
                  | ((unsigned long)g <<  8)
                  | ((unsigned long)r << 16);
#ifdef PDC_FLAT
            seg = __dpmi_allocate_dos_memory(1, &sel);
            if (seg < 0)
                return ERR;
            setdosmemdword(seg * 16L, table);
#endif
            memset(&regs, 0, sizeof(regs));
            regs.W.ax = 0x4F09;
            regs.h.bl = 0;
            regs.W.cx = 1;
            regs.W.dx = color;
#ifdef PDC_FLAT
            regs.W.di = 0;
            regs.W.es = seg;
            PDCINT(0x10, regs);
#elif defined(__WATCOMC__)
            regs.W.di = FP_OFF(&table);
            regs.W.es = FP_SEG(&table);
            PDCINT(0x10, regs);
#else
            regs.W.di = FP_OFF(&table);
            segread(&sregs);
            sregs.es = FP_SEG(&table);
            int86x(0x10, &regs, &regs, &sregs);
#endif
#ifdef PDC_FLAT
            __dpmi_free_dos_memory(sel);
#endif
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
#ifdef PDC_FLAT
    int vbe_info_sel = -1; /* custodial */
    int vbe_info_seg;
#endif
    struct VbeInfoBlock vbe_info;
    PDCREGS regs;
#if !defined(PDC_FLAT) && !defined(__WATCOMC__)
    struct SREGS sregs;
#endif
    unsigned long mode_addr;
    struct ModeInfoBlock mode_info;
    unsigned vesa_mode;

    /* Request VESA BIOS information */
    memset(&vbe_info, 0, sizeof(vbe_info));
    memcpy(vbe_info.VbeSignature, "VBE2", 4);

#ifdef PDC_FLAT
    vbe_info_seg = __dpmi_allocate_dos_memory(
            (sizeof(vbe_info) + 15) / 16,
            &vbe_info_sel);
    if (vbe_info_seg < 0)
        goto error;
    dosmemput(&vbe_info, sizeof(vbe_info), vbe_info_seg * 16L);
#endif

    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x4F00;
#ifdef PDC_FLAT
    regs.W.di = 0;
    regs.W.es = vbe_info_seg;
    PDCINT(0x10, regs);
#elif defined(__WATCOMC__)
    regs.W.di = FP_OFF(&vbe_info);
    regs.W.es = FP_SEG(&vbe_info);
    PDCINT(0x10, regs);
#else
    regs.W.di = FP_OFF(&vbe_info);
    segread(&sregs);
    sregs.es = FP_SEG(&vbe_info);
    int86x(0x10, &regs, &regs, &sregs);
#endif

    /* Check for successful completion of function: is VESA BIOS present? */
    if (regs.W.ax != 0x004F)
        goto error;
#ifdef PDC_FLAT
    dosmemget(vbe_info_seg * 16L, sizeof(vbe_info), &vbe_info);
#endif
    if (memcmp(vbe_info.VbeSignature, "VESA", 4) != 0)
        goto error;

    /* Get the address of the mode list */
    /* The mode list may be within the DOS memory area allocated above.
       That area must remain allocated and must not be rewritten until
       we're done here. */
#ifdef PDC_FLAT
    mode_addr = _FAR_POINTER(vbe_info.VideoModePtr >> 16,
                             vbe_info.VideoModePtr & 0xFFFF);
#else
    mode_addr = vbe_info.VideoModePtr;
#endif

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
    PDC_state.bits_per_pixel = mode_info.BitsPerPixel;

#ifdef PDC_FLAT
    /* Enable the linear frame buffer if available */
    if ((mode_info.ModeAttributes & 0x80) != 0)
    {
        do { /* while (FALSE) */
            unsigned win_size;
            unsigned width;

            width = (vbe_info.VbeVersion >= 0x300)
                  ? mode_info.LinBytesPerScanLine
                  : mode_info.BytesPerScanLine;

            PDC_state.linear_sel = __dpmi_allocate_ldt_descriptors(1);
            if (PDC_state.linear_sel < 0)
                break;
            win_size = width * mode_info.YResolution;
            PDC_state.linear_addr = _map_frame_buffer(mode_info.PhysBasePtr, win_size);
            if (PDC_state.linear_addr == 0)
            {
                __dpmi_free_ldt_descriptor(PDC_state.linear_sel);
                PDC_state.linear_sel = 0;
                break;
            }
            __dpmi_set_segment_base_address(PDC_state.linear_sel, PDC_state.linear_addr);
            __dpmi_set_segment_limit(PDC_state.linear_sel, (win_size - 1) | 0xFFF);
            vesa_mode |= 0x4000;
            PDC_state.bytes_per_line = width;
        } while (FALSE);
    }
#else
    PDC_state.linear_sel = 0;
#endif

    if (PDC_state.bits_per_pixel <= 8)
    {
        PDC_state.red_max = 63;
        PDC_state.green_max = 63;
        PDC_state.blue_max = 63;
    }
    else
    {
#ifdef PDC_FLAT
        if (PDC_state.linear_sel && vbe_info.VbeVersion >= 0x300)
        {
            PDC_state.red_max = (1 << mode_info.LinRedMaskSize) - 1;
            PDC_state.red_pos = mode_info.LinRedFieldPosition;
            PDC_state.green_max = (1 << mode_info.LinGreenMaskSize) - 1;
            PDC_state.green_pos = mode_info.LinGreenFieldPosition;
            PDC_state.blue_max = (1 << mode_info.LinBlueMaskSize) - 1;
            PDC_state.blue_pos = mode_info.LinBlueFieldPosition;
        }
        else
#endif
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

#ifdef PDC_FLAT
    __dpmi_free_dos_memory(vbe_info_sel);
#endif
    return vesa_mode;

error:
    /* If we can't access the VESA BIOS Extensions for any reason, return the
       640x480 VGA mode */
#ifdef PDC_FLAT
    if (vbe_info_sel != -1)
        __dpmi_free_dos_memory(vbe_info_sel);
#endif

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
    PDC_state.red_max = 63;
    PDC_state.green_max = 63;
    PDC_state.blue_max = 63;

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

    if (rows * PDC_state.font_height <= 480 && cols * PDC_state.font_width <= 640)
    {
        /* Set up a ModeInfoBlock for mode 0x0012 */
        unsigned new_rows = 480 / PDC_state.font_height;
        unsigned new_cols = 640 / PDC_state.font_width;
        selected_mode = 0x0012;
        selected_size = new_rows * new_cols;
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
        new_cols = mode_info0.XResolution / PDC_state.font_width;
        new_rows = mode_info0.YResolution / PDC_state.font_height;
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

#ifdef PDC_FLAT
    int mode_info_sel = -1; /* custodial */
    int mode_info_seg;
#endif
    PDCREGS regs;
#if !defined(PDC_FLAT) && !defined(__WATCOMC__)
    struct SREGS sregs;
#endif

    memset(mode_info, 0, sizeof(*mode_info));
#ifdef PDC_FLAT
    mode_info_seg = __dpmi_allocate_dos_memory(
            (sizeof(*mode_info) + 15) / 16,
            &mode_info_sel);
    if (mode_info_seg < 0)
        goto error;
    dosmemput(mode_info, sizeof(*mode_info), mode_info_seg * 16L);
#endif

    memset(&regs, 0, sizeof(regs));
    regs.W.ax = 0x4F01;
    regs.W.cx = mode;
#ifdef PDC_FLAT
    regs.W.di = 0;
    regs.W.es = mode_info_seg;
    PDCINT(0x10, regs);
#elif defined(__WATCOMC__)
    regs.W.di = FP_OFF(mode_info);
    regs.W.es = FP_SEG(mode_info);
    PDCINT(0x10, regs);
#else
    regs.W.di = FP_OFF(mode_info);
    segread(&sregs);
    sregs.es = FP_SEG(mode_info);
    int86x(0x10, &regs, &regs, &sregs);
#endif

    if (regs.W.ax != 0x004F)
        goto error;
#ifdef PDC_FLAT
    dosmemget(mode_info_seg * 16L, sizeof(*mode_info), mode_info);
#endif
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

#ifdef PDC_FLAT
    __dpmi_free_dos_memory(mode_info_sel);
#endif
    return TRUE;

error:
#ifdef PDC_FLAT
    if (mode_info_sel != -1)
        __dpmi_free_dos_memory(mode_info_sel);
#endif
    return FALSE;
}

/* Make a physical address mapping, to support the linear frame buffer */
#ifdef __DJGPP__
static unsigned long
_map_frame_buffer(unsigned long phys_addr, unsigned long size)
{
    __dpmi_meminfo info;
    int rc;

    info.handle = 0;
    info.address = phys_addr;
    info.size = size;
    rc = __dpmi_physical_address_mapping(&info);

    return rc == 0 ? info.address : 0;
}

static void
_unmap_frame_buffer(unsigned long phys_addr)
{
    __dpmi_meminfo info;

    info.handle = 0;
    info.address = phys_addr;
    info.size = 0;
    __dpmi_free_physical_address_mapping(&info);
}
#elif defined(__WATCOMC__) && defined(__386__)
static unsigned long
_map_frame_buffer(unsigned long phys_addr, unsigned long size)
{
    unsigned flags = 0;
    unsigned long r_bxcx = 0;

    _asm {
        mov ecx, phys_addr;
        mov ebx, ecx;
        shr ebx, 16;
        mov edi, size;
        mov esi, edi;
        shr esi, 16;
        mov eax, 0x0800;
        int 0x31;
        pushfd;
        pop flags;
        shl ebx, 16;
        mov bx, cx;
        mov r_bxcx, ebx;
    }

    return (flags & 1) ? 0 : r_bxcx;
}

static void
_unmap_frame_buffer(unsigned long phys_addr)
{
    _asm {
        mov ecx, phys_addr;
        mov ebx, ecx;
        shr ebx, 16;
        mov eax, 0x0801;
        int 0x31;
    }
}
#endif

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

static int __dpmi_allocate_ldt_descriptors(int count)
{
    int flags = 0;
    int r_eax = 0;

    _asm {
        mov ecx, count;
        mov eax, 0x0000;
        int 0x31;
        pushfd;
        pop flags;
        mov r_eax, eax;
    }

    return (flags & 0x01) ? -1 : r_eax;
}

static int __dpmi_free_ldt_descriptor(int sel)
{
    int flags = 0;

    _asm {
        mov ebx, sel;
        mov eax, 0x0001;
        int 0x31;
        pushfd;
        pop flags;
    }
    return (flags & 0x01) ? -1 : 0;
}

static int __dpmi_set_segment_base_address(int sel, unsigned long addr)
{
    int flags = 0;

    _asm {
        mov ebx, sel;
        mov edx, addr;
        mov ecx, edx;
        shr ecx, 16;
        mov eax, 0x0007;
        int 0x31;
        pushfd;
        pop flags;
    }

    return (flags & 0x01) ? -1 : 0;
}

static int __dpmi_set_segment_limit(int sel, unsigned long limit)
{
    int flags = 0;

    _asm {
        mov ebx, sel;
        mov edx, limit;
        mov ecx, edx;
        shr ecx, 16;
        mov eax, 0x0008;
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

/*****************************************************************************
*                    Support for a built-in 16 line font                     *
*****************************************************************************/

static void _vgafont_close(bool bold);
static unsigned _vgafont_char_width(bool bold);
static unsigned _vgafont_char_height(bool bold);
static const unsigned char *_vgafont_glyph_data(bool bold, unsigned long pos);

#ifdef PDC_WIDE
/* This maps Unicode to CP437 */

static const struct
{
    unsigned short uni;
    unsigned char pos;
} cp437_map[] =
{
    /* Regular mappings */
    { 0x0000, 0x00 },
    { 0x263A, 0x01 },
    { 0x263B, 0x02 },
    { 0x2665, 0x03 },
    { 0x2666, 0x04 },
    { 0x2663, 0x05 },
    { 0x2660, 0x06 },
    { 0x2022, 0x07 },
    { 0x25D8, 0x08 },
    { 0x25CB, 0x09 },
    { 0x25D9, 0x0A },
    { 0x2642, 0x0B },
    { 0x2640, 0x0C },
    { 0x266A, 0x0D },
    { 0x266B, 0x0E },
    { 0x263C, 0x0F },
    { 0x25BA, 0x10 },
    { 0x25C4, 0x11 },
    { 0x2195, 0x12 },
    { 0x203C, 0x13 },
    { 0x00B6, 0x14 },
    { 0x00A7, 0x15 },
    { 0x25AC, 0x16 },
    { 0x21A8, 0x17 },
    { 0x2191, 0x18 },
    { 0x2193, 0x19 },
    { 0x2192, 0x1A },
    { 0x2190, 0x1B },
    { 0x221F, 0x1C },
    { 0x2194, 0x1D },
    { 0x25B2, 0x1E },
    { 0x25BC, 0x1F },
    { 0x2302, 0x7F },
    { 0x00C7, 0x80 },
    { 0x00FC, 0x81 },
    { 0x00E9, 0x82 },
    { 0x00E2, 0x83 },
    { 0x00E4, 0x84 },
    { 0x00E0, 0x85 },
    { 0x00E5, 0x86 },
    { 0x00E7, 0x87 },
    { 0x00EA, 0x88 },
    { 0x00EB, 0x89 },
    { 0x00E8, 0x8A },
    { 0x00EF, 0x8B },
    { 0x00EE, 0x8C },
    { 0x00EC, 0x8D },
    { 0x00C4, 0x8E },
    { 0x00C5, 0x8F },
    { 0x00C9, 0x90 },
    { 0x00E6, 0x91 },
    { 0x00C6, 0x92 },
    { 0x00F4, 0x93 },
    { 0x00F6, 0x94 },
    { 0x00F2, 0x95 },
    { 0x00FB, 0x96 },
    { 0x00F9, 0x97 },
    { 0x00FF, 0x98 },
    { 0x00D6, 0x99 },
    { 0x00DC, 0x9A },
    { 0x00A2, 0x9B },
    { 0x00A3, 0x9C },
    { 0x00A5, 0x9D },
    { 0x20A7, 0x9E },
    { 0x0192, 0x9F },
    { 0x00E1, 0xA0 },
    { 0x00ED, 0xA1 },
    { 0x00F3, 0xA2 },
    { 0x00FA, 0xA3 },
    { 0x00F1, 0xA4 },
    { 0x00D1, 0xA5 },
    { 0x00AA, 0xA6 },
    { 0x00BA, 0xA7 },
    { 0x00BF, 0xA8 },
    { 0x2310, 0xA9 },
    { 0x00AC, 0xAA },
    { 0x00BD, 0xAB },
    { 0x00BC, 0xAC },
    { 0x00A1, 0xAD },
    { 0x00AB, 0xAE },
    { 0x00BB, 0xAF },
    { 0x2591, 0xB0 },
    { 0x2592, 0xB1 },
    { 0x2593, 0xB2 },
    { 0x2502, 0xB3 },
    { 0x2524, 0xB4 },
    { 0x2561, 0xB5 },
    { 0x2562, 0xB6 },
    { 0x2556, 0xB7 },
    { 0x2555, 0xB8 },
    { 0x2563, 0xB9 },
    { 0x2551, 0xBA },
    { 0x2557, 0xBB },
    { 0x255D, 0xBC },
    { 0x255C, 0xBD },
    { 0x255B, 0xBE },
    { 0x2510, 0xBF },
    { 0x2514, 0xC0 },
    { 0x2534, 0xC1 },
    { 0x252C, 0xC2 },
    { 0x251C, 0xC3 },
    { 0x2500, 0xC4 },
    { 0x253C, 0xC5 },
    { 0x255E, 0xC6 },
    { 0x255F, 0xC7 },
    { 0x255A, 0xC8 },
    { 0x2554, 0xC9 },
    { 0x2569, 0xCA },
    { 0x2566, 0xCB },
    { 0x2560, 0xCC },
    { 0x2550, 0xCD },
    { 0x256C, 0xCE },
    { 0x2567, 0xCF },
    { 0x2568, 0xD0 },
    { 0x2564, 0xD1 },
    { 0x2565, 0xD2 },
    { 0x2559, 0xD3 },
    { 0x2558, 0xD4 },
    { 0x2552, 0xD5 },
    { 0x2553, 0xD6 },
    { 0x256B, 0xD7 },
    { 0x256A, 0xD8 },
    { 0x2518, 0xD9 },
    { 0x250C, 0xDA },
    { 0x2588, 0xDB },
    { 0x2584, 0xDC },
    { 0x258C, 0xDD },
    { 0x2590, 0xDE },
    { 0x2580, 0xDF },
    { 0x03B1, 0xE0 },
    { 0x00DF, 0xE1 },
    { 0x0393, 0xE2 },
    { 0x03C0, 0xE3 },
    { 0x03A3, 0xE4 },
    { 0x03C3, 0xE5 },
    { 0x00B5, 0xE6 },
    { 0x03C4, 0xE7 },
    { 0x03A6, 0xE8 },
    { 0x0398, 0xE9 },
    { 0x03A9, 0xEA },
    { 0x03B4, 0xEB },
    { 0x221E, 0xEC },
    { 0x03C6, 0xED },
    { 0x03B5, 0xEE },
    { 0x2229, 0xEF },
    { 0x2261, 0xF0 },
    { 0x00B1, 0xF1 },
    { 0x2265, 0xF2 },
    { 0x2264, 0xF3 },
    { 0x2320, 0xF4 },
    { 0x2321, 0xF5 },
    { 0x00F7, 0xF6 },
    { 0x2248, 0xF7 },
    { 0x00B0, 0xF8 },
    { 0x2219, 0xF9 },
    { 0x00B7, 0xFA },
    { 0x221A, 0xFB },
    { 0x207F, 0xFC },
    { 0x00B2, 0xFD },
    { 0x25A0, 0xFE },
    { 0x00A0, 0xFF },
    /* Extra mappings so ACS_* will work */
    { 0x00A4, 0x0F },
    { 0x2260, 0xD8 },
    { 0x23BA, 0x2D },
    { 0x23BB, 0x2D },
    { 0x23BC, 0x2D },
    { 0x23BD, 0x5F }
};

#define NO_CHAR 0xFE
static unsigned char pages[10][256];
static unsigned char *map[256];

static void _build_rom_map(void)
{
    unsigned page = 0;
    unsigned i;

    memset(pages, NO_CHAR, sizeof(pages));
    for (i = 0; i < 256; ++i)
        map[i] = NULL;
    /* Map ASCII as identity */
    map[0] = pages[page++];
    for (i = 0x20; i <= 0x7E; ++i)
        map[0][i] = i;
    /* Map according to the list */
    for (i = 0; i < sizeof(cp437_map)/sizeof(cp437_map[0]); ++i)
    {
        unsigned cp = cp437_map[i].uni;
        unsigned t1 = cp >> 8;
        unsigned t2 = cp & 0xFF;
        if (map[t1] == NULL)
        {
            if (page >= sizeof(pages)/sizeof(pages[0]))
            {
                fprintf(stderr, "Need to adjust size of pages array\n)");
                abort();
            }
            map[t1] = pages[page++];
        }
        map[t1][t2] = cp437_map[i].pos;
    }
}

static unsigned char _unicode_to_cp437(unsigned long cp)
{
    unsigned t1, t2;

    if (cp > 0xFFFF)
        return NO_CHAR;
    t1 = cp >> 8;
    t2 = cp & 0xFF;
    if (map[t1] == NULL)
        return NO_CHAR;
    return map[t1][t2];
}
#endif

static int _vgafont_open(void)
{
#ifdef PDC_WIDE
    _build_rom_map();
#endif
    PDC_state.font_close = _vgafont_close;
    PDC_state.font_char_width = _vgafont_char_width;
    PDC_state.font_char_height = _vgafont_char_height;
    PDC_state.font_glyph_data = _vgafont_glyph_data;
    return 0;
}

static void _vgafont_close(bool bold)
{
    /* no operation */
}

static unsigned _vgafont_char_width(bool bold)
{
    return 8;
}

static unsigned _vgafont_char_height(bool bold)
{
    return 14;
}

static const unsigned char *_vgafont_glyph_data(bool bold, unsigned long pos)
{
    unsigned pos437;
#ifdef PDC_WIDE
    pos437 = _unicode_to_cp437(pos);
#else
    pos437 = pos & 0xFF;
#endif
    return font_bytes + pos437*14;
}

/*****************************************************************************
*                           Support for PSF fonts                            *
*****************************************************************************/

/* PSF specification from:
   https://www.win.tue.nl/~aeb/linux/kbd/font-formats-1.html
   Only PSF version 2 is supported. */

#define PSF2_MAGIC "\x72\xb5\x4A\x86"

/* bits used in flags */
#define PSF2_HAS_UNICODE_TABLE 0x01

/* max version recognized so far */
#define PSF2_MAXVERSION 0

/* UTF8 separators */
#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE

struct psf2_header
{
    unsigned char magic[4];
    unsigned long version;
    unsigned long headersize;    /* offset of bitmaps in file */
    unsigned long flags;
    unsigned long length;        /* number of glyphs */
    unsigned long charsize;      /* number of bytes for each character */
    unsigned long height, width; /* max dimensions of glyphs */
    /* charsize = height * ((width + 7) / 8) */
};

/* Information about a single font */
struct font_data
{
    struct psf2_header header;
    unsigned char **glyphs;     /* Bits from the font */
    size_t **map[17];           /* Map code points to font positions */
    unsigned char *blank;       /* Default glyph */
};

static struct font_data psf_fonts[2]; /* normal and bold */

static void _psf_font_close(bool bold);
static unsigned _psf_font_char_width(bool bold);
static unsigned _psf_font_char_height(bool bold);
static const unsigned char *_psf_font_glyph_data(
        bool bold, unsigned long codepoint);
static void _psf_font_do_close(struct font_data *fdata);
static long _psf_font_read_utf8(FILE *fp);
static int _psf_font_map_char(struct font_data *fdata, long codepoint,
        size_t pos);
static const unsigned char *_psf_font_do_glyph_data(
        struct font_data *fdata, long codepoint);

/* Open the font with the given file name, as the normal or the bold font.
 * Return 0 if OK, -1 on any error. */
static int _psf_font_open(const char *name, bool bold)
{
    struct font_data *fdata = &psf_fonts[bold != 0];
    FILE *fp;
    size_t size;
    size_t pos;

    memset(fdata, 0, sizeof(*fdata));

    fp = fopen(name, "rb");
    if (fp == NULL)
        goto error;

    /* Read and check the header */
    size = fread(&fdata->header, sizeof(fdata->header), 1, fp);
    if (size != 1)
        goto error;
    if (memcmp(fdata->header.magic, PSF2_MAGIC, 4) != 0)
        goto error;
    if (fdata->header.charsize < fdata->header.height * ((fdata->header.width + 7) / 8))
        goto error;

    /* Read the bitmap data */
    if (fdata->header.length != (size_t)fdata->header.length)
        goto error; /* only possible if size_t has less than four bytes */
    fdata->glyphs = calloc(fdata->header.length, sizeof(fdata->glyphs[0]));
    if (fdata->glyphs == NULL)
        goto error;
    for (pos = 0; pos < fdata->header.length; ++pos)
    {
        fdata->glyphs[pos] = malloc(fdata->header.charsize);
        if (fdata->glyphs[pos] == NULL)
            goto error;
        size = fread(fdata->glyphs[pos], 1, fdata->header.charsize, fp);
        if (size != fdata->header.charsize)
            goto error;
    }

    /* Read the Unicode mapping if one is indicated */
    if (fdata->header.flags & PSF2_HAS_UNICODE_TABLE)
    {
        pos = 0;
        while (1)
        {
            long codepoint = _psf_font_read_utf8(fp);
            if (codepoint == -PSF2_STARTSEQ)
            {
                /* Introduces a sequence of combining characters; these are
                 * not supported */
                do
                {
                    codepoint = _psf_font_read_utf8(fp);
                } while (codepoint != -1 && codepoint != -PSF2_SEPARATOR);
            }
            if (codepoint == -PSF2_SEPARATOR)
            {
                ++pos; /* next glyph position */
            }
            else if (codepoint < 0)
            {
                break; /* end of file or error */
            }
            else
            {
                /* single code point mapped to the current position */
                if (pos >= fdata->header.length)
                    goto error;
                if (_psf_font_map_char(fdata, codepoint, pos) < 0)
                    goto error;
            }
        }
    }

    /* Provide a blank glyph for nonexistent mappings */
    if (_psf_font_do_glyph_data(fdata, 0xFFFD) == NULL)
    {
        fdata->blank = calloc(1, fdata->header.charsize);
        if (fdata->blank == NULL)
            goto error;
    }
    else
    {
        fdata->blank = NULL;
    }

    fclose(fp);
    PDC_state.font_close = _psf_font_close;
    PDC_state.font_char_width = _psf_font_char_width;
    PDC_state.font_char_height = _psf_font_char_height;
    PDC_state.font_glyph_data = _psf_font_glyph_data;
    return 0;

error:
    if (fp != NULL)
        fclose(fp);
    _psf_font_do_close(fdata);
    return -1;
}

/* Close the normal or the bold font */
static void _psf_font_close(bool bold)
{
    _psf_font_do_close(&psf_fonts[bold != 0]);
}

/* Free memory associated with a font */
static void _psf_font_do_close(struct font_data *fdata)
{
    size_t pos;
    unsigned i, j;

    /* Free the glyph data */
    if (fdata->glyphs != NULL)
    {
        for (pos = 0; pos < fdata->header.length; ++pos)
            free(fdata->glyphs[pos]);
        free(fdata->glyphs);
        fdata->glyphs = NULL;
    }
    free(fdata->blank);
    fdata->blank = NULL;

    /* Free the Unicode mapping */
    for (i = 0; i < 17; ++i)
    {
        if (fdata->map[i] != NULL)
        {
            for (j = 0; j < 256; ++j)
                free(fdata->map[i][j]);
            free(fdata->map[i]);
            fdata->map[i] = NULL;
        }
    }
}

/* Local function to read a UTF-8 sequence or a separator from the font file */
static long _psf_font_read_utf8(FILE *fp)
{
    int byte;
    long codepoint;
    long min;
    unsigned count;

    /* Read first byte */
    byte = fgetc(fp);
    if (byte == EOF)
        return -1;
    if (byte == PSF2_STARTSEQ || byte == PSF2_SEPARATOR)
        return -byte; /* markers meaningful to the font format */
    if (byte < 0x80)
    {
        return byte;  /* ASCII */
    }
    else if (byte < 0xC2)
    {
        return -1;    /* Not valid as first byte */
    }
    else if (byte < 0xE0)
    {
        /* Two byte character */
        codepoint = byte & 0x1F;
        min = 0x80;
        count = 1;
    }
    else if (byte < 0xF0)
    {
        /* Three byte character */
        codepoint = byte & 0x0F;
        min = 0x800;
        count = 2;
    }
    else if (byte < 0xF5)
    {
        /* Four byte character */
        codepoint = byte & 0x07;
        min = 0x10000;
        count = 3;
    }
    else
    {
        return -1; /* Not valid as first byte */
    }

    /* Read continuation bytes */
    while (count != 0)
    {
        byte = fgetc(fp);
        if (byte < 0x80 || 0xBF < byte)
            return -1;
        codepoint = (codepoint << 6) | (byte & 0x3F);
        --count;
    }

    /* Check for overlong, surrogate, out of range */
    if (codepoint < min || codepoint > 0x10FFFF
    ||  (0xD800 <= codepoint && codepoint <= 0xDFFF))
        return -1;

    return codepoint;
}

/* Local function to establish a mapping from a codepoint to a font position */
static int _psf_font_map_char(struct font_data *fdata, long codepoint, size_t pos)
{
    unsigned t1 = codepoint >> 16;
    unsigned t2 = (codepoint >> 8) & 0xFF;
    unsigned t3 = codepoint & 0xFF;
    unsigned i;

    if (fdata->map[t1] == NULL)
    {
        fdata->map[t1] = calloc(256, sizeof(fdata->map[0][0]));
        if (fdata->map[t1] == NULL)
            return -1;
    }
    if (fdata->map[t1][t2] == NULL)
    {
        fdata->map[t1][t2] = malloc(256 * sizeof(fdata->map[0][0][0]));
        if (fdata->map[t1][t2] == NULL)
            return -1;
        for (i = 0; i < 256; ++i)
            fdata->map[t1][t2][i] = (size_t)-1;
    }
    fdata->map[t1][t2][t3] = pos;
    return 0;
}

/* Return the character width in pixels */
static unsigned _psf_font_char_width(bool bold)
{
    return psf_fonts[bold != 0].header.width;
}

/* Return the character height in pixels */
static unsigned _psf_font_char_height(bool bold)
{
    return psf_fonts[bold != 0].header.height;
}

/* Return the glyph data as read from the file */
/* Never returns NULL. If the codepoint is not mapped, returns the mapping
 * for U+FFFD if that exists, or the blank glyph otherwise */
static const unsigned char *_psf_font_glyph_data(
        bool bold, unsigned long codepoint)
{
    struct font_data *fdata = &psf_fonts[bold != 0];
    const unsigned char *bitmap;

    bitmap = _psf_font_do_glyph_data(fdata, codepoint);
    if (bitmap == NULL)
        bitmap = _psf_font_do_glyph_data(fdata, 0xFFFD);
    if (bitmap == NULL)
        bitmap = fdata->blank;

    return bitmap;
}

/* Return the glyph data as read from the file */
/* Local function used by PDC_psf_glyph_data; can return NULL */
static const unsigned char *_psf_font_do_glyph_data(
        struct font_data *fdata, long codepoint)
{
    unsigned t1 = codepoint >> 16;
    unsigned t2 = (codepoint >> 8) & 0xFF;
    unsigned t3 = codepoint & 0xFF;
    const unsigned char *bitmap = NULL;
    size_t pos;

    if (t1 <= 17 && fdata->map[t1] != NULL && fdata->map[t1][t2] != NULL)
    {
        pos = fdata->map[t1][t2][t3];
        if (pos != (size_t)-1)
            bitmap = fdata->glyphs[pos];
    }
    return bitmap;
}
