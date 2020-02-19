/* PDCurses */

#include "pdcdos.h"
#include "pdcvesa.h"

#include <stdlib.h>
#include <string.h>

/* TODO: support 8 bit palette registers if available */
/* TODO: support modes with more than 4 bits per pixel */

struct PDC_video_state PDC_state;

static short realtocurs[16] =
{
    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
    COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, COLOR_BLACK + 8,
    COLOR_BLUE + 8, COLOR_GREEN + 8, COLOR_CYAN + 8, COLOR_RED + 8,
    COLOR_MAGENTA + 8, COLOR_YELLOW + 8, COLOR_WHITE + 8
};

static int saved_scrnmode[3];

static int _get_mode_info(unsigned mode, struct ModeInfoBlock *mode_info);
static unsigned _find_video_mode(int rows, int cols);
static unsigned _find_mode(
        struct ModeInfoBlock *mode_info,
        unsigned long mode_addr,
        int rows, int cols);

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
    int i;

    PDC_LOG(("PDC_scr_open() - called\n"));

    SP = calloc(1, sizeof(SCREEN));

    if (!SP)
        return ERR;

    PDC_resize_screen(25, 80);
    for (i = 0; i < 16; i++)
        PDC_state.pdc_curstoreal[realtocurs[i]] = i;

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
    SP->orig_cursor = PDC_get_cursor_mode();
    PDC_curs_set(SP->visibility);

    return OK;
}

void PDC_reset_prog_mode(void)
{
        PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
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

/* _egapal() - Find the EGA palette value (0-63) for the color (0-15).
   On VGA, this is an index into the DAC. */

static short _egapal(short color)
{
    PDCREGS regs;

    regs.W.ax = 0x1007;
    regs.h.bl = PDC_state.pdc_curstoreal[color];

    PDCINT(0x10, regs);

    return regs.h.bh;
}

bool PDC_can_change_color(void)
{
    return TRUE;
}

/* These are only valid when PDC_state.pdc_adapter == _VGACOLOR */

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    PDCREGS regs;

    /* Read single DAC register */
    /* TODO: support VESA modes */

    regs.W.ax = 0x1015;
    regs.h.bl = _egapal(color);

    PDCINT(0x10, regs);

    /* Scale and store */

    *red = DIVROUND((unsigned)(regs.h.dh) * 1000, 63);
    *green = DIVROUND((unsigned)(regs.h.ch) * 1000, 63);
    *blue = DIVROUND((unsigned)(regs.h.cl) * 1000, 63);

    return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    PDCREGS regs;

    /* Scale */

    regs.h.dh = DIVROUND((unsigned)red * 63, 1000);
    regs.h.ch = DIVROUND((unsigned)green * 63, 1000);
    regs.h.cl = DIVROUND((unsigned)blue * 63, 1000);

    /* Set single DAC register */
    /* TODO: support VESA modes */

    regs.W.ax = 0x1010;
    regs.W.bx = _egapal(color);

    PDCINT(0x10, regs);

    return OK;
}

static unsigned _find_video_mode(int rows, int cols)
{
    int vbe_info_sel = -1; /* custodial */
    int vbe_info_seg;
    struct VbeInfoBlock vbe_info;
    __dpmi_regs regs;
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
    regs.x.ax = 0x4F00;
    regs.x.di = 0;
    regs.x.es = vbe_info_seg;
    __dpmi_int(0x10, &regs);

    /* Check for successful completion of function: is VESA BIOS present? */
    if (regs.x.ax != 0x004F)
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

    __dpmi_free_dos_memory(vbe_info_sel);
    return vesa_mode;

error:
    /* If we can't access the VESA BIOS Extensions for any reason, return the
       640x480 VGA mode */
    if (vbe_info_sel != -1)
        __dpmi_free_dos_memory(vbe_info_sel);

    PDC_state.linear_buffer = FALSE;
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

    selected_mode = 0xFFFF;
    selected_size = (rows == 0 && cols == 0) ? 0 : 0xFFFFFFFF;

    if (rows <= 30 && cols <= 80)
    {
        /* Set up a ModeInfoBlock for mode 0x0012 */
        selected_mode = 0x0012;
        selected_size = 80 * 30;
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
        /* TODO: 8, 15, 16, 24 and 32 bits */

        default:
            continue;
        }

        /* At least as many rows and columns as requested */
        new_cols = mode_info0.XResolution / 8;
        new_rows = mode_info0.YResolution / _FONT16;
        if (new_cols < cols || new_rows < rows)
            continue;

        /* If rows == 0 and cols == 0, select the largest available size;
           otherwise, select the smallest that can hold that many rows and
           columns */
        new_size = (unsigned long)new_rows * new_cols;
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

        /* Select this mode, pending discovery of a better one */
        selected_mode = mode;
        selected_size = new_size;
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
    __dpmi_regs regs;

    mode_info_seg = __dpmi_allocate_dos_memory(
            (sizeof(*mode_info) + 15) / 16,
            &mode_info_sel);
    if (mode_info_seg < 0)
        goto error;

    memset(mode_info, 0, sizeof(*mode_info));
    dosmemput(mode_info, sizeof(*mode_info), mode_info_seg * 16L);

    memset(&regs, 0, sizeof(regs));
    regs.x.ax = 0x4F01;
    regs.x.cx = mode;
    regs.x.di = 0;
    regs.x.es = mode_info_seg;
    (void) __dpmi_int(0x10, &regs);

    if (regs.x.ax != 0x004F)
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
    if (mode_info_sel != -1) __dpmi_free_dos_memory(mode_info_sel);
    return FALSE;
}
