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

RCSID("$Id: util.c,v 1.56 2006/12/07 18:39:00 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                          util

  Synopsis:
	char *unctrl(chtype c);
	void filter(void);
	void use_env(bool x);
	int delay_output(int ms);
	int getcchar(const cchar_t *wcval, wchar_t *wch, attr_t *attrs,
		     short *color_pair, void *opts);
	int setcchar(cchar_t *wcval, const wchar_t *wch, const attr_t attrs,
		     short color_pair, const void *opts);
	wchar_t *wunctrl(cchar_t *wc);

  X/Open Description:
	The unctrl() routine expands the character c into a character
	string which is a printable representation of the character.
	Control characters are displayed in the ^X notation.  Printing
	characters are displayed normally.

	The delay_output() function inserts ms millisecond pause in output.
	On some systems, this has no effect.

  PDCurses Description:
	The conversion from a control character to a two-character
	sequence is done by the unctrl() function. In the BSD version
	of curses it is done by a macro, which uses a publicly
	available translation table. Some ill-behaved application
	programs use the table directly, and since it does not exist
	in this curses version such application will link with an
	error message complaining about undefined symbols.

	filter() and use_env() are no-ops on PDCurses.

  X/Open Return Value:
	FIXME

  Portability				     X/Open    BSD    SYS V
	unctrl					Y	Y	Y
	filter					Y	-      3.0
	use_env					Y	-      4.0
	delay_output				Y	Y	Y
	getcchar				Y
	setcchar				Y
	wunctrl					Y

**man-end****************************************************************/

char *unctrl(chtype c)
{
	static char strbuf[3] = {0, 0, 0};

	chtype ic;

	PDC_LOG(("unctrl() - called\n"));

	ic = c & A_CHARTEXT;

	if (ic >= 0x20 && ic != 0x7f)		/* normal characters */
	{
		strbuf[0] = (char)ic;
		strbuf[1] = '\0';
		return strbuf;
	}

	strbuf[0] = '^';			/* '^' prefix */

	if (ic == 0x7f)				/* 0x7f == DEL */
		strbuf[1] = '?';
	else					/* other control */
		strbuf[1] = (char)(ic + '@');

	return strbuf;
}

bool has_key(int key)
{
	PDC_LOG(("has_key() - called: key %d\n",key));

	return (key >= KEY_MIN && key <= KEY_MAX);
}

void filter(void)
{
	PDC_LOG(("filter() - called\n"));
}

void use_env(bool x)
{
	PDC_LOG(("use_env() - called: x %d\n", x));
}

int delay_output(int ms)
{
	PDC_LOG(("delay_output() - called: ms %d\n", ms));

	return napms(ms);
}

#ifdef PDC_WIDE
int getcchar(const cchar_t *wcval, wchar_t *wch, attr_t *attrs,
	     short *color_pair, void *opts)
{
	if (!wcval)
		return ERR;

	if (wch)
	{
		if (!attrs || !color_pair)
			return ERR;

		*wch = (*wcval & A_CHARTEXT);
		*attrs = (*wcval & (A_ATTRIBUTES & ~A_COLOR));
		*color_pair = PAIR_NUMBER(*wcval & A_COLOR);

		if (*wch)
			*++wch = L'\0';

		return OK;
	}
	else
		return ((*wcval & A_CHARTEXT) != L'\0');
}

int setcchar(cchar_t *wcval, const wchar_t *wch, const attr_t attrs,
	     short color_pair, const void *opts)
{
	if (!wcval || !wch)
		return ERR;

	*wcval = *wch | attrs | COLOR_PAIR(color_pair);

	return OK;
}

wchar_t *wunctrl(cchar_t *wc)
{
	static wchar_t strbuf[3] = {0, 0, 0};

	cchar_t ic;

	PDC_LOG(("wunctrl() - called\n"));

	ic = *wc & A_CHARTEXT;

	if (ic >= 0x20 && ic != 0x7f)		/* normal characters */
	{
		strbuf[0] = (wchar_t)ic;
		strbuf[1] = L'\0';
		return strbuf;
	}

	strbuf[0] = '^';			/* '^' prefix */

	if (ic == 0x7f)				/* 0x7f == DEL */
		strbuf[1] = '?';
	else					/* other control */
		strbuf[1] = (wchar_t)(ic + '@');

	return strbuf;
}
#endif
