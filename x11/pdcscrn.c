/* PDCurses */

#include "pdcx11.h"

Pixel colors[PDC_MAXCOL];

/* COLOR_PAIR to attribute encoding table. */

static struct {short f, b;} atrtab[PDC_COLOR_PAIRS];

/* close the physical screen */

void PDC_scr_close(void)
{
    PDC_LOG(("PDC_scr_close() - called\n"));
}

void PDC_scr_free(void)
{
    XCursesExit();
}

static void _initialize_colors(void)
{
    int i, r, g, b;

    colors[COLOR_BLACK]   = xc_app_data.colorBlack;
    colors[COLOR_RED]     = xc_app_data.colorRed;
    colors[COLOR_GREEN]   = xc_app_data.colorGreen;
    colors[COLOR_YELLOW]  = xc_app_data.colorYellow;
    colors[COLOR_BLUE]    = xc_app_data.colorBlue;
    colors[COLOR_MAGENTA] = xc_app_data.colorMagenta;
    colors[COLOR_CYAN]    = xc_app_data.colorCyan;
    colors[COLOR_WHITE]   = xc_app_data.colorWhite;

    colors[COLOR_BLACK + 8]   = xc_app_data.colorBoldBlack;
    colors[COLOR_RED + 8]     = xc_app_data.colorBoldRed;
    colors[COLOR_GREEN + 8]   = xc_app_data.colorBoldGreen;
    colors[COLOR_YELLOW + 8]  = xc_app_data.colorBoldYellow;
    colors[COLOR_BLUE + 8]    = xc_app_data.colorBoldBlue;
    colors[COLOR_MAGENTA + 8] = xc_app_data.colorBoldMagenta;
    colors[COLOR_CYAN + 8]    = xc_app_data.colorBoldCyan;
    colors[COLOR_WHITE + 8]   = xc_app_data.colorBoldWhite;

#define RGB(R, G, B) ( ((unsigned long)(R) << 16) | \
                       ((unsigned long)(G) << 8) | \
                       ((unsigned long)(B)) )

    /* 256-color xterm extended palette: 216 colors in a 6x6x6 color
       cube, plus 24 shades of gray */

    for (i = 16, r = 0; r < 6; r++)
        for (g = 0; g < 6; g++)
            for (b = 0; b < 6; b++)
                colors[i++] = RGB(r ? r * 40 + 55 : 0,
                                  g ? g * 40 + 55 : 0,
                                  b ? b * 40 + 55 : 0);
    for (i = 0; i < 24; i++)
        colors[i + 232] = RGB(i * 10 + 8, i * 10 + 8, i * 10 + 8);

#undef RGB
}

/* open the physical screen -- allocate SP, miscellaneous intialization */

int PDC_scr_open(int argc, char **argv)
{
    extern bool sb_started;

    PDC_LOG(("PDC_scr_open() - called\n"));

    if ((XCursesInitscr(argc, argv) == ERR) || !SP)
        return ERR;

    _initialize_colors();

    SP->cursrow = SP->curscol = 0;
    SP->orig_attr = FALSE;
    SP->sb_on = sb_started;
    SP->sb_total_y = 0;
    SP->sb_viewport_y = 0;
    SP->sb_cur_y = 0;
    SP->sb_total_x = 0;
    SP->sb_viewport_x = 0;
    SP->sb_cur_x = 0;

    return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
             nlines, ncols));

    if (nlines || ncols || !SP->resized)
        return ERR;

    SP->lines = resize_window_height / font_height;

    LINES = SP->lines - SP->linesrippedoff - SP->slklines;

    SP->cols = COLS = resize_window_width / font_width;

    window_width = resize_window_width;
    window_height = resize_window_height;
    visible_cursor = TRUE;

    SP->resized = FALSE;

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
}

void PDC_save_screen_mode(int i)
{
}

void PDC_init_pair(short pair, short fg, short bg)
{
    atrtab[pair].f = fg;
    atrtab[pair].b = bg;
}

int PDC_pair_content(short pair, short *fg, short *bg)
{
    *fg = atrtab[pair].f;
    *bg = atrtab[pair].b;

    return OK;
}

bool PDC_can_change_color(void)
{
    return TRUE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    XColor tmp;
    Colormap cmap = DefaultColormap(XCURSESDISPLAY,
                                    DefaultScreen(XCURSESDISPLAY));

    tmp.pixel = colors[color];
    XQueryColor(XCURSESDISPLAY, cmap, &tmp);

    *red = ((double)(tmp.red) * 1000 / 65535) + 0.5;
    *green = ((double)(tmp.green) * 1000 / 65535) + 0.5;
    *blue = ((double)(tmp.blue) * 1000 / 65535) + 0.5;

    return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    XColor tmp;

    tmp.red = ((double)red * 65535 / 1000) + 0.5;
    tmp.green = ((double)green * 65535 / 1000) + 0.5;
    tmp.blue = ((double)blue * 65535 / 1000) + 0.5;

    Colormap cmap = DefaultColormap(XCURSESDISPLAY,
                                    DefaultScreen(XCURSESDISPLAY));

    if (XAllocColor(XCURSESDISPLAY, cmap, &tmp))
        colors[color] = tmp.pixel;

    return OK;
}
