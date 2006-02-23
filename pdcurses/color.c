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

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef COLOR_PAIR
#undef PAIR_NUMBER
#undef start_color
#undef init_pair
#undef init_color
#undef has_colors
#undef can_change_color
#undef color_content
#undef pair_content
#undef PDC_set_line_color

static void PDC_init_pair(short, short, short);

#ifdef PDCDEBUG
const char *rcsid_color =
	"$Id: color.c,v 1.42 2006/02/23 01:46:52 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  Name:                                                         color

  Synopsis:
	chtype COLOR_PAIR(int n);
	int PAIR_NUMBER(chtype value);
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
	to change the definition of its colors. Although this is possible,
	at least with VGA monitors, this function always returns FALSE.

	pair_content() is used to determine what the colors of a given
	color-pair consist of.

	PDC_set_line_color() is used to set the color, globally, for the 
	color of the lines drawn for the attributes: A_UNDERLINE, 
	A_OVERLINE, A_LEFTLINE and A_RIGHTLINE.  PDCurses only feature.

	NOTE: COLOR_PAIR(), PAIR_NUMBER() and has_colors() are
	implemented as macros.

  X/Open Return Value:
	All functions return OK on success and ERR on error except for
	has_colors() and can_change_colors() which return TRUE or FALSE.

  X/Open Errors:
	No errors are defined for this function.

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

int COLORS = PDC_COLORS;
int COLOR_PAIRS = PDC_COLOR_PAIRS;

static bool colorstarted = FALSE;

/* COLOR_PAIR to attribute encoding table. */

#if defined(XCURSES)
unsigned char *atrtab = NULL;
#else
unsigned char atrtab[MAX_ATRTAB];
#endif

unsigned char colorset[PDC_COLOR_PAIRS];

chtype COLOR_PAIR(int n)
{
	return ((chtype)n << PDC_COLOR_SHIFT) & A_COLOR;
}

int PAIR_NUMBER(chtype value)
{
	return (value & A_COLOR) >> PDC_COLOR_SHIFT;
}

int start_color(void)
{
	PDC_LOG(("start_color() - called\n"));

	if (SP->mono)
		return ERR;

	/* PDC_init_atrtab(); - already done in initscr() */

	memset(colorset, 0, PDC_COLOR_PAIRS);

	colorstarted = TRUE;

	return OK;
}

int init_pair(short colorpair, short foreground, short background)
{
	short oldforeground, oldbackground;

	PDC_LOG(("init_pair() - called: colorpair %d fore %d back %d\n",
		colorpair, foreground, background));

	if (!colorstarted || colorpair >= COLOR_PAIRS || colorpair < 1)
		return ERR;

	oldforeground = (short)(atrtab[colorpair * PDC_OFFSET] & 0x0F);
	oldbackground = (short)((atrtab[colorpair * PDC_OFFSET] & 0xF0) >> 4);

	PDC_init_pair(colorpair, foreground, background);

	/* To allow the PDC_PRESERVE_SCREEN option to work, we only 
	   reset curscr if this call to init_pair() alters a color pair 
	   created by the user. */

	if (colorset[colorpair])
	    if (oldforeground != foreground || oldbackground != background)
		curscr->_clear = TRUE;

	colorset[colorpair] = TRUE;

	return OK;
}

bool has_colors(void)
{
	PDC_LOG(("has_colors() - called\n"));

	return SP->mono ? FALSE : TRUE;
}

int init_color(short color, short red, short green, short blue)
{
	PDC_LOG(("init_color() - called\n"));

	return ERR;
}

int color_content(short color, short *red, short *green, short *blue)
{
	PDC_LOG(("color_content() - called\n"));

	/* A crude implementation. Does not account for intensity. 
	   ncurses uses 680 for non-A_BOLD, so let's copy that. - WJM3 	*/

	if ((color >= COLORS || color < 0) || (!red || !green || !blue))
		return ERR;

	*red = (color & COLOR_RED) ? 680 : 0;
	*green = (color & COLOR_GREEN) ? 680 : 0;
	*blue = (color & COLOR_BLUE) ? 680 : 0;

	return OK;
}

bool can_change_color(void)
{
	PDC_LOG(("can_change_color() - called\n"));

	return FALSE;
}

int pair_content(short colorpair, short *foreground,
			   short *background)
{
	PDC_LOG(("pair_content() - called\n"));

	if ((colorpair >= COLOR_PAIRS || colorpair < 1) ||
	    (!foreground || !background))
		return ERR;

	*foreground = (short)(atrtab[colorpair * PDC_OFFSET] & 0x0F);
	*background = (short)((atrtab[colorpair * PDC_OFFSET] & 0xF0) >> 4);

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
	int i, orig_fore, orig_back;

	if (SP->orig_attr == 0)
	{
		orig_fore = COLOR_WHITE;
		orig_back = COLOR_BLACK;
	}
	else
	{
		orig_fore = SP->orig_attr & A_CHARTEXT;
		orig_back = (SP->orig_attr & A_ATTRIBUTES) >> 16;
	}

	for (i = 0; i < PDC_COLOR_PAIRS; i++)
		PDC_init_pair(i, orig_fore, orig_back);
}

static void PDC_init_pair(short pairnum, short fg, short bg)
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

		atrtab[(pairnum * PDC_OFFSET) + i] = att;
	}
}
