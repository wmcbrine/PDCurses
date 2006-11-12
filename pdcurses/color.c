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

#include <curspriv.h>
#include <stdlib.h>
#include <string.h>

RCSID("$Id: color.c,v 1.70 2006/11/12 10:26:14 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                         color

  Synopsis:
	int start_color(void);
	int init_pair(short pair, short fg, short bg);
	int init_color(short color, short red, short green, short blue);
	bool has_colors(void);
	bool can_change_color(void);
	int color_content(short color, short *redp, short *greenp,
			  short *bluep);
	int pair_content(short pair, short *fgp, short *bgp);
	int PDC_set_line_color(short color);

  X/Open Description:
	To use these routines, start_color() must be called, usually 
	immediately after initscr(). Colors are always used in pairs 
	refered to as color-pairs. A color-pair consists of a foreground 
	color and a background color. A color-pair is initialized with 
	init_pair(). After it has been initialized, COLOR_PAIR(n) can be 
	used like any other video attribute.

	start_color() initializes eight basic colors (black, red, green, 
	yellow, blue, magenta, cyan, and white), and two global 
	variables; COLORS and COLOR_PAIRS (respectively defining the 
	maximum number of colors and color-pairs the terminal is capable 
	of displaying).

	init_pair() changes the definitions of a color-pair. The routine 
	takes three arguments: the number of the color-pair to be 
	redefined, and the new values of the foreground and background 
	colors. The value of color-pair must be between 0 and 
	COLOR_PAIRS - 1, inclusive. The values of foreground and 
	background must be between 0 and COLORS - 1, inclusive. If the 
	color pair was previously initialized, the screen is refreshed 
	and all occurrences of that color-pair are changed to the new 
	definition.

	has_colors() indicates if the terminal supports, and can 
	maniplulate color. It returns TRUE or FALSE.

	can_change_color() indicates if the terminal has the capability
	to change the definition of its colors.

	pair_content() is used to determine what the colors of a given
	color-pair consist of.

  PDCurses Description:
	PDC_set_line_color() is used to set the color, globally, for the 
	color of the lines drawn for the attributes: A_UNDERLINE, 
	A_OVERLINE, A_LEFTLINE and A_RIGHTLINE. A value of -1 (the 
	default) indicates that the current foreground color should be 
	used.

	NOTE: COLOR_PAIR() and PAIR_NUMBER() are implemented as macros.

  X/Open Return Value:
	All functions return OK on success and ERR on error except for
	has_colors() and can_change_colors() which return TRUE or FALSE.

  Portability				     X/Open    BSD    SYS V
	start_color				Y	-      3.2
	init_pair				Y	-      3.2
	init_color				Y	-      3.2
	has_colors				Y	-      3.2
	can_change_color			Y	-      3.2
	color_content				Y	-      3.2
	pair_content				Y	-      3.2
	PDC_set_line_color			-	-       -

**man-end****************************************************************/

int COLORS = 0;
int COLOR_PAIRS = PDC_COLOR_PAIRS;

bool pdc_color_started = FALSE;

/* COLOR_PAIR to attribute encoding table. */

unsigned char *pdc_atrtab = (unsigned char *)NULL;

/* pair_set[] tracks whether a pair has been set via init_pair() */

static bool pair_set[PDC_COLOR_PAIRS];

int start_color(void)
{
	PDC_LOG(("start_color() - called\n"));

	if (SP->mono)
		return ERR;

	pdc_color_started = TRUE;

	PDC_set_blink(FALSE);	/* Also sets COLORS, to 8 or 16 */

	if (SP->orig_attr && !getenv("PDC_ORIGINAL_COLORS"))
	{
		SP->orig_attr = FALSE;
		PDC_init_atrtab();
	}

	memset(pair_set, 0, PDC_COLOR_PAIRS);

	return OK;
}

static void _init_pair_core(short pair, short fg, short bg)
{
	unsigned char att, temp_bg;
	chtype i;

	for (i = 0; i < PDC_OFFSET; i++)
	{
		att = fg | (bg << 4);

		if (i & (A_REVERSE >> PDC_ATTR_SHIFT))
			att = bg | (fg << 4);
		if (i & (A_UNDERLINE >> PDC_ATTR_SHIFT))
			att = 1;
		if (i & (A_INVIS >> PDC_ATTR_SHIFT))
		{
			temp_bg = att >> 4;
			att = temp_bg << 4 | temp_bg;
		}
		if (i & (A_BOLD >> PDC_ATTR_SHIFT))
			att |= 8;
		if (i & (A_BLINK >> PDC_ATTR_SHIFT))
			att |= 128;

		pdc_atrtab[pair * PDC_OFFSET + i] = att;
	}
}

int init_pair(short pair, short fg, short bg)
{
	PDC_LOG(("init_pair() - called: pair %d fg %d bg %d\n", pair, fg, bg));

	if (!pdc_color_started || pair < 1 || pair >= COLOR_PAIRS ||
	    fg < 0 || fg >= COLORS || bg < 0 || bg >= COLORS)
		return ERR;

	/* To allow the PDC_PRESERVE_SCREEN option to work, we only 
	   reset curscr if this call to init_pair() alters a color pair 
	   created by the user. */

	if (pair_set[pair])
	{
		short oldfg, oldbg;

		pair_content(pair, &oldfg, &oldbg);

		if (oldfg != fg || oldbg != bg)
			curscr->_clear = TRUE;
	}

	_init_pair_core(pair, fg, bg);

	pair_set[pair] = TRUE;

	return OK;
}

bool has_colors(void)
{
	PDC_LOG(("has_colors() - called\n"));

	return !(SP->mono);
}

int init_color(short color, short red, short green, short blue)
{
	PDC_LOG(("init_color() - called\n"));

	if (color < 0 || color >= COLORS || !PDC_can_change_color() ||
	    red < 0 || red > 1000 || green < 0 || green > 1000 ||
	    blue < 0 || blue > 1000)
		return ERR;

	return PDC_init_color(color, red, green, blue);
}

int color_content(short color, short *red, short *green, short *blue)
{
	PDC_LOG(("color_content() - called\n"));

	if (color < 0 || color >= COLORS || !red || !green || !blue)
		return ERR;

	if (PDC_can_change_color())
		return PDC_color_content(color, red, green, blue);
	else
	{
		/* Simulated values for platforms that don't support 
		   palette changing */

		short maxval = (color & 8) ? 1000 : 680;

		*red = (color & COLOR_RED) ? maxval : 0;
		*green = (color & COLOR_GREEN) ? maxval : 0;
		*blue = (color & COLOR_BLUE) ? maxval : 0;

		return OK;
	}
}

bool can_change_color(void)
{
	PDC_LOG(("can_change_color() - called\n"));

	return PDC_can_change_color();
}

int pair_content(short pair, short *fg, short *bg)
{
	PDC_LOG(("pair_content() - called\n"));

	if (pair < 1 || pair >= COLOR_PAIRS || !fg || !bg)
		return ERR;

	*fg = (short)(pdc_atrtab[pair * PDC_OFFSET] & 0x0F);
	*bg = (short)((pdc_atrtab[pair * PDC_OFFSET] & 0xF0) >> 4);

	return OK;
}

int PDC_set_line_color(short color)
{
	if (color < -1 || color >= COLORS)
		return ERR;

	SP->line_color = color;

	return OK;
}

void PDC_init_atrtab(void)
{
	int i;

	if (!SP->orig_attr)
	{
		SP->orig_fore = COLOR_WHITE;
		SP->orig_back = COLOR_BLACK;
	}

	for (i = 0; i < PDC_COLOR_PAIRS; i++)
		_init_pair_core(i, SP->orig_fore, SP->orig_back);
}
