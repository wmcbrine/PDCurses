/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcsdl.h"

RCSID("$Id: pdcscrn.c,v 1.23 2007/07/03 00:11:46 wmcbrine Exp $")

#include "deffont.h"
#include "deficon.h"

SDL_Surface *pdc_screen = NULL, *pdc_font = NULL, *pdc_icon = NULL;
int pdc_sheight = 0, pdc_swidth = 0, pdc_yoffset = 0, pdc_xoffset = 0;

SDL_Color pdc_color[16];
Uint32 pdc_mapped[16];
int pdc_fheight, pdc_fwidth, pdc_flastc;
bool pdc_own_screen;
WINDOW *pdc_lastscr;

/* COLOR_PAIR to attribute encoding table. */

static short *atrtab = (short *)NULL;

void PDC_scr_close(void)
{
	PDC_LOG(("PDC_scr_close() - called\n"));
}

void PDC_scr_free(void)
{
	delwin(pdc_lastscr);

	if (SP)
		free(SP);
	if (atrtab)
		free(atrtab);

	atrtab = (short *)NULL;
}

/* open the physical screen -- allocate SP, miscellaneous intialization */

int PDC_scr_open(int argc, char **argv)
{
	int i;

	PDC_LOG(("PDC_scr_open() - called\n"));

	SP = calloc(1, sizeof(SCREEN));
	atrtab = calloc(PDC_COLOR_PAIRS * 2 * sizeof(short), 1);

	if (!SP || !atrtab)
		return ERR;

	pdc_own_screen = !pdc_screen;

	if (pdc_own_screen)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			fprintf(stderr, "Could not start SDL: %s\n", 
				SDL_GetError());
			return ERR;
		}

	        atexit(SDL_Quit);
	}

	if (!pdc_font)
		pdc_font = SDL_LoadBMP("pdcfont.bmp");

	if (!pdc_font)
		pdc_font = SDL_LoadBMP_RW(SDL_RWFromMem(deffont,
			sizeof(deffont)), 0);

	if (!pdc_font)
	{
		fprintf(stderr, "Could not load font\n");
		return ERR;
	}

	if (!pdc_font->format->palette)
	{
		fprintf(stderr, "Font must have a palette\n");
		return ERR;
	}

	pdc_fheight = pdc_font->h / 8;
	pdc_fwidth = pdc_font->w / 32;
	pdc_flastc = pdc_font->format->palette->ncolors - 1;

	if (pdc_own_screen && !pdc_icon)
	{
		pdc_icon = SDL_LoadBMP("pdcicon.bmp");

		if (!pdc_icon)
			pdc_icon = SDL_LoadBMP_RW(SDL_RWFromMem(deficon,
				sizeof(deficon)), 0);

		if (pdc_icon)
			SDL_WM_SetIcon(pdc_icon, NULL);
	}

	if (pdc_own_screen)
	{
		pdc_sheight = 25 * pdc_fheight;
		pdc_swidth = 80 * pdc_fwidth;

		pdc_screen = SDL_SetVideoMode(pdc_swidth, pdc_sheight, 0,
			SDL_SWSURFACE|SDL_ANYFORMAT|SDL_RESIZABLE);
	}
	else
	{
		if (!pdc_sheight)
			pdc_sheight = pdc_screen->h - pdc_yoffset;

		if (!pdc_swidth)
			pdc_swidth = pdc_screen->w - pdc_xoffset;
	}

	if (!pdc_screen)
	{
		fprintf(stderr, "Couldn't create a surface: %s\n",
			SDL_GetError());
		return ERR;
	}

	for (i = 0; i < 8; i++)
	{
		pdc_color[i].r = (i & COLOR_RED) ? 0xc0 : 0;
		pdc_color[i].g = (i & COLOR_GREEN) ? 0xc0 : 0;
		pdc_color[i].b = (i & COLOR_BLUE) ? 0xc0 : 0;

		pdc_color[i + 8].r = (i & COLOR_RED) ? 0xff : 0x40;
		pdc_color[i + 8].g = (i & COLOR_GREEN) ? 0xff : 0x40;
		pdc_color[i + 8].b = (i & COLOR_BLUE) ? 0xff : 0x40;
	}

	for (i = 0; i < 16; i++)
		pdc_mapped[i] = SDL_MapRGB(pdc_screen->format,
			pdc_color[i].r, pdc_color[i].g, pdc_color[i].b);

	SDL_EnableUNICODE(1);

	PDC_mouse_set();

	if (pdc_own_screen)
		PDC_set_title(argc ? argv[0] : "PDCurses");

	SP->lines = PDC_get_rows();
	SP->cols = PDC_get_columns();

	SP->mouse_wait = PDC_CLICK_PERIOD;
	SP->audible = FALSE;

	PDC_reset_prog_mode();

	SP->mono = FALSE;

	pdc_lastscr = newwin(SP->lines, SP->cols, 0, 0);
	wattrset(pdc_lastscr, (chtype)(-1));
	werase(pdc_lastscr);

	return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
	if (!pdc_own_screen)
		return ERR;

	if (nlines && ncols)
	{
		pdc_sheight = nlines * pdc_fheight;
		pdc_swidth = ncols * pdc_fwidth;
	}

	SDL_FreeSurface(pdc_screen);

	pdc_screen = SDL_SetVideoMode(pdc_swidth, pdc_sheight, 0,
		SDL_SWSURFACE|SDL_ANYFORMAT|SDL_RESIZABLE);

	SP->resized = FALSE;
	SP->cursrow = SP->curscol = 0;

	wresize(pdc_lastscr, PDC_get_rows(), PDC_get_columns());
	werase(pdc_lastscr);

	return OK;
}

void PDC_reset_prog_mode(void)
{
	PDC_LOG(("PDC_reset_prog_mode() - called.\n"));

	PDC_flushinp();
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, 
		SDL_DEFAULT_REPEAT_INTERVAL);
}

void PDC_reset_shell_mode(void)
{
	PDC_LOG(("PDC_reset_shell_mode() - called.\n"));

	SDL_EnableKeyRepeat(0, 0);
	PDC_flushinp();
}

void PDC_restore_screen_mode(int i)
{
}

void PDC_save_screen_mode(int i)
{
}

void PDC_init_pair(short pair, short fg, short bg)
{
	atrtab[pair * 2] = fg;
	atrtab[pair * 2 + 1] = bg;
}

int PDC_pair_content(short pair, short *fg, short *bg)
{
	*fg = atrtab[pair * 2];
	*bg = atrtab[pair * 2 + 1];

	return OK;
}

bool PDC_can_change_color(void)
{
	return TRUE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
	*red = DIVROUND(pdc_color[color].r * 1000, 255);
	*green = DIVROUND(pdc_color[color].g * 1000, 255);
	*blue = DIVROUND(pdc_color[color].b * 1000, 255);

	return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
	pdc_color[color].r = DIVROUND(red * 255, 1000);
	pdc_color[color].g = DIVROUND(green * 255, 1000);
	pdc_color[color].b = DIVROUND(blue * 255, 1000);

	pdc_mapped[color] = SDL_MapRGB(pdc_screen->format,
		pdc_color[color].r, pdc_color[color].g, pdc_color[color].b);

	wrefresh(curscr);

	return OK;
}
