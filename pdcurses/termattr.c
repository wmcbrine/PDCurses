/**************************************************************************
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
**************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>
#include <limits.h>

/* undefine any macros for functions defined in this module */
#undef baudrate
#undef erasechar
#undef has_ic
#undef has_il
#undef killchar
#undef longname
#undef termattrs
#undef termname
#undef wordchar

#ifdef PDCDEBUG
const char *rcsid_termattr =
	"$Id: termattr.c,v 1.19 2006/02/07 20:16:52 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                     termattr

  Synopsis:
	int baudrate(void);
	char erasechar(void);
	bool has_ic(void);
	bool has_il(void);
	char killchar(void);
	char *longname(void);
	chtype termattrs(void);
	char *termname(void);

	char wordchar(void);

  X/Open Description:
	baudrate() returns the output speed of the terminal.  The number 
	returned is bits per second, for example 9600, and is an integer.

	The user's current ERASE character is returned from a call to
	erasechar();

	The has_ic function will return TRUE if the terminal has insert 
	character and delete character capabilities.

	The has_il() function will return TRUE if the terminal has insert
	line and delete line capabilities.

	The user's current KILL character is returned from a call to
	killchar();

	longname() returns a pointer to a static area containing a
	verbose description of the current terminal.  The maximum length
	of the string is 128 characters.  It is defined only after the
	call to initscr() or newterm().  The area is overwritten by each
	call to newterm() and is not restored by set_term().  The value
	should therefore be saved between calls to newterm(), if
	longname() is going to be used with multiple terminals.

	termname() returns a pointer to a static area containing a
	short description of the current terminal (14 characters).

	termattrs() returns a logical OR of all video attributes
	supported by the terminal.

  PDCurses Description:
	baudrate() returns the largest possible (portable) int value 
	(INT_MAX from limits.h) IF direct video is possible, OR the 
	approximate guess at BIOS speeds, 19200.

	erasechar(), killchar() and wordchar() all return values that 
	are hardcoded at this time.  There may be future development to 
	allow applications an easy way to specify these value.

	For DOS, has_ic() and has_il() always return TRUE, as the speed 
	of the DIRECT VIDEO or even BIOS writes is quicker than on a 
	serial terminal.

	In addition to the above definition for longname(), the form of 
	this string is the adapter name (or video card name) and the 
	text resolution. This may also be followed by the notation that 
	the video card may be a clone, which indicates that the card 
	identification maps to more than one unique card. e.g. The MDS 
	Genius and the Quadram QuadHPG identify themselves in the same 
	manner, but are vastly different in maximum resolution.

	The user's current WORD character is returned from a call to
	wordchar().

  X/Open Return Value:
	All functions return OK on success and ERR on error.

  X/Open Errors:
	No errors are defined for this function.

  Portability				     X/Open    BSD    SYS V
					     Dec '88
	baudrate				Y	Y	Y
	erasechar				Y	Y	Y
	has_ic					Y	Y	Y
	has_il					Y	Y	Y
	killchar				Y	Y	Y
	longname				Y	Y	Y
	termattrs				Y	Y	Y
	termname				Y	Y	Y
	wordchar				-	-	-

**man-end**********************************************************************/

int PDC_CDECL baudrate(void)
{
	PDC_LOG(("baudrate() - called\n"));

	return SP->direct_video ? INT_MAX : 19200;
}

char PDC_CDECL erasechar(void)
{
	PDC_LOG(("erasechar() - called\n"));

	return _ECHAR;		/* character delete char (^H) */
}

bool PDC_CDECL has_ic(void)
{
	PDC_LOG(("has_ic() - called\n"));

	return TRUE;
}

bool PDC_CDECL has_il(void)
{
	PDC_LOG(("has_il() - called\n"));

	return TRUE;
}

char PDC_CDECL killchar(void)
{
	PDC_LOG(("killchar() - called\n"));

	return _DLCHAR;		/* line delete char (^U) */
}

char * PDC_CDECL longname(void)
{
	static char _display[128];
	char *p = _display;

	PDC_LOG(("longname() - called\n"));

	p += sprintf(_display, "PDCurses for "
#if defined(DOS)
		"DOS "
#elif defined(OS2)
		"OS/2 "
#elif defined(WIN32)
		"Win32 "
#else
		"X11 "
#endif
		);

#if defined(OS2) && !defined(EMXVIDEO)
	switch (SP->adapter.adapter)
	{
	case DISPLAY_CGA:
		p += sprintf(p, "CGA");
		break;
	case DISPLAY_MONOCHROME:
		p += sprintf(p, "MDA");
		break;
	case DISPLAY_EGA:
		p += sprintf(p, "EGA");
		break;
	case DISPLAY_VGA:
		p += sprintf(p, "VGA");
		break;
	case DISPLAY_8514A:
		p += sprintf(p, "8514");
		break;
# ifdef DISPLAY_XGA
	case DISPLAY_XGA:
		p += sprintf(p, "XGA");
# endif
	}

#elif defined(DOS)
	switch (SP->adapter)
	{
	case _CGA:
		p += sprintf(p, "CGA");
		break;
	case _MDA:
		p += sprintf(p, "MDA");
		break;
	case _EGACOLOR:
	case _EGAMONO:
		p += sprintf(p, "EGA");
		break;
	case _VGACOLOR:
	case _VGAMONO:
		p += sprintf(p, "VGA");
		break;
	case _MCGACOLOR:
	case _MCGAMONO:
		p += sprintf(p, "MCGA");
		break;
	case _MDS_GENIUS:
		p += sprintf(p, "Genius");
		break;
	default:
		p += sprintf(p, "Unknown");
	}

	if (SP->bogus_adapter)
		p += sprintf(p, " (Clone)");
#endif

	sprintf(p, " %s-%dx%d", SP->mono ? "MONO" : "COLOR",
		LINES, COLS);

	return _display;
}

chtype PDC_CDECL termattrs(void)
{
	chtype temp = (A_NORMAL | A_BOLD | A_BLINK | A_REVERSE | 
		A_UNDERLINE | A_ITALIC);

	/* note: blink is bold background on some platforms */

	PDC_LOG(("termattrs() - called\n"));

	if (!SP->mono)
		temp |= A_COLOR;

	return temp;
}

char * PDC_CDECL termname(void)
{
	PDC_LOG(("termname() - called\n"));

	return "pdcurses";
}

char PDC_CDECL wordchar(void)
{
	PDC_LOG(("wordchar() - called\n"));

	return _DWCHAR;			/* word delete char */
}
