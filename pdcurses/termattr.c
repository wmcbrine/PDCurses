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
#  include <config.h>
#endif
#include <curses.h>

#ifdef UNIX
#include <defs.h>
#include <term.h>
#endif

#include <stdio.h>
#include <string.h>
#include <limits.h>

/* undefine any macros for functions defined in this module */
#undef	baudrate
#undef	erasechar
#undef	has_ic
#undef	has_il
#undef	killchar
#undef	longname
#undef	termattrs
#undef	termname
#undef	wordchar

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_termattr  = "$Id: termattr.c,v 1.2 2002/11/27 11:25:18 mark Exp $";
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
  	attr_t termattrs(void);
  	char *termname(void);

  	char	wordchar(void);

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
 	If FAST_VIDEO is true, then this is the largest possible
 	(portable) int value (INT_MAX from limits.h) IF direct
 	video is possible, OR the approximate guess at BIOS speeds,
 	19200.
 	If FAST_VIDEO is false, this is an approximate guess at BIOS
 	speeds, 19200.

 	erasechar(), killchar() and wordchar() all return values that are 
 	hardcoded at this time.  There may be future development
 	to allow applications an easy way to specify these value.

 	For DOS, has_ic() and has_il() always return TRUE, as the speed of
 	the DIRECT VIDEO or even BIOS writes is quicker than on a
 	serial terminal.
 	For FLEXOS, this value has meaning and depends on the terminal,
 	though at this time, it is a constant TRUE.

 	In addition to the above definition for longname(), the form of 
 	this string is the adapter name (or video card name) and the text 
 	resolution. This may also be followed by the notation that the video 
 	card may be a clone, which indicates that the card identification
 	maps to more than one unique card.
 	e.g. The MDS Genius and the Quadram QuadHPG identify themselves
 	in the same manner, but are vastly different in maximum resolution.

 	The user's current WORD character is returned from a call to
 	wordchar();

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      baudrate                              Y        Y       Y
      erasechar                             Y        Y       Y
      has_ic                                Y        Y       Y
      has_il                                Y        Y       Y
      killchar                              Y        Y       Y
      longname                              Y        Y       Y
      termattrs                             Y        Y       Y
      termname                              Y        Y       Y
      wordchar                              -        -       -

**man-end**********************************************************************/

#ifdef	FLEXOS
extern	char*	_flexos_gname();
#endif
static	char	_display[ 128 ];
static	char	_shrtnme[ 14 ];

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	baudrate(void)
#else
int	PDC_CDECL	baudrate()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("baudrate() - called\n");
#endif

#ifdef UNIX
	return (_CUR_TERM.baudrate);
#else
#ifdef FAST_VIDEO
	return (SP->direct_video ? INT_MAX : 19200 );
#else
	return (19200);			/* Approx. guess at BIOS speeds.*/
#endif
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
char	PDC_CDECL	erasechar(void)
#else
char	PDC_CDECL	erasechar()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("erasechar() - called\n");
#endif

#ifdef UNIX
#ifdef USE_TERMIO
	ioctl(_CUR_TERM.fd, TCGETA, &_CUR_TERM.prog_mode);
	return(_CUR_TERM.prog_mode.c_cc[VERASE]);
#else
	ioctl(_CUR_TERM.fd, TIOCGETP, &_CUR_TERM.prog_mode.v6);
	return(_CUR_TERM.prog_mode.v6.sg_erase);
#endif
#else
	return(	_ECHAR );	/* character delete char (^H) */
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_CDECL	has_ic(void)
#else
bool	PDC_CDECL	has_ic()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("has_ic() - called\n");
#endif

#ifdef UNIX
	if (insert_character != NULL && delete_character != NULL)
		return(TRUE);
	else
		return(FALSE);
#else
	return( TRUE );
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_CDECL	has_il(void)
#else
bool	PDC_CDECL	has_il()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("has_il() - called\n");
#endif

#ifdef UNIX
	if (insert_line != NULL && delete_line != NULL)
		return(TRUE);
	else
		return(FALSE);
#else
	return( TRUE );
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
char	PDC_CDECL	killchar(void)
#else
char	PDC_CDECL	killchar()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("killchar() - called\n");
#endif

#ifdef UNIX
#ifdef USE_TERMIO
	ioctl(_CUR_TERM.fd, TCGETA, &_CUR_TERM.prog_mode);
	return(_CUR_TERM.prog_mode.c_cc[VKILL]);
#else
	ioctl(_CUR_TERM.fd, TIOCGETP, &_CUR_TERM.prog_mode.v6);
	return(_CUR_TERM.prog_mode.v6.sg_kill);
#endif
#else
	return( _DLCHAR );	/* line delete char (^U) */
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
char*	PDC_CDECL	longname(void)
#else
char*	PDC_CDECL	longname()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("longname() - called\n");
#endif

#if defined (XCURSES)
	if (SP->mono)
		sprintf(_display,"X11-MONO-%dx%d",LINES,COLS);
	else
		sprintf(_display,"X11-COLOR-%dx%d",LINES,COLS);
#endif

#ifdef OS2
# ifdef EMXVIDEO
	switch (SP->mono)
	{
	case FALSE:	sprintf(_display,"COLOR-%dx%d", LINES, COLS); break;
	case TRUE:	sprintf(_display,"MONO-%dx%d", LINES, COLS); break;
# else
	switch	(SP->adapter.adapter)
	{
	case DISPLAY_CGA:	sprintf(_display, "CGA-%dx%d", LINES, COLS);	  break;
	case DISPLAY_MONOCHROME:	sprintf(_display, "MDA-%dx%d", LINES, COLS);	  break;
	case DISPLAY_EGA:	sprintf(_display, "EGA-%dx%d", LINES, COLS); break;
	case DISPLAY_VGA:	sprintf(_display, "VGA-%dx%d", LINES, COLS); break;
	case DISPLAY_8514A:     sprintf(_display, "8514-%dx%d", LINES, COLS);  break;
# endif
	default:	sprintf(_display, "Unknown-%dx%d", LINES, COLS);  break;
	}
#endif

#ifdef DOS
	switch	(SP->adapter)
	{
	case _CGA:	sprintf(_display, "CGA-%dx%d", LINES, COLS);	  break;
	case _MDA:	sprintf(_display, "MDA-%dx%d", LINES, COLS);	  break;
	case _EGACOLOR:	sprintf(_display, "EGAColor-%dx%d", LINES, COLS); break;
	case _EGAMONO:	sprintf(_display, "EGAMono-%dx%d", LINES, COLS);  break;
	case _VGACOLOR:	sprintf(_display, "VGAColor-%dx%d", LINES, COLS); break;
	case _VGAMONO:	sprintf(_display, "VGAMono-%dx%d", LINES, COLS);  break;
	case _MCGACOLOR:sprintf(_display, "MCGAColor-%dx%d", LINES, COLS);break;
	case _MCGAMONO:	sprintf(_display, "MCGAMono-%dx%d", LINES, COLS); break;
	case _MDS_GENIUS:sprintf(_display, "Genius-%dx%d", LINES, COLS);  break;
	default:	sprintf(_display, "Unknown-%dx%d", LINES, COLS);  break;
	}
#endif

#ifdef	FLEXOS
	switch	(SP->adapter)
	{
	case _FLEXOS:	sprintf(_display, "%s", _cursesgname());	  break;
	default:	sprintf(_display, "Unknown-%dx%d", LINES, COLS);  break;
	}
#endif


	if (SP->bogus_adapter)
		strcat(_display, " (Clone)");

	return (_display);
}
/***********************************************************************/
#ifdef HAVE_PROTO
attr_t	PDC_CDECL	termattrs(void)
#else
attr_t	PDC_CDECL	termattrs()
#endif
/***********************************************************************/
{
	attr_t temp =   (A_NORMAL | A_BOLD | A_BLINK | A_REVERSE); /* blink is bold background on some platforms */

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("termattrs() - called\n");
#endif

	if( ! SP->mono )
		temp |= A_COLOR;

	return( temp );
}
/***********************************************************************/
#ifdef HAVE_PROTO
char*	PDC_CDECL	termname(void)
#else
char*	PDC_CDECL	termname()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("termname() - called\n");
#endif

#if defined (XCURSES)
	if (SP->mono)
		sprintf(_shrtnme,"X-MONO");
	else
		sprintf(_shrtnme,"X-COLOR");
#endif

#ifdef     OS2
# ifdef	EMXVIDEO
	switch (SP->mono)
	{
	case FALSE:	sprintf(_shrtnme,"COLOR"); break;
	case TRUE:	sprintf(_shrtnme,"MONO"); break;
# else
	switch	(SP->adapter.adapter)
	{
	case DISPLAY_CGA:	sprintf(_shrtnme, "CGA");	  break;
	case DISPLAY_MONOCHROME:	sprintf(_shrtnme, "MDA");	  break;
	case DISPLAY_EGA:	sprintf(_shrtnme, "EGA"); break;
	case DISPLAY_VGA:	sprintf(_shrtnme, "VGA"); break;
	case DISPLAY_8514A:     sprintf(_shrtnme, "8514");  break;
# endif
	default:	sprintf(_shrtnme, "Unknown");  break;
	}
#endif

#ifdef DOS
	switch	(SP->adapter)
	{
	case _CGA:	sprintf(_shrtnme, "CGA");	  break;
	case _MDA:	sprintf(_shrtnme, "MDA");	  break;
	case _EGACOLOR:	sprintf(_shrtnme, "EGAColor"); break;
	case _EGAMONO:	sprintf(_shrtnme, "EGAMono");  break;
	case _VGACOLOR:	sprintf(_shrtnme, "VGAColor"); break;
	case _VGAMONO:	sprintf(_shrtnme, "VGAMono");  break;
	case _MCGACOLOR:sprintf(_shrtnme, "MCGAColor");break;
	case _MCGAMONO:	sprintf(_shrtnme, "MCGAMono"); break;
	case _MDS_GENIUS:sprintf(_shrtnme, "Genius");  break;
	default:	sprintf(_shrtnme, "Unknown");  break;
	}
#endif

#ifdef	FLEXOS
	switch	(SP->adapter)
	{
	case _FLEXOS:	sprintf(_shrtnme, "%s", _cursesgname());	  break;
	default:	sprintf(_shrtnme, "Unknown");  break;
	}
#endif

	return (_shrtnme);
}
/***********************************************************************/
#ifdef HAVE_PROTO
char	PDC_CDECL	wordchar(void)
#else
char	PDC_CDECL	wordchar()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("wordchar() - called\n");
#endif

#ifdef UNIX
#ifdef USE_TERMIO
	ioctl(_CUR_TERM.fd, TCGETA, &_CUR_TERM.prog_mode);
	return(_CUR_TERM.prog_mode.c_cc[VWERASE]);
#else
#ifdef TIOCGLTC
	ioctl(_CUR_TERM.fd, TIOCGLTC, &_CUR_TERM.prog_mode.bsd_new);
	return(_CUR_TERM.prog_mode.bsd_new.t_werase);
#else
	return(0);
#endif
#endif
#else
	return (_DWCHAR);			/* word delete char */
#endif
}
