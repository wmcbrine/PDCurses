/************************************************************************** 
* This file is part of PDCurses. PDCurses is public domain software;
* you may use it for any purpose. This software is provided AS IS with
* NO WARRANTY whatsoever.
*
* If you use PDCurses in an application, an acknowledgement would be
* appreciated, but is not mandatory. If you make corrections or
* enhancements to PDCurses, please forward them to the current
* maintainer for the benefit of other users.
*
* No distribution of modified PDCurses code may be made under the name
* "PDCurses", except by the current maintainer. (Although PDCurses is
* public domain, the name is a trademark.)
*
* See the file maintain.er for details of the current maintainer.
**************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>

/* undefine any macros for functions defined in this module */
#undef slk_init
#undef slk_set
#undef slk_refresh
#undef slk_noutrefresh
#undef slk_label
#undef slk_clear
#undef slk_restore
#undef slk_touch
#undef slk_attron
#undef slk_attrset
#undef slk_attroff
#undef slk_color

#ifdef PDCDEBUG
const char *rcsid_slk =
	"$Id: slk.c,v 1.19 2006/02/16 22:59:49 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          slk

  Synopsis:
	int slk_init(int fmt);
	int slk_set(int labnum, const char *label, int fmt);
	int slk_refresh(void);
	int slk_noutrefresh(void);
	char *slk_label(int labnum);
	int slk_clear(void);
	int slk_restore(void);
	int slk_touch(void);
	int slk_attron(const chtype attrs);
	int slk_attrset(const chtype attrs);
	int slk_attroff(const chtype attrs);
	int slk_color(short color_pair);

  X/Open Description:
	These functions manipulate a window that contain Soft Label Keys 
	(SLK). To use the SLK functions, a call to slk_init() must be 
	made BEFORE initscr() or newterm(). slk_init() removes 1 or 2 
	lines from the useable screen, depending on the format selected.

	The line(s) removed from the screen are used as a separate 
	window, in which SLKs are displayed.

	slk_init() requires a single parameter which describes the 
	format of the SLKs as follows:
 
 		0	3-2-3 format
 		1	4-4 format
 		2	4-4-4 format (ncurses extension)
 		3	4-4-4 format with index line (ncurses extension)
			2 lines used
 		55	5-5 format (pdcurses format)

	The functions slk_refresh(), slk_noutrefresh() and slk_touch() 
	are analagous to refresh(), noutrefresh() and touch() functions.

  PDCurses Description:
	System V compatible color support is included.
	See <curses.h> for further details.

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	slk_init				Y	-	Y
	slk_set					Y	-	Y
	slk_refresh				Y	-	Y
	slk_noutrefresh				Y	-	Y
	slk_label				Y	-	Y
	slk_clear				Y	-	Y
	slk_restore				Y	-	Y
	slk_touch				Y	-	Y
	slk_attron				Y	-	Y
	slk_attrset				Y	-	Y
	slk_attroff				Y	-	Y

**man-end**********************************************************************/


#define LABEL_NORMAL	8
#define LABEL_EXTENDED	10
#define LABEL_NCURSES_EXTENDED	12

char slk_temp_string[64];

static int slk_start_col[LABEL_NCURSES_EXTENDED];
static chtype slk_attributes[LABEL_NCURSES_EXTENDED];
static int space_seperator = 1;
static int label_length = 0;
static int labels = 0;
static int label_fmt = 0;
static int label_line = 0;

void (*PDC_initial_slk)(void);
static void PDC_slk_init(void);

static struct {
	char	label[32];
	int	format;
} slk_save[LABEL_NCURSES_EXTENDED];


/* slk_init() is the slk initialization routine.
   This must be called before initscr().

   label_fmt = 0, 1 or 55.
       0 = 3-2-3 format
       1 = 4 - 4 format
       2 = 4-4-4 format	(ncurses extension for PC 12 function keys)
       3 = 4-4-4 format	(ncurses extension for PC 12 function keys -
	with index line)
      55 = 5 - 5 format	(extended for PC, 10 function keys) */

int slk_init(int fmt)
{
	PDC_LOG(("slk_init() - called\n"));

	if (SP)
		return ERR;

	switch (fmt) 
	{
	case 0:  /* 3 - 2 - 3 */
		labels = LABEL_NORMAL;
		break;

	case 1:   /* 4 - 4 */
		labels = LABEL_NORMAL;
		break;

	case 2:   /* 4 4 4 */
		labels = LABEL_NCURSES_EXTENDED;
		break;

	case 3:   /* 4 4 4  with index */
		labels = LABEL_NCURSES_EXTENDED;
		break;

	case 55:  /* 5 - 5 */
		labels = LABEL_EXTENDED;
		break;

	default:
		return ERR;
	}

	PDC_initial_slk = PDC_slk_init;
	label_fmt = fmt;

	return OK;
}

/* PDC_slk_set() Used to set a slk label to a string.

   label_num = 1 - 8 (or 10) (number of the label)
   label_str = string (8 or 7 bytes total), NULL chars or NULL pointer
   label_fmt =  justification
      0 = left
      1 = center
      2 = right
   save = 1 yes or 0 no  */

static int PDC_slk_set(int label_num, const char *label_str,
		       int label_fmt, int save)
{
	int i, num, slen, llen, col;

	PDC_LOG(("PDC_slk_set() - called\n"));

	if (label_num < 1 || label_num > labels ||
	    label_fmt < 0 || label_fmt > 2)
		return ERR;

	num = label_num - 1;

	/* A NULL in either the first byte or pointer
	   indicates a clearing of the label. */

	if (label_str == (char *)0 || *label_str == '\0') 
	{
		slk_attributes[num] = SP->slk_winptr->_attrs;
		wmove(SP->slk_winptr, label_line, slk_start_col[num]);

		for (i = 0; i < label_length; ++i)
			waddch(SP->slk_winptr, ' ');

		/* Save the string and attribute */

		if (save)
		{
			*slk_save[num].label = '\0';
			slk_save[num].format = 0;
		}

		wmove(SP->slk_winptr, label_line, 0);   /* park it */

		return OK;
	}

	/* Otherwise, format the character and put in position. */

	memset(slk_temp_string, 0, sizeof(slk_temp_string));
	strncpy(slk_temp_string, label_str, label_length);

	/* Save the string and attribute */

	if (save)
	{
		strcpy(slk_save[num].label, slk_temp_string);
		slk_save[num].format = label_fmt;
	}

	slk_attributes[num] = SP->slk_winptr->_attrs;
	wmove(SP->slk_winptr, label_line, slk_start_col[num]);

	for (i = 0; i < label_length; ++i)
		waddch(SP->slk_winptr, ' ');

	wmove(SP->slk_winptr, label_line, slk_start_col[num]);

	switch (label_fmt)
	{
	case 0:  /* LEFT */
		waddstr(SP->slk_winptr, slk_temp_string);
		break;

	case 1:  /* CENTER */
		slen = strlen(slk_temp_string);
		llen = label_length / 2;
		col = llen - slen / 2;

		if (col + slen > label_length)
			--col;

		mvwaddstr(SP->slk_winptr, label_line,
			slk_start_col[num] + col, slk_temp_string);
		break;

	default:  /* RIGHT */
		slen = strlen(slk_temp_string);
		col = label_length - slen;

		mvwaddstr(SP->slk_winptr, label_line,
			slk_start_col[num] + col, slk_temp_string);
		break;
	}

	wmove(SP->slk_winptr, label_line, 0);	/* park it */

	return OK;
}

/*  slk_set() Used to set a slk label to a string.

   label_num = 1 - 8 (or 10) (number of the label)
   label_str = string (8 or 7 bytes total), NULL chars or NULL pointer
   label_fmt =  justification
      0 = left
      1 = center
      2 = right  */

int slk_set(int label_num, const char *label_str, int label_fmt)
{
	PDC_LOG(("slk_set() - called\n"));

	return PDC_slk_set(label_num, label_str, label_fmt, 1);
}

int slk_refresh(void)
{
	PDC_LOG(("slk_refresh() - called\n"));

	if (SP->slk_winptr == (WINDOW *)NULL)
		return ERR;

	slk_noutrefresh();
	return doupdate();
}

int slk_noutrefresh(void)
{
	PDC_LOG(("slk_noutrefresh() - called\n"));

	if (SP->slk_winptr == (WINDOW *)NULL)
		return ERR;

	return wnoutrefresh(SP->slk_winptr);
}

char *slk_label(int labnum)
{
	PDC_LOG(("slk_label() - called\n"));

	if (labnum < 0 || labnum > labels)
		return (char *)0;

	memset(slk_temp_string, 0, sizeof (slk_temp_string));
	strcpy(slk_temp_string, slk_save[labnum - 1].label);

	return slk_temp_string;	/* only good temporarily */
}

int slk_clear(void)
{
	int i;

	PDC_LOG(("slk_clear() - called\n"));

	for (i = 0; i < labels; ++i)
	{
		wattrset(SP->slk_winptr, slk_attributes[i]);
		PDC_slk_set(i + 1, "", 0, 0);
	}

	return wrefresh(SP->slk_winptr);
}

int slk_restore(void)
{
	int i;
	chtype attr = SP->slk_winptr->_attrs;

	PDC_LOG(("slk_restore() - called\n"));

	for (i = 0; i < labels; ++i)
	{
		wattrset(SP->slk_winptr, slk_attributes[i]);
		PDC_slk_set(i + 1, slk_save[i].label, slk_save[i].format, 0);
	}

	SP->slk_winptr->_attrs = attr;

	return wrefresh(SP->slk_winptr);
}

int slk_touch(void)
{
	PDC_LOG(("slk_touch() - called\n"));

	return touchwin(SP->slk_winptr);
}

int slk_attron(const chtype attrs)
{
	int i, rc;

	PDC_LOG(("slk_attron() - called\n"));

	rc = wattron(SP->slk_winptr, attrs);

	for (i = 0; i < labels; ++i)
		PDC_slk_set(i + 1, slk_save[i].label, slk_save[i].format, 0);

	return rc;
}

int slk_attroff(const chtype attrs)
{
	int i, rc;

	PDC_LOG(("slk_attroff() - called\n"));

	rc = wattroff(SP->slk_winptr, attrs);

	for (i = 0; i < labels; ++i)
		PDC_slk_set(i + 1, slk_save[i].label, slk_save[i].format, 0);

	return rc;
}

int slk_attrset(const chtype attrs)
{
	int i, rc;

	PDC_LOG(("slk_attrset() - called\n"));

	rc = wattrset(SP->slk_winptr, attrs);

	for (i = 0; i < labels; ++i)
		PDC_slk_set(i + 1, slk_save[i].label, slk_save[i].format, 0);

	return rc;
}

int slk_color(short color_pair)
{
	int i, rc;

	PDC_LOG(("slk_color() - called\n"));

	rc = wcolor_set(SP->slk_winptr, color_pair, NULL);

	for (i = 0; i < labels; ++i)
		PDC_slk_set(i + 1, slk_save[i].label, slk_save[i].format, 0);

	return rc;
}

static void PDC_slk_init(void)
{
	int i;
	chtype save_attr;

	PDC_LOG(("PDC_slk_init() - called\n"));

	if (label_fmt == 3)
	{
		SP->slklines = 2;
		label_line = 1;
	}
	else
		SP->slklines = 1;

	if (!SP->slk_winptr)
	{
		if ((SP->slk_winptr = newwin(SP->slklines, COLS, 
		     LINES-SP->slklines, 0)) == (WINDOW *)0)
			return;

		wattrset(SP->slk_winptr, A_REVERSE);
	}

	PDC_slk_calc();

	/* if we have an index line, display it now */

	if (label_fmt == 3)
	{
		save_attr = SP->slk_winptr->_attrs;
		wattrset(SP->slk_winptr,A_NORMAL);
		wmove(SP->slk_winptr, 0, 0);
		whline(SP->slk_winptr, 0, COLS);

		for (i = 0; i < labels; i++)
			mvwprintw(SP->slk_winptr, 0, slk_start_col[i], 
				"F%d", i + 1);

		SP->slk_winptr->_attrs = save_attr;
	}

	touchwin(SP->slk_winptr);
}

void PDC_slk_calc(void)
{
	int i, center, col = 0;

	PDC_LOG(("PDC_slk_calc() - called\n"));

	switch (label_fmt) 
	{
	case 0:  /* 3 - 2 - 3 */
		label_length = (COLS / labels) - space_seperator - 1;
		break;

	case 1:   /* 4 - 4 */
		label_length = (COLS / labels) - space_seperator;
		break;

	case 2:   /* 4 4 4 */
	case 3:   /* 4 4 4 with index */
		label_length = (COLS / labels) - space_seperator;
		break;

	case 55:  /* 5 - 5 */
		label_length = (COLS / labels) - space_seperator;
		break;

	default: /* should never get here!! */
		return;
	}

	/* set default attribute */

	for (i = 0; i < labels; ++i)
		slk_attributes[i] = A_REVERSE;

	switch (label_fmt)
	{
	case 0:     /* 3 - 2 - 3 F-Key layout */
		slk_start_col[0] = col;
		slk_start_col[1] = (col += label_length + space_seperator);
		slk_start_col[2] = (col += label_length + space_seperator);

		center = COLS / 2;

		slk_start_col[3] = center - label_length;
		slk_start_col[4] = center + space_seperator;

		col = COLS - ((label_length + space_seperator) * 2 +
			label_length);

		slk_start_col[5] = col;
		slk_start_col[6] = (col += label_length + space_seperator);
		slk_start_col[7] = (col += label_length + space_seperator);
		break;

	case 1:     /* 4 - 4 F-Key layout */
		slk_start_col[0] = col;
		slk_start_col[1] = (col += label_length + space_seperator);
		slk_start_col[2] = (col += label_length + space_seperator);
		slk_start_col[3] = (col += label_length + space_seperator);

		col = COLS - ((label_length + space_seperator) * 3 + 
			label_length);

		slk_start_col[4] = col;
		slk_start_col[5] = (col += label_length + space_seperator);
		slk_start_col[6] = (col += label_length + space_seperator);
		slk_start_col[7] = (col += label_length + space_seperator);
		break;

	case 2:     /* 4 4 4 F-Key layout */
	case 3:     /* 4 4 4 F-Key layout with index */
		slk_start_col[0] = col;
		slk_start_col[1] = (col += label_length + space_seperator);
		slk_start_col[2] = (col += label_length + space_seperator);
		slk_start_col[3] = (col += label_length + space_seperator);

		center = COLS/2;

		slk_start_col[4] = center - (2 * label_length) -
			space_seperator;
		slk_start_col[5] = center - label_length;
		slk_start_col[6] = center + space_seperator;
		slk_start_col[7] = center + label_length +
			(2 * space_seperator);

		col = COLS - ((label_length + space_seperator) * 3 + 
			label_length);

		slk_start_col[8] = col;
		slk_start_col[9] = (col += label_length + space_seperator);
		slk_start_col[10] = (col += label_length + space_seperator);
		slk_start_col[11] = (col += label_length + space_seperator);
		break;

	default:    /* 5 - 5 F-Key layout */
		slk_start_col[0] = col;
		slk_start_col[1] = (col += label_length + space_seperator);
		slk_start_col[2] = (col += label_length + space_seperator);
		slk_start_col[3] = (col += label_length + space_seperator);
		slk_start_col[4] = (col += label_length + space_seperator);

		col = COLS - ((label_length + space_seperator) * 4 + 
			label_length);

		slk_start_col[5] = col;
		slk_start_col[6] = (col += label_length + space_seperator);
		slk_start_col[7] = (col += label_length + space_seperator);
		slk_start_col[8] = (col += label_length + space_seperator);
		slk_start_col[9] = (col += label_length + space_seperator);
		break;
	}

	/* make sure labels are all in window */

	for (i = 0; i < labels; ++i)
		slk_set(i + 1, slk_save[i].label, slk_save[i].format);
}

int PDC_mouse_in_slk(int y, int x)
{
	int i;

	PDC_LOG(("PDC_mouse_in_slk() - called: y->%d x->%d\n", y, x));

	/* If the line on which the mouse was clicked is NOT the last 
	   line of the screen, we are not interested in it. */

	if ((SP->slk_winptr == NULL) ||
	    (y != SP->slk_winptr->_begy + label_line))
		return 0;

	for (i = 0; i < labels; i++)
		if (x >= slk_start_col[i]
		 && x <= slk_start_col[i] + label_length - 1)
			return i + 1;

	return 0;
}
