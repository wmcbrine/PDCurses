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

/* undefine any macros for functions defined in this module */
#undef unctrl
#undef keyname
#undef has_key
#undef filter
#undef use_env
#undef delay_output
#undef flushinp

RCSID("$Id: util.c,v 1.45 2006/07/14 03:21:16 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                          util

  Synopsis:
	char *unctrl(chtype c);
	char *keyname(int key);
	void filter(void);
	void use_env(bool x);
	int delay_output(int ms);
	int flushinp(void);

  X/Open Description:
	The unctrl() routine expands the character c into a character
	string which is a printable representation of the character.
	Control characters are displayed in the ^X notation.  Printing
	characters are displayed normally.

	The keyname() function returns a pointer to a character string 
	containing a symbolic name corresponding to that specified in 
	the argument key. key may be any key returned by wgetch().

	The delay_output() function inserts ms millisecond pause in output.
	On some systems, this has no effect.

	The flushinp() routine throws away any type-ahead that has been 
	typed by the user and has not yet been read by the program.

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
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	unctrl					Y	Y	Y
	keyname					Y	-      3.0
	filter					Y	-      3.0
	use_env					-	-      4.0
	delay_output				Y	Y	Y
	flushinp				Y	Y	Y

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

char *keyname(int key)
{
	/* Key names must be in exactly the same order as in curses.h */

	static char *key_name[] = {
		"KEY_BREAK", "KEY_DOWN", "KEY_UP", "KEY_LEFT", 
		"KEY_RIGHT", "KEY_HOME", "KEY_BACKSPACE", "KEY_F0", 
		"KEY_F(1)", "KEY_F(2)", "KEY_F(3)", "KEY_F(4)", 
		"KEY_F(5)", "KEY_F(6)", "KEY_F(7)", "KEY_F(8)", 
		"KEY_F(9)", "KEY_F(10)", "KEY_F(11)", "KEY_F(12)", 
		"KEY_F(13)", "KEY_F(14)", "KEY_F(15)", "KEY_F(16)", 
		"KEY_F(17)", "KEY_F(18)", "KEY_F(19)", "KEY_F(20)", 
		"KEY_F(21)", "KEY_F(22)", "KEY_F(23)", "KEY_F(24)", 
		"KEY_F(25)", "KEY_F(26)", "KEY_F(27)", "KEY_F(28)", 
		"KEY_F(29)", "KEY_F(30)", "KEY_F(31)", "KEY_F(32)", 
		"KEY_F(33)", "KEY_F(34)", "KEY_F(35)", "KEY_F(36)", 
		"KEY_F(37)", "KEY_F(38)", "KEY_F(39)", "KEY_F(40)", 
		"KEY_F(41)", "KEY_F(42)", "KEY_F(43)", "KEY_F(44)", 
		"KEY_F(45)", "KEY_F(46)", "KEY_F(47)", "KEY_F(48)", 
		"KEY_F(49)", "KEY_F(50)", "KEY_F(51)", "KEY_F(52)", 
		"KEY_F(53)", "KEY_F(54)", "KEY_F(55)", "KEY_F(56)", 
		"KEY_F(57)", "KEY_F(58)", "KEY_F(59)", "KEY_F(60)", 
		"KEY_F(61)", "KEY_F(62)", "KEY_F(63)", "KEY_DL", 
		"KEY_IL", "KEY_DC", "KEY_IC", "KEY_EIC", "KEY_CLEAR", 
		"KEY_EOS", "KEY_EOL", "KEY_SF", "KEY_SR", "KEY_NPAGE", 
		"KEY_PPAGE", "KEY_STAB", "KEY_CTAB", "KEY_CATAB", 
		"KEY_ENTER", "KEY_SRESET", "KEY_RESET", "KEY_PRINT", 
		"KEY_LL", "KEY_ABORT", "KEY_SHELP", "KEY_LHELP", 
		"KEY_BTAB", "KEY_BEG", "KEY_CANCEL", "KEY_CLOSE", 
		"KEY_COMMAND", "KEY_COPY", "KEY_CREATE", "KEY_END", 
		"KEY_EXIT", "KEY_FIND", "KEY_HELP", "KEY_MARK", 
		"KEY_MESSAGE", "KEY_MOVE", "KEY_NEXT", "KEY_OPEN", 
		"KEY_OPTIONS", "KEY_PREVIOUS", "KEY_REDO", 
		"KEY_REFERENCE", "KEY_REFRESH", "KEY_REPLACE", 
		"KEY_RESTART", "KEY_RESUME", "KEY_SAVE", "KEY_SBEG", 
		"KEY_SCANCEL", "KEY_SCOMMAND", "KEY_SCOPY", 
		"KEY_SCREATE", "KEY_SDC", "KEY_SDL", "KEY_SELECT", 
		"KEY_SEND", "KEY_SEOL", "KEY_SEXIT", "KEY_SFIND", 
		"KEY_SHOME", "KEY_SIC", "UNKNOWN KEY", "KEY_SLEFT", 
		"KEY_SMESSAGE", "KEY_SMOVE", "KEY_SNEXT", 
		"KEY_SOPTIONS", "KEY_SPREVIOUS", "KEY_SPRINT", 
		"KEY_SREDO", "KEY_SREPLACE", "KEY_SRIGHT", "KEY_SRSUME", 
		"KEY_SSAVE", "KEY_SSUSPEND", "KEY_SUNDO", "KEY_SUSPEND", 
		"KEY_UNDO", "ALT_0", "ALT_1", "ALT_2", "ALT_3", "ALT_4", 
		"ALT_5", "ALT_6", "ALT_7", "ALT_8", "ALT_9", "ALT_A", 
		"ALT_B", "ALT_C", "ALT_D", "ALT_E", "ALT_F", "ALT_G", 
		"ALT_H", "ALT_I", "ALT_J", "ALT_K", "ALT_L", "ALT_M", 
		"ALT_N", "ALT_O", "ALT_P", "ALT_Q", "ALT_R", "ALT_S", 
		"ALT_T", "ALT_U", "ALT_V", "ALT_W", "ALT_X", "ALT_Y", 
		"ALT_Z", "CTL_LEFT", "CTL_RIGHT", "CTL_PGUP", 
		"CTL_PGDN", "CTL_HOME", "CTL_END", "KEY_A1", "KEY_A2", 
		"KEY_A3", "KEY_B1", "KEY_B2", "KEY_B3", "KEY_C1", 
		"KEY_C2", "KEY_C3", "PADSLASH", "PADENTER", 
		"CTL_PADENTER", "ALT_PADENTER", "PADSTOP", "PADSTAR", 
		"PADMINUS", "PADPLUS", "CTL_PADSTOP", "CTL_PADCENTER", 
		"CTL_PADPLUS", "CTL_PADMINUS", "CTL_PADSLASH", 
		"CTL_PADSTAR", "ALT_PADPLUS", "ALT_PADMINUS", 
		"ALT_PADSLASH", "ALT_PADSTAR", "ALT_PADSTOP", "CTL_INS", 
		"ALT_DEL", "ALT_INS", "CTL_UP", "CTL_DOWN", "CTL_TAB", 
		"ALT_TAB", "ALT_MINUS", "ALT_EQUAL", "ALT_HOME", 
		"ALT_PGUP", "ALT_PGDN", "ALT_END", "ALT_UP", "ALT_DOWN", 
		"ALT_RIGHT", "ALT_LEFT", "ALT_ENTER", "ALT_ESC", 
		"ALT_BQUOTE", "ALT_LBRACKET", "ALT_RBRACKET", 
		"ALT_SEMICOLON", "ALT_FQUOTE", "ALT_COMMA", "ALT_STOP", 
		"ALT_FSLASH", "ALT_BKSP", "CTL_BKSP", "PAD0", 
		"CTL_PAD0", "CTL_PAD1", "CTL_PAD2", "CTL_PAD3", 
		"CTL_PAD4", "CTL_PAD5", "CTL_PAD6", "CTL_PAD7", 
		"CTL_PAD8", "CTL_PAD9", "ALT_PAD0", "ALT_PAD1", 
		"ALT_PAD2", "ALT_PAD3", "ALT_PAD4", "ALT_PAD5", 
		"ALT_PAD6", "ALT_PAD7", "ALT_PAD8", "ALT_PAD9", 
		"CTL_DEL", "ALT_BSLASH", "CTL_ENTER", "SHF_PADENTER", 
		"SHF_PADSLASH", "SHF_PADSTAR", "SHF_PADPLUS", 
		"SHF_PADMINUS", "SHF_UP", "SHF_DOWN", "SHF_IC", 
		"SHF_DC", "KEY_MOUSE", "KEY_SHIFT_L", "KEY_SHIFT_R", 
		"KEY_CONTROL_L", "KEY_CONTROL_R", "KEY_ALT_L", 
		"KEY_ALT_R", "KEY_RESIZE", "KEY_SUP", "KEY_SDOWN"
	};

	PDC_LOG(("keyname() - called: key %d\n", key));

	if ((key >= 0) && (key < 0x80))
		return unctrl((chtype)key);

	return has_key(key) ? key_name[key - KEY_MIN] : "UNKNOWN KEY";
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

int flushinp(void)
{
	PDC_LOG(("flushinp() - called\n"));

	PDC_flushinp();

	c_gindex = 1;			/* set indices to kill buffer	 */
	c_pindex = 0;
	c_ungind = 0;			/* clear c_ungch array		 */

	return OK;
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

		if (*wch != L'\0')
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
		strbuf[1] = '\0';
		return strbuf;
	}

	strbuf[0] = '^';			/* '^' prefix */

	if (ic == 0x7f)				/* 0x7f == DEL */
		strbuf[1] = '?';
	else					/* other control */
		strbuf[1] = (wchar_t)(ic + '@');

	return strbuf;
}

char *key_name(wchar_t c)
{
	PDC_LOG(("key_name() - called\n"));

	return keyname((int)c);
}
#endif

#undef traceon
#undef traceoff

void traceon(void)
{
	PDC_LOG(("traceon() - called\n"));

	trace_on = TRUE;
}

void traceoff(void)
{
	PDC_LOG(("traceoff() - called\n"));

	trace_on = FALSE;
}
