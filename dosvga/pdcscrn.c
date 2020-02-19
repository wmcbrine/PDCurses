/* PDCurses */

#include "pdcdos.h"

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

    if (PDC_state.scrn_mode < 0x100)
    {
        PDC_state.linear_buffer = FALSE;
        PDC_state.video_width = 640;
        PDC_state.video_height = 400;
        PDC_state.bytes_per_line = 80;
        switch (PDC_state.scrn_mode)
        {
        case 0x0D:
        case 0x0E:
            PDC_state.video_height = 200;
            break;

        case 0x0F:
        case 0x10:
            PDC_state.video_height = 350;
            break;

        case 0x11:
        case 0x12:
            PDC_state.video_height = 480;
            break;
        }
    }
    else
    {
        /* TODO: support VESA modes */
    }

    PDC_state.font_addr = _get_font_address();
    LINES = PDC_get_rows();
    COLS = PDC_get_columns();
    PDC_curs_set(1);
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

    PDC_resize_screen(80, 25);
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

    /* Trash the stored value of orig_cursor -- it's only good if the
       video mode doesn't change */

    SP->orig_cursor = 0x0607;

    _set_scrn_mode(0x12);

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
