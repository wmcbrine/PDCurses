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
#include <limits.h>

RCSID("$Id: termattr.c,v 1.49 2007/04/13 04:28:52 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								termattr

  Synopsis:
	int baudrate(void);
	char erasechar(void);
	bool has_ic(void);
	bool has_il(void);
	char killchar(void);
	char *longname(void);
	chtype termattrs(void);
	attr_t term_attrs(void);
	char *termname(void);

	int erasewchar(wchar_t *ch);
	int killwchar(wchar_t *ch);

	char wordchar(void);

  Description:
	baudrate() is supposed to return the output speed of the 
	terminal. In PDCurses, it simply returns INT_MAX.

	has_ic and has_il() return TRUE. These functions have meaning in 
	some other implementations of curses.

	erasechar() and killchar() return ^H and ^U, respectively -- the 
	ERASE and KILL characters. In other curses implementations, 
	these may vary by terminal type. erasewchar() and killwchar() 
	are the wide-character versions; they take a pointer to a 
	location in which to store the character, and return OK or ERR.

	longname() returns a pointer to a static area containing a
	verbose description of the current terminal. The maximum length
	of the string is 128 characters.  It is defined only after the
	call to initscr() or newterm().

	termname() returns a pointer to a static area containing a
	short description of the current terminal (14 characters).

	termattrs() returns a logical OR of all video attributes
	supported by the terminal.

	wordchar() is a PDCurses extension of the concept behind the 
	functions erasechar() and killchar(), returning the "delete 
	word" character, ^W.

  Portability				     X/Open    BSD    SYS V
	baudrate				Y	Y	Y
	erasechar				Y	Y	Y
	has_ic					Y	Y	Y
	has_il					Y	Y	Y
	killchar				Y	Y	Y
	longname				Y	Y	Y
	termattrs				Y	Y	Y
	termname				Y	Y	Y
	erasewchar				Y
	killwchar				Y
	term_attrs				Y
	wordchar				-	-	-

**man-end****************************************************************/

int baudrate(void)
{
	PDC_LOG(("baudrate() - called\n"));

	return INT_MAX;
}

char erasechar(void)
{
	PDC_LOG(("erasechar() - called\n"));

	return _ECHAR;		/* character delete char (^H) */
}

bool has_ic(void)
{
	PDC_LOG(("has_ic() - called\n"));

	return TRUE;
}

bool has_il(void)
{
	PDC_LOG(("has_il() - called\n"));

	return TRUE;
}

char killchar(void)
{
	PDC_LOG(("killchar() - called\n"));

	return _DLCHAR;		/* line delete char (^U) */
}

char *longname(void)
{
	PDC_LOG(("longname() - called\n"));

	return ttytype + 9;	/* skip "pdcurses|" */
}

chtype termattrs(void)
{
	chtype temp = A_BLINK | A_BOLD | A_INVIS | A_REVERSE | A_UNDERLINE;

	/* note: blink is bold background on some platforms */

	PDC_LOG(("termattrs() - called\n"));

	if (!SP->mono)
		temp |= A_COLOR;

	return temp;
}

attr_t term_attrs(void)
{
	PDC_LOG(("term_attrs() - called\n"));

	return WA_BLINK | WA_BOLD | WA_INVIS | WA_LEFT |
		WA_REVERSE | WA_RIGHT | WA_UNDERLINE;
}

char *termname(void)
{
	PDC_LOG(("termname() - called\n"));

	return "pdcurses";
}

char wordchar(void)
{
	PDC_LOG(("wordchar() - called\n"));

	return _DWCHAR;			/* word delete char */
}

#ifdef PDC_WIDE
int erasewchar(wchar_t *ch)
{
	PDC_LOG(("erasewchar() - called\n"));

	if (!ch)
		return ERR;

	*ch = (wchar_t)_ECHAR;

	return OK;
}

int killwchar(wchar_t *ch)
{
	PDC_LOG(("killwchar() - called\n"));

	if (!ch)
		return ERR;

	*ch = (wchar_t)_DLCHAR;

	return OK;
}
#endif
