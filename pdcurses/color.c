/*
***************************************************************************
* This file comprises part of PDCurses. PDCurses is Public Domain software.
* You may use this code for whatever purposes you desire. This software
* is provided AS IS with NO WARRANTY whatsoever.
* Should this software be used in another application, an acknowledgement
* that PDCurses code is used would be appreciated, but is not mandatory.
*
* Any changes which you make to this software which may improve or enhance
* it, should be forwarded to the current maintainer for the benefit of
* other users.
*
* The only restriction placed on this code is that no distribution of
* modified PDCurses code be made under the PDCurses name, by anyone
* other than the current maintainer.
*
* See the file maintain.er for details of the current maintainer.
***************************************************************************
*/
#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef	start_color
#undef	init_pair
#undef	init_color
#undef	has_colors
#undef	can_change_color
#undef	color_content
#undef	pair_content
#undef	PDC_set_line_color

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef HAVE_PROTO
static void PDC_init_pair(short, short, short);
#else
static void PDC_init_pair();
#endif

#ifdef PDCDEBUG
char *rcsid_color = "$Id: color.c,v 1.18 2006/01/26 17:55:00 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

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
	init_pair(). After it has been initialized, COLOR_PAIR(n), a 
	macro defined in <curses.h>, can be used like any other video 
	attribute.

	start_color() initializes eight basic colors (black, red, green, 
	yellow, blue, magenta, cyan, and white), and two global 
	variables; COLORS and COLOR_PAIRS (respectively defining the 
	maximum number of colors and color-pairs the terminal is capable 
	of displaying).

	init_pair() changes the definitions of a color-pair. The routine 
	takes three arguments: the number of the color-pair to be 
	redefined, and the new values of the foreground and background 
	colors. The value of color-pair must be between 1 and 
	COLOR_PAIRS - 1. The values of foreground and background must be 
	between 0 and COLORS - 1 (this is a PDCurses abberation; 
	normally it is 0 and COLORS). If the color pair was previously 
	initialized, the screen is refreshed and all occurrences of that 
	color-pair are changed to the new definition.

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

	NOTE: has_colors() is implemented as a macro.

  X/Open Return Value:
	All functions return OK on success and ERR on error except for
	has_colors() and can_change_colors() which return TRUE or FALSE.

  X/Open Errors:
	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V   PDCurses
                                          Dec '88
      start_color                           -        -      3.2
      init_pair                             -        -      3.2
      init_color                            -        -      3.2
      has_colors                            -        -      3.2
      can_change_color                      -        -      3.2
      color_content                         -        -      3.2
      pair_content                          -        -      3.2
      PDC_set_line_color                    -        -       -       Y

**man-end**********************************************************************/

int COLORS = PDC_COLORS;
int COLOR_PAIRS = PDC_COLOR_PAIRS;

/* COLOR_PAIR to attribute encoding table. */

#if defined(CHTYPE_LONG)
# if defined(XCURSES)
unsigned char *atrtab = NULL;
# else
unsigned char atrtab[MAX_ATRTAB];
# endif
#else
unsigned char atrtab[MAX_ATRTAB] = 
{
	0x07,0x0F,0x70,0x78,0x87,0x8F,0xF0,0xF8, /* A_NORMAL */
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x00,0x08,0x70,0x78,0x80,0x88,0xF0,0xF8,
	0x70,0x00,0x17,0x00,0x00,0x00,0x00,0x00  /* for flash() */
};
#endif

unsigned char colorset[PDC_COLOR_PAIRS];

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	start_color(void)
#else
int	PDC_CDECL	start_color()
#endif
/***********************************************************************/
{
	PDC_LOG(("start_color() - called\n"));

	if (SP->mono)
		return ERR;

	COLORS = 8;

#if defined(CHTYPE_LONG)
	COLOR_PAIRS = PDC_COLOR_PAIRS;
	/* PDC_init_atrtab(); - already done in initscr() */
#else
	COLOR_PAIRS = 33;  /* actually only allows 32 */
#endif
	memset(colorset, 0, PDC_COLOR_PAIRS);
	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	init_pair(short colorpair, short foreground,
				  short background)
#else
int	PDC_CDECL	init_pair(colorpair, foreground, background)
short colorpair;
short foreground;
short background;
#endif
/***********************************************************************/
{
#if defined(CHTYPE_LONG) && !defined(XCURSES)
# define USE_PDC_INIT
#endif
	short oldforeground, oldbackground;
#ifndef USE_PDC_INIT
	unsigned char norm;
#endif
	PDC_LOG(("init_pair() - called: colorpair %d fore %d back %d\n",
		colorpair, foreground, background));

	if (colorpair >= COLOR_PAIRS || colorpair < 1)
		return ERR;

	oldforeground = (short)(atrtab[colorpair * PDC_OFFSET] & 0x0F);
	oldbackground = (short)((atrtab[colorpair * PDC_OFFSET] & 0xF0) >> 4);

#ifdef USE_PDC_INIT
	PDC_init_pair(colorpair, foreground, background);
#else
	norm = (unsigned char)(foreground & 0x0007) +
		((background & 0x0007) << 4);

	atrtab[(colorpair * PDC_OFFSET)] = norm;		/* normal */
# ifndef XCURSES
	atrtab[(colorpair * PDC_OFFSET) + 1] = norm + 8;	/* bold */
	atrtab[(colorpair * PDC_OFFSET) + 4] = norm + 128;	/* blink */
	atrtab[(colorpair * PDC_OFFSET) + 5] = norm + 8 + 128;	/* bold-blink */
# endif
#endif

/*
 * To allow the PDC_PRESERVE_SCREEN option to work, we only reset
 * curscr if this call to init_pair() alters a color pair created
 * by the user.
 */
	if (colorset[colorpair])
	{
	    if ( oldforeground != foreground || oldbackground != background )
		curscr->_clear = TRUE;
	}
	colorset[colorpair] = TRUE;
	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_CDECL	has_colors(void)
#else
bool	PDC_CDECL	has_colors()
#endif
/***********************************************************************/
{
	PDC_LOG(("has_colors() - called\n"));

	return SP->mono ? FALSE : TRUE;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	init_color(short color, short red, short green,
				   short blue)
#else
int	PDC_CDECL	init_color(color, red, green, blue)
short color;
short red;
short green;
short blue;
#endif
/***********************************************************************/
{
	PDC_LOG(("init_color() - called\n"));

	return ERR;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	color_content(short color, short *red, short *green,
				      short *blue)
#else
int	PDC_CDECL	color_content(color, red, green, blue)
short color;
short *red;
short *green;
short *blue;
#endif
/***********************************************************************/
{
	PDC_LOG(("color_content() - called\n"));

	/* A crude implementation. Does not account for intensity. 
	   ncurses uses 680 for non-A_BOLD, so let's copy that. - WJM3
	*/

	if ((color >= COLORS || color < 0) || (!red || !green || !blue))
		return ERR;

	*red = (color & COLOR_RED) ? 680 : 0;
	*green = (color & COLOR_GREEN) ? 680 : 0;
	*blue = (color & COLOR_BLUE) ? 680 : 0;

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_CDECL	can_change_color(void)
#else
bool	PDC_CDECL	can_change_color()
#endif
/***********************************************************************/
{
	PDC_LOG(("can_change_color() - called\n"));

	return FALSE;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	pair_content(int colorpair, short *foreground,
				     short *background)
#else
int	PDC_CDECL	pair_content(colorpair, foreground, background)
int colorpair;
short *foreground;
short *background;
#endif
/***********************************************************************/
{
	PDC_LOG(("pair_content() - called\n"));

	if ((colorpair >= COLOR_PAIRS || colorpair < 1) ||
	    (!foreground || !background))
		return ERR;

	*foreground = (short)(atrtab[colorpair * PDC_OFFSET] & 0x0F);
	*background = (short)((atrtab[colorpair * PDC_OFFSET] & 0xF0) >> 4);

	return OK;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	PDC_set_line_color(short color)
#else
int	PDC_CDECL	PDC_set_line_color(color)
short color;
#endif
/***********************************************************************/
{
	if (color >= COLORS || color < 0)
		return ERR;

	SP->line_color = color;

	return OK;
}

#if defined(CHTYPE_LONG)
/***********************************************************************/
#ifdef HAVE_PROTO
void	PDC_init_atrtab(void)
#else
void	PDC_init_atrtab()
#endif
/***********************************************************************/
{
	int i, orig_fore, orig_back;

	if ( SP->orig_attr == 0 )
	{
		orig_fore = COLOR_WHITE;
		orig_back = COLOR_BLACK;
	}
	else
	{
		orig_fore = SP->orig_attr & A_CHARTEXT;
		orig_back = (SP->orig_attr & A_ATTRIBUTES) >> 16;
	}

	for ( i = 0; i < PDC_COLOR_PAIRS; i++ )
		PDC_init_pair( i, orig_fore, orig_back );

	PDC_init_pair( PDC_COLOR_PAIRS, orig_back, orig_fore );
}

/***********************************************************************/
#ifdef HAVE_PROTO
static	void PDC_init_pair(short pairnum, short fg, short bg)
#else
static	void PDC_init_pair(pairnum, fg, bg)
short pairnum;
short fg;
short bg;
#endif
/***********************************************************************/
{
	unsigned char att, temp_bg;
	attr_t i;

	for (i = 0; i < PDC_OFFSET; i++)
	{
		att = fg | (bg << 4);

		if (i & (A_REVERSE >> 19))
			att = bg | (fg << 4);
		if (i & (A_UNDERLINE >> 19))
			att = 1;
		if (i & (A_INVIS >> 19))
		{
			temp_bg = att >> 4;
			att = temp_bg << 4 | temp_bg;
		}
		if (i & (A_BOLD >> 19))
			att |= 8;
		if (i & (A_BLINK >> 19))
			att |= 128;

		atrtab[(pairnum * PDC_OFFSET) + i] = att;
	}
}
#endif
