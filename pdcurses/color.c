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
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include <curspriv.h>
#include <stdlib.h>
#include <string.h>

RCSID("$Id: color.c,v 1.64 2006/10/11 06:21:21 wmcbrine Exp $");

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

	PDC_set_line_color() is used to set the color, globally, for the 
	color of the lines drawn for the attributes: A_UNDERLINE, 
	A_OVERLINE, A_LEFTLINE and A_RIGHTLINE.  PDCurses only feature.

	NOTE: COLOR_PAIR() and PAIR_NUMBER() are implemented as macros.

  X/Open Return Value:
	All functions return OK on success and ERR on error except for
	has_colors() and can_change_colors() which return TRUE or FALSE.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	start_color				-	-      3.2
	init_pair				-	-      3.2
	init_color				-	-      3.2
	has_colors				-	-      3.2
	can_change_color			-	-      3.2
	color_content				-	-      3.2
	pair_content				-	-      3.2
	PDC_set_line_color			-	-       -

**man-end****************************************************************/

int COLORS = 8;
int COLOR_PAIRS = PDC_COLOR_PAIRS;

static bool colorstarted = FALSE;

/* COLOR_PAIR to attribute encoding table. */

unsigned char *pdc_atrtab = (unsigned char *)NULL;

/* colorset[] tracks whether a pair has been set via init_pair() */

static unsigned char colorset[PDC_COLOR_PAIRS];

int start_color(void)
{
	PDC_LOG(("start_color() - called\n"));

	if (SP->mono)
		return ERR;

	if (SP->orig_attr && !getenv("PDC_ORIGINAL_COLORS"))
	{
		SP->orig_attr = FALSE;
		PDC_init_atrtab();
	}

	memset(colorset, 0, PDC_COLOR_PAIRS);

	colorstarted = TRUE;

	return OK;
}

static void _init_pair_core(short pairnum, short fg, short bg)
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

		pdc_atrtab[(pairnum * PDC_OFFSET) + i] = att;
	}
}

int init_pair(short colorpair, short foreground, short background)
{
	PDC_LOG(("init_pair() - called: colorpair %d fore %d back %d\n",
		colorpair, foreground, background));

	if (!colorstarted || colorpair >= COLOR_PAIRS || colorpair < 1)
		return ERR;

	/* To allow the PDC_PRESERVE_SCREEN option to work, we only 
	   reset curscr if this call to init_pair() alters a color pair 
	   created by the user. */

	if (colorset[colorpair])
	{
	    short oldforeground, oldbackground;

	    pair_content(colorpair, &oldforeground, &oldbackground);

	    if (oldforeground != foreground || oldbackground != background)
		curscr->_clear = TRUE;
	}

	_init_pair_core(colorpair, foreground, background);

	colorset[colorpair] = TRUE;

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

	if (color >= (COLORS * 2) || color < 0 || !PDC_can_change_color() ||
	    red < 0 || red > 1000 || green < 0 || green > 1000 ||
	    blue < 0 || blue > 1000)
		return ERR;

	return PDC_init_color(color, red, green, blue);
}

int color_content(short color, short *red, short *green, short *blue)
{
	PDC_LOG(("color_content() - called\n"));

	if (color >= (COLORS * 2) || color < 0 || !red || !green || !blue)
		return ERR;

	if (PDC_can_change_color())
		return PDC_color_content(color, red, green, blue);
	else
	{
		/* Simulated values for platforms that don't support 
		   palette changing */

		short maxval = (color >= COLORS) ? 1000 : 680;

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

int pair_content(short colorpair, short *foreground, short *background)
{
	PDC_LOG(("pair_content() - called\n"));

	if ((colorpair >= COLOR_PAIRS || colorpair < 1) ||
	    (!foreground || !background))
		return ERR;

	*foreground = (short)(pdc_atrtab[colorpair * PDC_OFFSET] & 0x0F);
	*background = (short)((pdc_atrtab[colorpair * PDC_OFFSET] & 0xF0) >> 4);

	return OK;
}

int PDC_set_line_color(short color)
{
	if (color >= COLORS || color < 0)
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
