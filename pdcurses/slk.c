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
#include <string.h>

RCSID("$Id: slk.c,v 1.37 2006/10/30 09:08:15 wmcbrine Exp $");

/*man-start**************************************************************

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
	int slk_attr_on(const attr_t attrs, void *opts);
	int slk_attrset(const chtype attrs);
	int slk_attr_set(const attr_t attrs, short color_pair, void *opts);
	int slk_attroff(const chtype attrs);
	int slk_attr_off(const attr_t attrs, void *opts);
	int slk_color(short color_pair);

	int slk_wset(int labnum, const wchar_t *label, int fmt);

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
	slk_attr_on				Y
	slk_attr_set				Y
	slk_attr_off				Y
	slk_wset				Y

**man-end****************************************************************/


#define LABEL_NORMAL	8
#define LABEL_EXTENDED	10
#define LABEL_NCURSES_EXTENDED	12

static chtype slk_temp_string[64];

static int slk_start_col[LABEL_NCURSES_EXTENDED];
static int label_length = 0;
static int labels = 0;
static int label_fmt = 0;
static int label_line = 0;
static bool hidden = FALSE;

void (*PDC_initial_slk)(void);
static void _slk_initial(void);
static void _slk_calc(void);

static struct {
	chtype	label[32];
	int	len;
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

	PDC_initial_slk = _slk_initial;
	label_fmt = fmt;

	return OK;
}

/* draw a single button */

static void _drawone(int num)
{
	int i, col, slen;

	if (hidden)
		return;

	slen = slk_save[num].len;

	switch (slk_save[num].format)
	{
	case 0:  /* LEFT */
		col = 0;
		break;

	case 1:  /* CENTER */
		col = (label_length - slen) / 2;

		if (col + slen > label_length)
			--col;
		break;

	default:  /* RIGHT */
		col = label_length - slen;
	}

	wmove(SP->slk_winptr, label_line, slk_start_col[num]);

	for (i = 0; i < label_length; ++i)
		waddch(SP->slk_winptr, (i >= col && i < (col + slen)) ?
			slk_save[num].label[i - col] : ' ');
}

/* redraw each button */

static void _redraw(void)
{
	int i;

	for (i = 0; i < labels; ++i)
		_drawone(i);
}

/* slk_set() Used to set a slk label to a string.

   label_num = 1 - 8 (or 10) (number of the label)
   label_str = string (8 or 7 bytes total), NULL chars or NULL pointer
   label_fmt =  justification
      0 = left
      1 = center
      2 = right  */

int slk_set(int label_num, const char *label_str, int label_fmt)
{
	PDC_LOG(("slk_set() - called\n"));

	int i;

	if (label_num < 1 || label_num > labels ||
	    label_fmt < 0 || label_fmt > 2)
		return ERR;

	label_num--;

	/* A NULL in either the first byte or pointer
	   indicates a clearing of the label. */

	if (label_str == (char *)0 || *label_str == '\0') 
	{
		/* Save the string and attribute */

		*slk_save[label_num].label = 0;
		slk_save[label_num].format = 0;
		slk_save[label_num].len = 0;
	}
	else
	{

		/* Otherwise, format the character and put in position */

		for (i = 0; i < label_length; i++)
		{
			chtype ch = label_str[i];

			slk_save[label_num].label[i] = ch;

			if (!ch)
				break;
		}

		/* Save the string and attribute */

		slk_save[label_num].label[label_length] = 0;
		slk_save[label_num].format = label_fmt;
		slk_save[label_num].len = i;
	}

	_drawone(label_num);

	return OK;
}

int slk_refresh(void)
{
	PDC_LOG(("slk_refresh() - called\n"));

	return (slk_noutrefresh() == ERR) ? ERR : doupdate();
}

int slk_noutrefresh(void)
{
	PDC_LOG(("slk_noutrefresh() - called\n"));

	return wnoutrefresh(SP->slk_winptr);
}

char *slk_label(int labnum)
{
	static char temp[64];
	chtype *p;
	int i;

	PDC_LOG(("slk_label() - called\n"));

	if (labnum < 0 || labnum > labels)
		return (char *)0;

	for (i = 0, p = slk_save[labnum - 1].label; *p; i++)
		temp[i] = *p++;

	temp[i] = '\0';

	return temp;
}

int slk_clear(void)
{
	PDC_LOG(("slk_clear() - called\n"));

	hidden = TRUE;
	werase(SP->slk_winptr);
	return wrefresh(SP->slk_winptr);
}

int slk_restore(void)
{
	PDC_LOG(("slk_restore() - called\n"));

	hidden = FALSE;
	_redraw();
	return wrefresh(SP->slk_winptr);
}

int slk_touch(void)
{
	PDC_LOG(("slk_touch() - called\n"));

	return touchwin(SP->slk_winptr);
}

int slk_attron(const chtype attrs)
{
	int rc;

	PDC_LOG(("slk_attron() - called\n"));

	rc = wattron(SP->slk_winptr, attrs);
	_redraw();

	return rc;
}

int slk_attr_on(const attr_t attrs, void *opts)
{
	PDC_LOG(("slk_attr_on() - called\n"));

	return slk_attron(attrs);
}

int slk_attroff(const chtype attrs)
{
	int rc;

	PDC_LOG(("slk_attroff() - called\n"));

	rc = wattroff(SP->slk_winptr, attrs);
	_redraw();

	return rc;
}

int slk_attr_off(const attr_t attrs, void *opts)
{
	PDC_LOG(("slk_attr_off() - called\n"));

	return slk_attroff(attrs);
}

int slk_attrset(const chtype attrs)
{
	int rc;

	PDC_LOG(("slk_attrset() - called\n"));

	rc = wattrset(SP->slk_winptr, attrs);
	_redraw();

	return rc;
}

int slk_color(short color_pair)
{
	int rc;

	PDC_LOG(("slk_color() - called\n"));

	rc = wcolor_set(SP->slk_winptr, color_pair, NULL);
	_redraw();

	return rc;
}

int slk_attr_set(const attr_t attrs, short color_pair, void *opts)
{
	PDC_LOG(("slk_attr_set() - called\n"));

	return slk_attrset(attrs | COLOR_PAIR(color_pair));
}

static void _slk_initial(void)
{
	int i;
	chtype save_attr;

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

	_slk_calc();

	/* if we have an index line, display it now */

	if (label_fmt == 3)
	{
		save_attr = SP->slk_winptr->_attrs;
		wattrset(SP->slk_winptr, A_NORMAL);
		wmove(SP->slk_winptr, 0, 0);
		whline(SP->slk_winptr, 0, COLS);

		for (i = 0; i < labels; i++)
			mvwprintw(SP->slk_winptr, 0, slk_start_col[i], 
				"F%d", i + 1);

		SP->slk_winptr->_attrs = save_attr;
	}

	touchwin(SP->slk_winptr);
}

static void _slk_calc(void)
{
	int i, center, col = 0;
	label_length = COLS / labels;

	if (label_length > 31)
		label_length = 31;

	switch (label_fmt)
	{
	case 0:     /* 3 - 2 - 3 F-Key layout */

		--label_length;

		slk_start_col[0] = col;
		slk_start_col[1] = (col += label_length);
		slk_start_col[2] = (col += label_length);

		center = COLS / 2;

		slk_start_col[3] = center - label_length + 1;
		slk_start_col[4] = center + 1;

		col = COLS - (label_length * 3) + 1;

		slk_start_col[5] = col;
		slk_start_col[6] = (col += label_length);
		slk_start_col[7] = (col += label_length);
		break;

	case 1:     /* 4 - 4 F-Key layout */

		for (i = 0; i < 8; i++)
		{
			slk_start_col[i] = col;
			col += label_length;

			if (i == 3)
				col = COLS - (label_length * 4) + 1; 
		}

		break;

	case 2:     /* 4 4 4 F-Key layout */
	case 3:     /* 4 4 4 F-Key layout with index */

		for (i = 0; i < 4; i++)
		{
			slk_start_col[i] = col;
			col += label_length;
		}

		center = COLS/2;

		slk_start_col[4] = center - (label_length * 2) + 1;
		slk_start_col[5] = center - label_length - 1;
		slk_start_col[6] = center + 1;
		slk_start_col[7] = center + label_length + 1;

		col = COLS - (label_length * 4) + 1;

		for (i = 8; i < 12; i++)
		{
			slk_start_col[i] = col;
			col += label_length;
		}

		break;

	default:    /* 5 - 5 F-Key layout */

		for (i = 0; i < 10; i++)
		{
			slk_start_col[i] = col;
			col += label_length;

			if (i == 4)
				col = COLS - (label_length * 5) + 1;
		}
	}

	--label_length;

	/* make sure labels are all in window */

	_redraw();
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
		 && x < slk_start_col[i] + label_length)
			return i + 1;

	return 0;
}

#ifdef PDC_WIDE
int slk_wset(int label_num, const wchar_t *label_str, int label_fmt)
{
	PDC_LOG(("slk_set() - called\n"));

	int i;

	if (label_num < 1 || label_num > labels ||
	    label_fmt < 0 || label_fmt > 2)
		return ERR;

	label_num--;

	/* A NULL in either the first byte or pointer
	   indicates a clearing of the label. */

	if (!label_str || !(*label_str)) 
	{
		/* Save the string and attribute */

		*slk_save[label_num].label = 0;
		slk_save[label_num].format = 0;
		slk_save[label_num].len = 0;
	}
	else
	{

		/* Otherwise, format the character and put in position */

		for (i = 0; i < label_length; i++)
		{
			chtype ch = label_str[i];

			slk_save[label_num].label[i] = ch;

			if (!ch)
				break;
		}

		/* Save the string and attribute */

		slk_save[label_num].label[label_length] = 0;
		slk_save[label_num].format = label_fmt;
		slk_save[label_num].len = i;
	}

	_drawone(label_num);

	return OK;
}
#endif
