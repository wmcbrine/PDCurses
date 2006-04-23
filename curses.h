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

/* $Id: curses.h,v 1.166 2006/04/23 01:35:44 wmcbrine Exp $ */

/*----------------------------------------------------------------------*
 *				PDCurses				*
 *----------------------------------------------------------------------*/

#ifndef __PDCURSES__
#define	__PDCURSES__ 1

/*man-start**************************************************************

PDCurses definitions list:  (Only define those needed)

	DOS		True if compiling for DOS.
	OS2		True if compiling for OS/2.
	WIN32		True if compiling for Windows.
	XCURSES		True if compiling for X11.
	MSC		True if using a Microsoft compiler.
	NOMACROS	Don't use (most) macros in place of functions.

PDCurses portable platform definitions list:

	PDC_BUILD	Defines API build version.
	PDCURSES	Enables access to PDCurses-only routines.
	XOPEN		Always true.
	SYSVcurses	True if you are compiling for SYSV portability.
	BSDcurses	True if you are compiling for BSD portability.

**man-end****************************************************************/

#define PDC_BUILD 2803
#define	PDCURSES	1	/* PDCurses-only routines	*/
#define	XOPEN		1	/* X/Open Curses routines	*/
#define	SYSVcurses	1	/* System V Curses routines	*/
#define	BSDcurses	1	/* BSD Curses routines		*/
#define	CHTYPE_LONG	1	/* size of chtype; long		*/

/*----------------------------------------
 *	BORLAND COMPILERS	Turbo C[++], Borland C[++]
 *
 *	Borland definitions:
 *		DOS
 *		OS2
 *
 *		__TURBOC__, __MSDOS__,
 *		__OS2__ and __WIN32__
 *		are predefined by compiler.
 */

#ifdef __TURBOC__		/* Borland gives defines this as a value*/
#  ifdef __MSDOS__
#    define DOS 6		/* Major release of DOS supported	*/
#  endif
#  ifdef __OS2__
#    define OS2 3		/* Major release of OS/2 supported	*/
#  endif
#  ifdef __WIN32__
#    ifndef WIN32
#      define WIN32 1
#    endif
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF			/* have vsscanf() */
#  endif
#endif


/*----------------------------------------
 *	METAWARE COMPILERS	High C
 *
 *	Metaware definitions:
 *		DOS
 */

#ifdef __HIGHC__
#  pragma off(prototype_override_warnings)
#  ifdef __MSDOS__
#    define DOS 6		/* Major release of DOS supported	*/
#    include <bios.h>
#  endif
#endif

/*----------------------------------------
 *	MICROSOFT COMPILERS	MSC
 *
 *	Microsoft definitions:
 *		MSC
 *		DOS || OS2
 */

#ifdef _MSC_VER					/* defined by compiler	*/
#  define MSC 1
#  ifdef __OS2__		/* You will have to define in makefile	*/
#    define USE_OS2_H 1		/* Use the os2.h for the compiler	*/
#    define OS2 3		/* Major release of OS/2 supported	*/
#    define FARKeyword far
#    define APIRET USHORT
#  else				/* no __OS2__ define, so assume DOS	*/
#    ifdef _WIN32	
#      ifndef WIN32
#        define WIN32
#      endif
#    else	
#      define DOS 6		/* Major release of DOS supported	*/
#      include <bios.h>
#    endif
#  endif
#endif

/*----------------------------------------
 *	MICROSOFT QUICK C COMPILERS	QC
 *
 */

#ifdef _QC					/* defined by compiler	*/
#  define MSC 1
#  define DOS 6			/* Major release of DOS supported	*/
#  include <bios.h>
#endif

/*----------------------------------------
 *	TOPSPEED compilers	TSC
 *
 *	TOPSPEED definitions:
 *		DOS || OS2
 */

#ifdef __TSC__			/* You may have to define in makefile	*/
#  ifdef __OS2__
#    define OS2 3		/* Major release of OS/2 supported	*/
#  endif
#endif

/*----------------------------------------
 *	IBM C Set/2 Compiler	CSET2
 *
 *	IBM definitions:
 *		OS2
 */

#ifdef __IBMC__
#  ifdef __OS2__
#    define OS2 3		/* Major release of OS/2 supported	*/
#  endif
#endif

/*----------------------------------------
 *	GNU compilers		emx
 *
 *	emx definitions:
 *		OS2
 */

#ifdef __EMX__			/* You may have to define in makefile	*/
#  ifndef __OS2__
#    define __OS2__		/* EMX does not define this :-( 	*/
#  endif
#  define OS2 3			/* Major release of OS/2 supported	*/
#  define CURSES__32BIT__
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H			/* have <unistd.h> */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF			/* have vsscanf() */
#  endif
#  ifdef EMXVIDEO
#    include <stdlib.h>
#    include <sys/video.h>
#  endif
#endif

/*----------------------------------------
 *	GNU compilers		djgpp
 *
 *	djgpp definitions:
 *		DOS
 */

#ifdef __DJGPP__		/* You may have to define in makefile	*/
#  define DOS 6
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H			/* have <unistd.h> */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF			/* have vsscanf() */
#  endif
#  ifndef HAVE_USLEEP
#    define HAVE_USLEEP				/* have usleep() */
#  endif
#endif


/*----------------------------------------
 *	GNU compilers	Cygnus Win32
 *
 *	cygnus definitions:
 *		WIN32
 */

#ifdef __CYGWIN32__		/* You may have to define in makefile	*/
#  if !defined(WIN32) && !defined(XCURSES)
#    define WIN32
#  endif
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H			/* have <unistd.h> */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF			/* have vsscanf() */
#  endif
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF
#  endif
#endif

/*----------------------------------------
 *	GNU compilers	Ming Win32
 *
 *	Ming definitions:
 *		WIN32
 */

#ifdef __MINGW32__
#  ifndef WIN32
#    define WIN32
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF			/* have vsscanf() */
#  endif
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF
#  endif
#endif

/*----------------------------------------
 *	LCC WIN32
 *
 */

#ifdef __LCC__		/* should already be defined by the compiler	*/
#  ifndef WIN32
#    define WIN32
#  endif
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF
#  endif
#endif

/*----------------------------------------
 *	Watcom C/C++ 10.6 compiler
 *
 *	WATCOM definitions:
 *		OS2
 *		WIN32
 */

#ifdef __WATCOMC__
#  if defined(__DOS__) || defined(__DOS4G__)
#    define DOS 7		/* Major release of DOS supported	*/
#    ifdef __386__
#      define int86 int386
#      define int86x int386x
#    endif
#  endif
#  if defined(__OS2__) || defined(__OS2V2__)
#    define OS2 3		/* Major release of OS/2 supported	*/
#    define CURSES__32BIT__
#  endif
#  if defined(__NT__) || defined(WIN32)
#    ifndef WIN32
#      define WIN32
#    endif
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF			/* have vsscanf() */
#  endif
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF
#  endif
#endif

/*----------------------------------------
 *	MicroWay NDP C/C++ 386 4.2.0 compiler
 */

#ifdef MX386
#  include <bios.h>
#  ifdef DOS
#    define MK_FP(seg,ofs)	( (((int) (seg)) << 4) + ((int) (ofs)) )
#    ifdef __i860
typedef void _int;
#    else
typedef int  _int;
#    endif
#  endif
#endif

/*----------------------------------------
 *	HI-TECH COMPILERS Pacific C
 *
 *	Hi-Tech definitions:
 *
 *		__PACIFIC__, DOS
 *		are predefined by compiler.
 */

#ifdef __PACIFIC__
#  include <ctype.h>
#  ifndef __SMALL__
#    define __SMALL__
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF
#  endif
#endif

/*----------------------------------------------------------------------*/

#if defined(WIN32) && defined(INCLUDE_WINDOWS_H)
# include <windows.h>
# ifdef MOUSE_MOVED
#  undef MOUSE_MOVED
# endif
#endif

#if defined(OS2) && !defined(EMXVIDEO)
# define INCL_VIO
# define INCL_KBD
# include <os2.h>
#endif

/* If it's not DOS, or OS/2, or Windows, it must be XCurses */

#if !defined(DOS) && !defined(OS2) && !defined(WIN32)
# ifndef XCURSES
#  define XCURSES
# endif
#endif

#include <stdarg.h>
#include <stdio.h>		/* Required by X/Open usage below	*/

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
extern "C"
{
# define bool _bool
#endif

/*----------------------------------------------------------------------
 *
 *	PDCurses Manifest Constants
 *
 */

#ifndef FALSE			/* booleans		 */
#  define	FALSE	0
#endif
#ifndef	TRUE			/* booleans		 */
#  define	TRUE	1
#endif
#ifndef	NULL
#  define	NULL	(void*)0	/* Null pointer		 */
#endif
#ifndef	ERR
#  define	ERR	(-1)		/* general error flag	 */
#endif
#ifndef	OK
#  define	OK	0		/* general OK flag	 */
#endif


/*----------------------------------------------------------------------
 *
 *	PDCurses Type Declarations
 *
 */

typedef unsigned char bool;	/* PDCurses Boolean type	*/

#ifdef CHTYPE_LONG
# if _LP64
typedef unsigned int chtype;
# else
typedef unsigned long chtype;	/* 16-bit attr + 16-bit char	*/
# endif
#else
typedef unsigned short chtype;	/* 8-bit attr + 8-bit char	*/
#endif

/*----------------------------------------------------------------------
 * This defines a new type for attributes.
 *
 */

typedef chtype attr_t;

/*----------------------------------------------------------------------
 * Define our mouse interface - same as SYSVR4 (with extensions)
 */

typedef struct
{
	int x;           /* absolute column, 0 based, measured in characters */
	int y;           /* absolute row, 0 based, measured in characters */
	short button[3]; /* state of each button */
	int changes;     /* flags indicating what has changed with the mouse */
} MOUSE_STATUS;


#define BUTTON_RELEASED		0000
#define BUTTON_PRESSED		0001
#define BUTTON_CLICKED		0002
#define BUTTON_DOUBLE_CLICKED	0003
#define BUTTON_TRIPLE_CLICKED	0004
#define BUTTON_MOVED		0005	/* PDCurses enhancement */
#define WHEEL_SCROLLED		0006	/* PDCurses enhancement */
#define BUTTON_ACTION_MASK	0007	/* PDCurses enhancement */
#define BUTTON_SHIFT		0010	/* PDCurses enhancement */
#define BUTTON_CONTROL		0020	/* PDCurses enhancement */
#define BUTTON_ALT		0040	/* PDCurses enhancement */
#define BUTTON_MODIFIER_MASK	0070	/* PDCurses enhancement */

#define MOUSE_X_POS		(Mouse_status.x)
#define MOUSE_Y_POS		(Mouse_status.y)
/*
 * Bits associated with the .changes field:
 *   3         2         1         0
 * 210987654321098765432109876543210
 *                                 1 <- button 1 has changed
 *                                10 <- button 2 has changed
 *                               100 <- button 3 has changed
 *                              1000 <- mouse has moved
 *                             10000 <- mouse position report
 *                            100000 <- mouse wheel up
 *                           1000000 <- mouse wheel down
 */
#define PDC_MOUSE_MOVED		 8
#define PDC_MOUSE_POSITION	16
#define PDC_MOUSE_WHEEL_UP	32
#define PDC_MOUSE_WHEEL_DOWN	64
#define A_BUTTON_CHANGED	(Mouse_status.changes & 7)
#define MOUSE_MOVED		(Mouse_status.changes & PDC_MOUSE_MOVED)
#define MOUSE_POS_REPORT	(Mouse_status.changes & PDC_MOUSE_POSITION)
#define BUTTON_CHANGED(x)	(Mouse_status.changes & (1 << ((x) - 1)))
#define BUTTON_STATUS(x)	(Mouse_status.button[(x) - 1])
#define MOUSE_WHEEL_UP		(Mouse_status.changes & PDC_MOUSE_WHEEL_UP)
#define MOUSE_WHEEL_DOWN	(Mouse_status.changes & PDC_MOUSE_WHEEL_DOWN)

/* mouse bit-masks */
#define BUTTON1_RELEASED	000000000001L
#define BUTTON1_PRESSED		000000000002L
#define BUTTON1_CLICKED		000000000004L
#define BUTTON1_DOUBLE_CLICKED	000000000010L
#define BUTTON1_TRIPLE_CLICKED	000000000020L
#define BUTTON1_MOVED		000000000020L /* PDCurses enhancement */
#define BUTTON2_RELEASED	000000000040L
#define BUTTON2_PRESSED		000000000100L
#define BUTTON2_CLICKED		000000000200L
#define BUTTON2_DOUBLE_CLICKED	000000000400L
#define BUTTON2_TRIPLE_CLICKED	000000001000L
#define BUTTON2_MOVED		000000001000L /* PDCurses enhancement */
#define BUTTON3_RELEASED	000000002000L
#define BUTTON3_PRESSED		000000004000L
#define BUTTON3_CLICKED		000000010000L
#define BUTTON3_DOUBLE_CLICKED	000000020000L
#define BUTTON3_TRIPLE_CLICKED	000000040000L
#define BUTTON3_MOVED		000000040000L /* PDCurses enhancement */
#define MOUSE_WHEEL_SCROLL	000000100000L /* PDCurses enhancement */
#define ALL_MOUSE_EVENTS	000000777777L
#define BUTTON_MODIFIER_SHIFT	000001000000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_CONTROL 000002000000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_ALT	000004000000L /* PDCurses enhancement */
#define REPORT_MOUSE_POSITION	000010000000L

/*----------------------------------------------------------------------
 *
 *	PDCurses Structure Definitions:
 *
 */

typedef struct _win		/* definition of a window	*/
{
	int	_cury;		/* current pseudo-cursor	*/
	int	_curx;
	int	_maxy;		/* max window coordinates	*/
	int	_maxx;
	int	_pmaxy;		/* max physical size		*/
	int	_pmaxx;
	int	_begy;		/* origin on screen		*/
	int	_begx;
	int	_lastpy;	/* last y coordinate of upper left
				   pad display area */
	int	_lastpx;	/* last x coordinate of upper left
				   pad display area */
	int	_lastsy1;	/* last upper y coordinate of screen
				   window for pad */
	int	_lastsx1;	/* last upper x coordinate of screen
				   window for pad */
	int	_lastsy2;	/* last lower y coordinate of screen
				   window for pad */
	int	_lastsx2;	/* last lower x coordinate of screen
				   window for pad */
	int	_flags;		/* window properties		*/
	chtype	_attrs;		/* standard attributes and colors */
	chtype	_bkgd;		/* background, normally blank	*/
	int	_tabsize;	/* tab character size		*/
	bool	_clear;		/* causes clear at next refresh	*/
	bool	_leaveit;	/* leaves cursor where it is	*/
	bool	_scroll;	/* allows window scrolling	*/
	bool	_nodelay;	/* input character wait flag	*/
	bool	_immed;		/* immediate update flag	*/
	bool	_sync;		/* synchronise window ancestors	*/
	bool	_use_keypad;	/* flags keypad key mode active	*/
	chtype	**_y;		/* pointer to line pointer array   */
	int	*_firstch;	/* first changed character in line */
	int	*_lastch;	/* last changed character in line  */
	int	_tmarg;		/* top of scrolling region	*/
	int	_bmarg;		/* bottom of scrolling region	*/
	int	_delayms;	/* milliseconds of delay for getch() */
	int	_parx, _pary;	/* coords relative to parent (0,0) */
	struct	_win *_parent;	/* subwin's pointer to parent win  */
} WINDOW;



/*----------------------------------------------------------------------
 *
 *	Private structures that are necessary for correct
 *	macro construction.
 *
 */

typedef struct			/* structure for ripped off lines */
{
	int line;
	int (*init)(WINDOW *, int);
} RIPPEDOFFLINE;


typedef struct
{
	bool	alive;		/* if initscr() called			*/
	bool	autocr;		/* if cr -> lf				*/
	bool	cbreak;		/* if terminal unbuffered		*/
	bool	echo;		/* if terminal echo			*/
	bool	raw_inp;	/* raw input mode (v. cooked input)	*/
	bool	raw_out;	/* raw output mode (7 v. 8 bits)	*/
	bool	orgcbr;		/* original MSDOS ^-BREAK setting	*/
	bool	visible_cursor; /* TRUE if cursor is visible		*/
	bool	audible;	/* FALSE if the bell is visual		*/
	bool	mono;		/* TRUE if current screen is mono	*/
	bool	sizeable;	/* TRUE if adapter is resizeable	*/
	bool	resized;	/* TRUE if TERM has been resized	*/
	bool	shell;		/* TRUE if reset_prog_mode() needs
				   to be called.			*/
	bool	orig_attr;	/* TRUE if we have the original colors	*/
	short	orig_fore;	/* Original screen foreground color	*/
	short	orig_back;	/* Original screen foreground color	*/
	chtype	blank;		/* Background character			*/
	int	cursrow;	/* position of physical cursor		*/
	int	curscol;	/* position of physical cursor		*/
	int	cursor;		/* Current Cursor definition		*/
	int	visibility;	/* Visibility of cursor			*/
	int	orig_emulation; /* Original cursor emulation value	*/
	int	orig_cursor;	/* Original cursor size			*/
	int	font;		/* default font size			*/
	int	orig_font;	/* Original font size			*/
	int	lines;		/* New value for LINES			*/
	int	cols;		/* New value for COLS			*/
	unsigned long _trap_mbe;	/* trap these mouse button events */
	unsigned long _map_mbe_to_key;	/* map mouse buttons to slk	*/
	int	slklines;		/* Lines in use by slk_init()	*/
	WINDOW *slk_winptr;		/* window for slk		*/
	int	linesrippedoff;		/* Lines ripped off via ripoffline() */
	int	linesrippedoffontop;	/* Lines ripped off on 
					   top via ripoffline()		*/
	int	delaytenths;		/* 1/10ths second to wait block
					   getch() for			*/
	bool	_preserve;		/* TRUE if screen background
					   to be preserved		*/
	int	_restore;		/* specifies if screen background
					   to be restored, and how	*/
	bool	save_key_modifiers;	/* TRUE if each key modifiers saved
					   with each key press		*/
	bool	return_key_modifiers;	/* TRUE if modifier keys are
					   returned as "real" keys	*/

#ifdef OS2
# ifdef EMXVIDEO			/* nop if using EMX builtins	*/
	int	tahead; 		/* Type-ahead value		*/
	int	adapter;		/* Screen type			*/
# else
	VIOMODEINFO scrnmode;		/* default screen mode		*/
	VIOCONFIGINFO adapter;		/* Screen type			*/
	KBDINFO kbdinfo;		/* keyboard info		*/
# endif
#else
	int	adapter;		/* Screen type			*/
#endif

#ifdef DOS
	int	scrnmode;	/* default screen mode			*/
	bool	direct_video;	/* Allow Direct Screen Memory writes	*/
	int	video_page;	/* Current PC video page		*/
	unsigned video_seg;	/* video base segment			*/
	unsigned video_ofs;	/* video base offset			*/
	bool	bogus_adapter;	/* TRUE if adapter has insane values	*/
#endif

#ifdef XCURSES
	int	XcurscrSize;	/* size of Xcurscr shared memory block	*/
	bool	sb_on;
	int	sb_viewport_y;
	int	sb_viewport_x;
	int	sb_total_y;
	int	sb_total_x;
	int	sb_cur_y;
	int	sb_cur_x;
#endif
	short	line_color;	/* Color of line attributes - default white */

} SCREEN;


/* external variables */
#ifdef PDC_DLL_BUILD
# ifndef CURSES_LIBRARY
__declspec(dllimport)	int		LINES;
__declspec(dllimport)	int		COLS;
__declspec(dllimport)	WINDOW		*stdscr;
__declspec(dllimport)	WINDOW		*curscr;
__declspec(dllimport)	SCREEN		*SP;
__declspec(dllimport)	int		use_emalloc;
__declspec(dllimport)	MOUSE_STATUS	Mouse_status;
__declspec(dllimport)	int		COLORS;
__declspec(dllimport)	int		COLOR_PAIRS;
# else
__declspec(dllexport) extern	int		LINES;
__declspec(dllexport) extern	int		COLS;
__declspec(dllexport) extern	WINDOW		*stdscr;
__declspec(dllexport) extern	WINDOW		*curscr;
__declspec(dllexport) extern	SCREEN		*SP;
__declspec(dllexport) extern	int		use_emalloc;
__declspec(dllexport) extern	MOUSE_STATUS	Mouse_status;
__declspec(dllexport) extern	int 		COLORS;
__declspec(dllexport) extern	int		COLOR_PAIRS;
# endif
#else
extern	int		LINES;		/* terminal height		*/
extern	int		COLS;		/* terminal width		*/
extern	WINDOW		*stdscr;	/* the default screen window	*/
extern	WINDOW		*curscr;	/* the current screen image	*/
extern	SCREEN		*SP;		/* curses variables		*/
extern	int		use_emalloc;	/* set to use your own malloc	*/
extern	MOUSE_STATUS	Mouse_status;
extern	int		COLORS, COLOR_PAIRS;
#endif

#ifdef CURSES_LIBRARY
extern	int		_default_lines;	/* For presetting maximum lines	*/
#endif



/*man-start**************************************************************

PDCurses Text Attributes:

Originally, PDCurses used a short (16 bits) for its chtype. To include 
colour, a number of things had to be sacrificed from the strict Unix and 
System V support. The main problem was fitting all character attributes 
and colour into an unsigned char (all 8 bits!).

Today, PDCurses by default uses a long (32 bits) for its chtype, as in 
System V. The short chtype is still available, by undefining CHTYPE_LONG 
and rebuilding the library.

The following is the structure of a win->_attrs chtype:

short form:

-------------------------------------------------
|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
-------------------------------------------------
 colour number |  attrs |   character eg 'a'

The available attribute enhancers are bold, reverse and blink. All other 
Unix attributes have no effect. The high order char is an index into an 
array of physical colours (defined in INITPAIR.c). 32 (5 bits) 
foreground/background colour combinations combined with 8 (3 bits) 
attribute modifiers are available.

long form:

----------------------------------------------------------------------------
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|..| 3| 2| 1| 0|
----------------------------------------------------------------------------
     colour number      |     modifiers         |      character eg 'a'
						     (potential for DBCS)

The available attribute modifiers are bold, underline, invisible, 
protect, reverse and blink. 256 (8 bits) colour pairs, 8 bits for 
modifiers, and 16 bits for character data. (In practice, only 64 colour 
pairs are available, and only the lower 8 bits of character data are 
currently used.)

**man-end****************************************************************/

/*** Video attribute macros ***/

#ifdef CHTYPE_LONG
# define A_NORMAL	(chtype)0x00000000
# define A_UNDERLINE	(chtype)0x00100000
# define A_REVERSE	(chtype)0x00200000
# define A_BLINK	(chtype)0x00400000
# define A_BOLD		(chtype)0x00800000
# define A_RIGHTLINE	(chtype)0x00010000
# define A_LEFTLINE	(chtype)0x00020000
# define A_ALTCHARSET	(chtype)0x00040000
# define A_INVIS	(chtype)0x00080000
# define A_ATTRIBUTES	(chtype)0xFFFF0000
# define A_CHARTEXT	(chtype)0x0000FFFF
# define A_COLOR	(chtype)0xFF000000

# define A_DIM		A_NORMAL
# define A_ITALIC	A_INVIS
# define A_STANDOUT	(A_BOLD | A_REVERSE)
# define A_PROTECT	(A_UNDERLINE | A_LEFTLINE | A_RIGHTLINE)

# define PDC_ATTR_SHIFT	19
# define PDC_COLOR_SHIFT 24
#else
# define A_NORMAL	(chtype)0x0000			/* System V */
# define A_ALTCHARSET	(chtype)0x0000			/* X/Open   */
# define A_BLINK	(chtype)0x0400			/* X/Open   */
# define A_BLANK	(chtype)0x0000			/* X/Open   */
# define A_BOLD		(chtype)0x0100			/* X/Open   */
# define A_DIM		(chtype)0x0000			/* X/Open   */
# define A_PROTECT	(chtype)0x0000			/* X/Open   */
# define A_REVERSE	(chtype)0x0200			/* X/Open   */
# define A_STANDOUT	(A_REVERSE | A_BOLD)		/* X/Open   */
# define A_UNDERLINE	(chtype)0x0000			/* X/Open   */
# define A_COLOR	(chtype)0xF800			/* System V */
# define A_CHARTEXT	(chtype)(0xFF)			/* X/Open   */
# define A_ATTRIBUTES	(chtype)(~A_CHARTEXT)		/* X/Open   */

# define A_LEFTLINE	(chtype)0x0000
# define A_RIGHTLINE	(chtype)0x0000
# define A_ITALIC	(chtype)0x0000
# define A_INVIS	(chtype)0x0000

# define PDC_ATTR_SHIFT	8
# define PDC_COLOR_SHIFT 11
#endif

#define CHR_MSK		A_CHARTEXT			/* Obsolete */
#define ATR_MSK		A_ATTRIBUTES			/* Obsolete */
#define ATR_NRM		A_NORMAL			/* Obsolete */

/*** Alternate character set macros ***/

/* ALTCHARSET definitions for PC from jshumate@wrdis01.robins.af.mil
   These match code page 437 and compatible pages (CP850, CP852, etc.) */

/* VT100-compatible symbols -- box chars */

#ifdef XCURSES
# define ACS_ULCORNER	((chtype)13 | A_ALTCHARSET)
# define ACS_LLCORNER	((chtype)14 | A_ALTCHARSET)
# define ACS_URCORNER	((chtype)12 | A_ALTCHARSET)
# define ACS_LRCORNER	((chtype)11 | A_ALTCHARSET)
# define ACS_RTEE	((chtype)22 | A_ALTCHARSET)
# define ACS_LTEE	((chtype)21 | A_ALTCHARSET)
# define ACS_BTEE	((chtype)23 | A_ALTCHARSET)
# define ACS_TTEE	((chtype)24 | A_ALTCHARSET)
# define ACS_HLINE	((chtype)18 | A_ALTCHARSET)
# define ACS_VLINE	((chtype)25 | A_ALTCHARSET)
# define ACS_PLUS	((chtype)15 | A_ALTCHARSET)
#else
# define ACS_ULCORNER	(chtype)0xda
# define ACS_LLCORNER	(chtype)0xc0
# define ACS_URCORNER	(chtype)0xbf
# define ACS_LRCORNER	(chtype)0xd9
# define ACS_RTEE	(chtype)0xb4
# define ACS_LTEE	(chtype)0xc3
# define ACS_BTEE	(chtype)0xc1
# define ACS_TTEE	(chtype)0xc2
# define ACS_HLINE	(chtype)0xc4
# define ACS_VLINE	(chtype)0xb3
# define ACS_PLUS	(chtype)0xc5
#endif

/* VT100-compatible symbols -- other */

#ifdef XCURSES
# define ACS_S1		((chtype)16 | A_ALTCHARSET)
# define ACS_S9		((chtype)20 | A_ALTCHARSET)
# define ACS_DIAMOND	((chtype)1 | A_ALTCHARSET)
# define ACS_CKBOARD	((chtype)2 | A_ALTCHARSET)
# define ACS_DEGREE	((chtype)7 | A_ALTCHARSET)
# define ACS_PLMINUS	((chtype)8 | A_ALTCHARSET)
# define ACS_BULLET	(chtype)'*'
#else
# define ACS_S1		(chtype)0x2d
# define ACS_S9		(chtype)0x5f
# define ACS_DIAMOND	((chtype)0x04 | A_ALTCHARSET)
# define ACS_CKBOARD	(chtype)0xb1
# define ACS_DEGREE	(chtype)0xf8
# define ACS_PLMINUS	(chtype)0xf1
# define ACS_BULLET	(chtype)0xf9
#endif

/* Teletype 5410v1 symbols -- these are defined in SysV curses, but
   are not well-supported by most terminals. Stick to VT100 characters
   for optimum portability. */

#ifdef XCURSES
# define ACS_LARROW	(chtype)'<'
# define ACS_RARROW	(chtype)'>'
# define ACS_DARROW	(chtype)'v'
# define ACS_UARROW	(chtype)'^'
# define ACS_BOARD	(chtype)'#'
# define ACS_LANTERN	(chtype)'#'
# define ACS_BLOCK	((chtype)0 | A_ALTCHARSET)
#else
# define ACS_LARROW	((chtype)0x1b | A_ALTCHARSET)
# define ACS_RARROW	((chtype)0x1a | A_ALTCHARSET)
# define ACS_DARROW	((chtype)0x19 | A_ALTCHARSET)
# define ACS_UARROW	((chtype)0x18 | A_ALTCHARSET)
# define ACS_BOARD	(chtype)0xb0
# define ACS_LANTERN	((chtype)0x0f | A_ALTCHARSET)
# define ACS_BLOCK	(chtype)0xdb
#endif

/* That goes double for these -- undocumented SysV symbols. Don't use
   them. Also, the definitions here aren't compatible with as many
   code pages as those above. */

#ifdef XCURSES
# define ACS_S3		((chtype)17 | A_ALTCHARSET)
# define ACS_S7		((chtype)19 | A_ALTCHARSET)
# define ACS_LEQUAL	((chtype)26 | A_ALTCHARSET)
# define ACS_GEQUAL	((chtype)27 | A_ALTCHARSET)
# define ACS_PI		((chtype)28 | A_ALTCHARSET)
# define ACS_NEQUAL	((chtype)29 | A_ALTCHARSET)
# define ACS_STERLING	((chtype)30 | A_ALTCHARSET)
#else
# define ACS_S3         (chtype)0x2d
# define ACS_S7         (chtype)0x2d
# define ACS_LEQUAL     (chtype)0xf3
# define ACS_GEQUAL     (chtype)0xf2
# define ACS_PI         (chtype)0xe3
# define ACS_NEQUAL     (chtype)0xd8
# define ACS_STERLING   (chtype)0x9c
#endif

/* Box char aliases */

#define ACS_BSSB	ACS_ULCORNER
#define ACS_SSBB	ACS_LLCORNER
#define ACS_BBSS	ACS_URCORNER
#define ACS_SBBS	ACS_LRCORNER
#define ACS_SBSS	ACS_RTEE
#define ACS_SSSB	ACS_LTEE
#define ACS_SSBS	ACS_BTEE
#define ACS_BSSS	ACS_TTEE
#define ACS_BSBS	ACS_HLINE
#define ACS_SBSB	ACS_VLINE
#define ACS_SSSS	ACS_PLUS

/*** Color macros ***/

#define COLOR_BLACK	0

#ifdef XCURSES		/* RGB */
# define COLOR_RED	1
# define COLOR_GREEN	2
# define COLOR_BLUE	4
#else			/* BGR */
# define COLOR_BLUE	1
# define COLOR_GREEN	2
# define COLOR_RED	4
#endif

#define COLOR_CYAN	(COLOR_BLUE | COLOR_GREEN)
#define COLOR_MAGENTA	(COLOR_RED | COLOR_BLUE)
#define COLOR_YELLOW	(COLOR_RED | COLOR_GREEN)

#define COLOR_WHITE	7

/*----------------------------------------------------------------------
 *
 *	Function and Keypad Key Definitions.
 *	Many are just for compatibility.
 *
 */

enum
{
	KEY_BREAK = 0x101,	/* Not on PC KBD	*/
	KEY_DOWN,	/* Down arrow key		*/
	KEY_UP,		/* Up arrow key			*/
	KEY_LEFT,	/* Left arrow key		*/
	KEY_RIGHT,	/* Right arrow key		*/
	KEY_HOME,	/* home key			*/
	KEY_BACKSPACE,	/* not on pc			*/
	KEY_F0,		/* function keys; 64 reserved	*/

	KEY_DL = KEY_F0 + 64,	/* delete line		*/
	KEY_IL,		/* insert line			*/
	KEY_DC,		/* delete character		*/
	KEY_IC,		/* insert char or enter ins mode */
	KEY_EIC,	/* exit insert char mode	*/
	KEY_CLEAR,	/* clear screen			*/
	KEY_EOS,	/* clear to end of screen	*/
	KEY_EOL,	/* clear to end of line		*/
	KEY_SF,		/* scroll 1 line forward	*/
	KEY_SR,		/* scroll 1 line back (reverse)	*/
	KEY_NPAGE,	/* next page			*/
	KEY_PPAGE,	/* previous page		*/
	KEY_STAB,	/* set tab			*/
	KEY_CTAB,	/* clear tab			*/
	KEY_CATAB,	/* clear all tabs		*/
	KEY_ENTER,	/* enter or send (unreliable)	*/
	KEY_SRESET,	/* soft/reset (partial/unreliable) */
	KEY_RESET,	/* reset/hard reset (unreliable) */
	KEY_PRINT,	/* print/copy			*/
	KEY_LL,		/* home down/bottom (lower left) */
	KEY_ABORT,	/* abort/terminate key (any)	*/
	KEY_SHELP,	/* short help			*/
	KEY_LHELP,	/* long help			*/
	KEY_BTAB,	/* Back tab key			*/
	KEY_BEG,	/* beg(inning) key		*/
	KEY_CANCEL,	/* cancel key			*/
	KEY_CLOSE,	/* close key			*/
	KEY_COMMAND,	/* cmd (command) key		*/
	KEY_COPY,	/* copy key			*/
	KEY_CREATE,	/* create key			*/
	KEY_END,	/* end key			*/
	KEY_EXIT,	/* exit key			*/
	KEY_FIND,	/* find key			*/
	KEY_HELP,	/* help key			*/
	KEY_MARK,	/* mark key			*/
	KEY_MESSAGE,	/* message key			*/
	KEY_MOVE,	/* move key			*/
	KEY_NEXT,	/* next object key		*/
	KEY_OPEN,	/* open key			*/
	KEY_OPTIONS,	/* options key			*/
	KEY_PREVIOUS,	/* previous object key		*/
	KEY_REDO,	/* redo key			*/
	KEY_REFERENCE,	/* ref(erence) key		*/
	KEY_REFRESH,	/* refresh key			*/
	KEY_REPLACE,	/* replace key			*/
	KEY_RESTART,	/* restart key			*/
	KEY_RESUME,	/* resume key			*/
	KEY_SAVE,	/* save key			*/
	KEY_SBEG,	/* shifted beginning key	*/
	KEY_SCANCEL,	/* shifted cancel key		*/
	KEY_SCOMMAND,	/* shifted command key		*/
	KEY_SCOPY,	/* shifted copy key		*/
	KEY_SCREATE,	/* shifted create key		*/
	KEY_SDC,	/* shifted delete char key	*/
	KEY_SDL,	/* shifted delete line key	*/
	KEY_SELECT,	/* select key			*/
	KEY_SEND,	/* shifted end key		*/
	KEY_SEOL,	/* shifted clear line key	*/
	KEY_SEXIT,	/* shifted exit key		*/
	KEY_SFIND,	/* shifted find key		*/
	KEY_SHOME,	/* shifted home key		*/
	KEY_SIC,	/* shifted input key		*/

	KEY_NONAME,	/* not used -- remove later	*/

	KEY_SLEFT,	/* shifted left arrow key	*/
	KEY_SMESSAGE,	/* shifted message key		*/
	KEY_SMOVE,	/* shifted move key		*/
	KEY_SNEXT,	/* shifted next key		*/
	KEY_SOPTIONS,	/* shifted options key		*/
	KEY_SPREVIOUS,	/* shifted prev key		*/
	KEY_SPRINT,	/* shifted print key		*/
	KEY_SREDO,	/* shifted redo key		*/
	KEY_SREPLACE,	/* shifted replace key		*/
	KEY_SRIGHT,	/* shifted right arrow		*/
	KEY_SRSUME,	/* shifted resume key		*/
	KEY_SSAVE,	/* shifted save key		*/
	KEY_SSUSPEND,	/* shifted suspend key		*/
	KEY_SUNDO,	/* shifted undo key		*/
	KEY_SUSPEND,	/* suspend key			*/
	KEY_UNDO,	/* undo key			*/

	/* PDCurses-specific key definitions -- PC only */

	ALT_0, ALT_1, ALT_2, ALT_3, ALT_4, ALT_5, ALT_6, ALT_7, ALT_8, 
	ALT_9, ALT_A, ALT_B, ALT_C, ALT_D, ALT_E, ALT_F, ALT_G, ALT_H, 
	ALT_I, ALT_J, ALT_K, ALT_L, ALT_M, ALT_N, ALT_O, ALT_P, ALT_Q, 
	ALT_R, ALT_S, ALT_T, ALT_U, ALT_V, ALT_W, ALT_X, ALT_Y, ALT_Z,

	CTL_LEFT,	/* Control-Left-Arrow	*/
	CTL_RIGHT, CTL_PGUP, CTL_PGDN, CTL_HOME, CTL_END,

	KEY_A1,		/* upper left on Virtual keypad	*/
	KEY_A2,		/* upper middle on Virt. keypad	*/
	KEY_A3,		/* upper right on Vir. keypad	*/
	KEY_B1,		/* middle left on Virt. keypad	*/
	KEY_B2,		/* center on Virt. keypad	*/
	KEY_B3,		/* middle right on Vir. keypad	*/
	KEY_C1,		/* lower left on Virt. keypad	*/
	KEY_C2,		/* lower middle on Virt. keypad */
	KEY_C3,		/* lower right on Vir. keypad	*/

	PADSLASH,	/* slash on keypad		*/
	PADENTER,	/* enter on keypad		*/
	CTL_PADENTER,	/* ctl-enter on keypad		*/
	ALT_PADENTER,	/* alt-enter on keypad		*/
	PADSTOP,	/* stop on keypad		*/
	PADSTAR,	/* star on keypad		*/
	PADMINUS,	/* minus on keypad		*/
	PADPLUS,	/* plus on keypad		*/
	CTL_PADSTOP,	/* ctl-stop on keypad		*/
	CTL_PADCENTER,	/* ctl-enter on keypad		*/
	CTL_PADPLUS,	/* ctl-plus on keypad		*/
	CTL_PADMINUS,	/* ctl-minus on keypad		*/
	CTL_PADSLASH,	/* ctl-slash on keypad		*/
	CTL_PADSTAR,	/* ctl-star on keypad		*/
	ALT_PADPLUS,	/* alt-plus on keypad		*/
	ALT_PADMINUS,	/* alt-minus on keypad		*/
	ALT_PADSLASH,	/* alt-slash on keypad		*/
	ALT_PADSTAR,	/* alt-star on keypad		*/
	ALT_PADSTOP,	/* alt-stop on keypad		*/
	CTL_INS,	/* ctl-insert			*/
	ALT_DEL,	/* alt-delete			*/
	ALT_INS,	/* alt-insert			*/
	CTL_UP,		/* ctl-up arrow			*/
	CTL_DOWN,	/* ctl-down arrow		*/
	CTL_TAB,	/* ctl-tab			*/
	ALT_TAB,
	ALT_MINUS,
	ALT_EQUAL,
	ALT_HOME,
	ALT_PGUP,
	ALT_PGDN,
	ALT_END,
	ALT_UP,		/* alt-up arrow			*/
	ALT_DOWN,	/* alt-down arrow		*/
	ALT_RIGHT,	/* alt-right arrow		*/
	ALT_LEFT,	/* alt-left arrow		*/
	ALT_ENTER,	/* alt-enter			*/
	ALT_ESC,	/* alt-escape			*/
	ALT_BQUOTE,	/* alt-back quote		*/
	ALT_LBRACKET,	/* alt-left bracket		*/
	ALT_RBRACKET,	/* alt-right bracket		*/
	ALT_SEMICOLON,	/* alt-semi-colon		*/
	ALT_FQUOTE,	/* alt-forward quote		*/
	ALT_COMMA,	/* alt-comma			*/
	ALT_STOP,	/* alt-stop			*/
	ALT_FSLASH,	/* alt-forward slash		*/
	ALT_BKSP,	/* alt-backspace		*/
	CTL_BKSP,	/* ctl-backspace		*/
	PAD0,		/* keypad 0			*/

	CTL_PAD0,	/* ctl-keypad 0			*/
	CTL_PAD1, CTL_PAD2, CTL_PAD3, CTL_PAD4, CTL_PAD5,
	CTL_PAD6, CTL_PAD7, CTL_PAD8, CTL_PAD9,

	ALT_PAD0,	/* alt-keypad 0			*/
	ALT_PAD1, ALT_PAD2, ALT_PAD3, ALT_PAD4, ALT_PAD5,
	ALT_PAD6, ALT_PAD7, ALT_PAD8, ALT_PAD9,

	CTL_DEL,	/* clt-delete			*/
	ALT_BSLASH,	/* alt-back slash		*/
	CTL_ENTER,	/* ctl-enter			*/

	SHF_PADENTER,	/* shift-enter on keypad	*/
	SHF_PADSLASH, SHF_PADSTAR, SHF_PADPLUS, SHF_PADMINUS,
	SHF_UP,		/* shift-up on keypad		*/
	SHF_DOWN,	/* shift-down on keypad		*/
	SHF_IC,		/* shift-insert on keypad	*/
	SHF_DC,		/* shift-delete on keypad	*/

	KEY_MOUSE,	/* "mouse" key			*/
	KEY_SHIFT_L,	/* Left-shift			*/
	KEY_SHIFT_R,	/* Right-shift			*/
	KEY_CONTROL_L,	/* Left-control			*/
	KEY_CONTROL_R,	/* Right-control		*/
	KEY_ALT_L,	/* Left-alt			*/
	KEY_ALT_R,	/* Right-alt			*/
	KEY_RESIZE,	/* Window resize		*/
	KEY_SUP,	/* Shifted up arrow		*/
	KEY_SDOWN	/* Shifted down arrow		*/
};

#define KEY_MIN		KEY_BREAK	/* Minimum curses key value	*/
#define KEY_MAX		KEY_SDOWN	/* Maximum curses key		*/

#define KEY_F(n)	(KEY_F0 + (n))

/*----------------------------------------------------------------------
 *
 *	PDCurses function declarations
 *
 */

/* Standard */

int	addch(const chtype);
int	addchnstr(const chtype *, int);
int	addchstr(const chtype *);
int	addnstr(const char *, int);
int	addstr(const char *);
int	attroff(chtype);
int	attron(chtype);
int	attrset(chtype);
int	baudrate(void);
int	beep(void);
int	bkgd(chtype);
void	bkgdset(chtype);
int	border(chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype);
int	box(WINDOW *, chtype, chtype);
bool	can_change_color(void);
int	cbreak(void); 
int	clearok(WINDOW *, bool);
int	clear(void);
int	clrtobot(void);
int	clrtoeol(void);
int	color_content(short, short *, short *, short *);
chtype	COLOR_PAIR(int);
int	color_set(short, void *);
int	copywin(const WINDOW *, WINDOW *, int, int, int, int, int, int, int);
int	curs_set(int);
int	def_prog_mode(void);
int	def_shell_mode(void);
int	delay_output(int);
int	delch(void);
int	deleteln(void);
void	delscreen(SCREEN *); 
int	delwin(WINDOW *);
WINDOW *derwin(WINDOW *, int, int, int, int);
int	doupdate(void);
WINDOW *dupwin(WINDOW *);
int	echochar(const chtype);
int	echo(void);
int	endwin(void);
char	erasechar(void);
int	erase(void);
void	filter(void);
int	flash(void);
int	flushinp(void);
chtype	getbkgd(WINDOW *);
int	getnstr(char *, int);
int	getstr(char *);
WINDOW *getwin(FILE *);
int	halfdelay(int);
bool	has_colors(void);
bool	has_ic(void);
bool	has_il(void);
int	hline(chtype, int);
void	idcok(WINDOW *, bool);
int	idlok(WINDOW *, bool);
void	immedok(WINDOW *, bool);
int	inchnstr(chtype *, int);
int	inchstr(chtype *);
chtype	inch(void);
int	init_color(short, short, short, short);
int	init_pair(short, short, short);
WINDOW *initscr(void);
int	innstr(char *, int);
int	insch(chtype);
int	insdelln(int);
int	insertln(void);
int	insnstr(const char *, int);
int	insstr(const char *);
int	instr(char *);
int	intrflush(WINDOW *, bool);
bool	isendwin(void);
bool	is_linetouched(WINDOW *, int);
bool	is_wintouched(WINDOW *);
char   *keyname(int);
int	keypad(WINDOW *, bool);
char	killchar(void);
int	leaveok(WINDOW *, bool);
char   *longname(void);
int	meta(WINDOW *, bool);
int	move(int, int);
int	mvaddch(int, int, const chtype);
int	mvaddchnstr(int, int, const chtype *, int);
int	mvaddchstr(int, int, const chtype *);
int	mvaddnstr(int, int, const char *, int);
int	mvaddstr(int, int, const char *);
int	mvcur(int, int, int, int);
int	mvdelch(int, int);
int	mvderwin(WINDOW *, int, int);
int	mvgetch(int, int);
int	mvgetnstr(int, int, char *, int);
int	mvgetstr(int, int, char *);
int	mvhline(int, int, chtype, int);
chtype	mvinch(int, int);
int	mvinchnstr(int, int, chtype *, int);
int	mvinchstr(int, int, chtype *);
int	mvinnstr(int, int, char *, int);
int	mvinsch(int, int, chtype);
int	mvinsnstr(int, int, const char *, int);
int	mvinsstr(int, int, const char *);
int	mvinstr(int, int, char *);
int	mvprintw(int, int, const char *, ...);
int	mvscanw(int, int, const char *, ...);
int	mvvline(int, int, chtype, int);
int	mvwaddchnstr(WINDOW *, int, int, const chtype *, int);
int	mvwaddchstr(WINDOW *, int, int, const chtype *);
int	mvwaddch(WINDOW *, int, int, const chtype);
int	mvwaddnstr(WINDOW *, int, int, const char *, int);
int	mvwaddstr(WINDOW *, int, int, const char *);
int	mvwdelch(WINDOW *, int, int);
int	mvwgetch(WINDOW *, int, int);
int	mvwgetnstr(WINDOW *, int, int, char *, int);
int	mvwgetstr(WINDOW *, int, int, char *);
int	mvwhline(WINDOW *, int, int, chtype, int);
int	mvwinchnstr(WINDOW *, int, int, chtype *, int);
int	mvwinchstr(WINDOW *, int, int, chtype *);
chtype	mvwinch(WINDOW *, int, int);
int	mvwinnstr(WINDOW *, int, int, char *, int);
int	mvwinsch(WINDOW *, int, int, chtype);
int	mvwinsnstr(WINDOW *, int, int, const char *, int);
int	mvwinsstr(WINDOW *, int, int, const char *);
int	mvwinstr(WINDOW *, int, int, char *);
int	mvwin(WINDOW *, int, int);
int	mvwprintw(WINDOW *, int, int, const char *, ...);
int	mvwscanw(WINDOW *, int, int, const char *, ...);
int	mvwvline(WINDOW *, int, int, chtype, int);
int	napms(int);
WINDOW *newpad(int, int);
SCREEN *newterm(char *, FILE *, FILE *);
WINDOW *newwin(int, int, int, int);
int	nl(void);
int	nocbreak(void);
int	nodelay(WINDOW *, bool);
int	noecho(void);
int	nonl(void);
void	noqiflush(void);
int	noraw(void);
int	notimeout(WINDOW *, bool);
int	overlay(const WINDOW *, WINDOW *);
int	overwrite(const WINDOW *, WINDOW *);
int	pair_content(short, short *, short *);
int	PAIR_NUMBER(chtype);
int	pechochar(WINDOW *, chtype);
int	pnoutrefresh(WINDOW *, int, int, int, int, int, int);
int	prefresh(WINDOW *, int, int, int, int, int, int);
int	printw(const char *, ...);
int	putwin(WINDOW *, FILE *);
void	qiflush(void);
int	raw(void);
int	redrawwin(WINDOW *);
int	refresh(void);
int	reset_prog_mode(void);
int	reset_shell_mode(void);
int	resetty(void);
int	ripoffline(int, int (*)(WINDOW *, int));
int	savetty(void);
int	scanw(const char *, ...);
int	scr_dump(const char *);
int	scr_init(const char *);
int	scrl(int);
int	scrollok(WINDOW *, bool);
int	scroll(WINDOW *);
int	scr_restore(const char *);
int	scr_set(const char *);
int	setscrreg(int, int);
SCREEN *set_term(SCREEN *);
int	slk_attroff(const chtype);
int	slk_attron(const chtype);
int	slk_attrset(const chtype);
int	slk_clear(void);
int	slk_color(short);
int	slk_init(int);
char   *slk_label(int);
int	slk_noutrefresh(void);
int	slk_refresh(void);
int	slk_restore(void);
int	slk_set(int, const char *, int);
int	slk_touch(void);
int	standend(void);
int	standout(void);
int	start_color(void);
WINDOW *subpad(WINDOW *, int, int, int, int);
WINDOW *subwin(WINDOW *, int, int, int, int);
int	syncok(WINDOW *, bool);
chtype	termattrs(void);
char   *termname(void);
void	timeout(int);
int	touchline(WINDOW *, int, int);
int	touchwin(WINDOW *);
int	typeahead(int);
int	untouchwin(WINDOW *);
void	use_env(bool);
int	vidattr(chtype);
int	vidputs(chtype, int (*)(int));
int	vline(chtype, int);
int	vw_printw(WINDOW *, const char *, va_list);
int	vwprintw(WINDOW *, const char *, va_list);
int	vw_scanw(WINDOW *, const char *, va_list);
int	vwscanw(WINDOW *, const char *, va_list);
int	waddchnstr(WINDOW *, const chtype *, int);
int	waddchstr(WINDOW *, const chtype *);
int	waddch(WINDOW *, const chtype);
int	waddnstr(WINDOW *, const char *, int);
int	waddstr(WINDOW *, const char *);
int	wattroff(WINDOW *, chtype);
int	wattron(WINDOW *, chtype);
int	wattrset(WINDOW *, chtype);
void	wbkgdset(WINDOW *, chtype);
int	wbkgd(WINDOW *, chtype);
int	wborder(WINDOW *, chtype, chtype, chtype, chtype,
		chtype, chtype, chtype, chtype);
int	wclear(WINDOW *);
int	wclrtobot(WINDOW *);
int	wclrtoeol(WINDOW *);
int	wcolor_set(WINDOW *, short, void *);
void	wcursyncup(WINDOW *);
int	wdelch(WINDOW *);
int	wdeleteln(WINDOW *);
int	wechochar(WINDOW *, const chtype);
int	werase(WINDOW *);
int	wgetch(WINDOW *);
int	wgetnstr(WINDOW *, char *, int);
int	wgetstr(WINDOW *, char *);
int	whline(WINDOW *, chtype, int);
int	winchnstr(WINDOW *, chtype *, int);
int	winchstr(WINDOW *, chtype *);
chtype	winch(WINDOW *);
int	winnstr(WINDOW *, char *, int);
int	winsch(WINDOW *, chtype);
int	winsdelln(WINDOW *, int);
int	winsertln(WINDOW *);
int	winsnstr(WINDOW *, const char *, int);
int	winsstr(WINDOW *, const char *);
int	winstr(WINDOW *, char *);
int	wmove(WINDOW *, int, int);
int	wnoutrefresh(WINDOW *);
int	wprintw(WINDOW *, const char *, ...);
int	wredrawln(WINDOW *, int, int);
int	wrefresh(WINDOW *);
int	wscanw(WINDOW *, const char *, ...);
int	wscrl(WINDOW *, int);
int	wsetscrreg(WINDOW *, int, int);
int	wstandend(WINDOW *);
int	wstandout(WINDOW *);
void	wsyncdown(WINDOW *);
void	wsyncup(WINDOW *);
void	wtimeout(WINDOW *, int);
int	wtouchln(WINDOW *, int, int, int);
int	wvline(WINDOW *, chtype, int);

/* Quasi-standard */

chtype	getattrs(WINDOW *);
char   *unctrl(chtype);

int	mouse_set(unsigned long);
int	mouse_on(unsigned long);
int	mouse_off(unsigned long);
int	request_mouse_pos(void);
int	map_button(unsigned long);
void	wmouse_position(WINDOW *, int *, int *);
unsigned long getmouse(void);
unsigned long getbmap(void);

/* NCurses */

bool	has_key(int);

/* PDCurses */

int	mvwinsertln(WINDOW *, int, int);
int	raw_output(bool);
int	resize_term(int, int);
WINDOW *resize_window(WINDOW *, int, int);
void	traceoff(void);
void	traceon(void);
char	wordchar(void);

#ifdef XCURSES
WINDOW *Xinitscr(int, char **);
void	XCursesExit(void);
int	sb_init(void);
int	sb_set_horz(int, int, int);
int	sb_set_vert(int, int, int);
int	sb_get_horz(int *, int *, int *);
int	sb_get_vert(int *, int *, int *);
int	sb_refresh(void);
#endif

int	PDC_chadd(WINDOW *, chtype, bool, bool);
int	PDC_chins(WINDOW *, chtype, bool);
int	PDC_ungetch(int);
void	PDC_set_title(const char *);
int	PDC_getclipboard(char **, long *);
int	PDC_setclipboard(const char *, long);
int	PDC_freeclipboard(char *);
int	PDC_clearclipboard(void);

unsigned long PDC_get_input_fd(void);
unsigned long PDC_get_key_modifiers(void);

int	PDC_wunderline(WINDOW *, int, bool);
int	PDC_wleftline(WINDOW *, int, bool);
int	PDC_wrightline(WINDOW *, int, bool);
int	PDC_set_line_color(short);

#ifndef max
# define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
# define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/*** Functions defined as macros ***/

#define getch()			wgetch(stdscr)
#define ungetch(ch)		PDC_ungetch(ch)

/* These will _only_ work as macros */

#define getbegyx(w, y, x)	(y = (w)->_begy, x = (w)->_begx)
#define getmaxyx(w, y, x)	(y = (w)->_maxy, x = (w)->_maxx)
#define getparyx(w, y, x)	(y = (w)->_pary, x = (w)->_parx)
#define getyx(w, y, x)		(y = (w)->_cury, x = (w)->_curx)

#define getbegx(w)		(w)->_begx
#define getbegy(w)		(w)->_begy
#define getmaxx(w)		(w)->_maxx
#define getmaxy(w)		(w)->_maxy
#define getparx(w)		(w)->_parx
#define getpary(w)		(w)->_pary

#define getsyx(y, x)		{ if (curscr->_leaveit) (y)=(x)=-1; \
				  else getyx(curscr,(y),(x)); }

#define setsyx(y, x)		{ if((y)==-1 && (x)==-1) curscr->_leaveit=TRUE;\
				  else { curscr->_leaveit=FALSE; \
					 wmove(curscr,(y),(x));} }

#define wresize(w, l, c)	((w = resize_window(w, l, c)) ? OK : ERR)

/* Macros mainly for those standard functions that are just wrappers for 
   the core functions. Defining NOMACROS saves space in the compiled 
   executable, but could slow it down slightly. It's also better for 
   debugging. */

#if !defined(NOMACROS) && !defined(PDCDEBUG)

#define COLOR_PAIR(n)		(((chtype)(n) << PDC_COLOR_SHIFT) & A_COLOR)
#define PAIR_NUMBER(n)		(((n) & A_COLOR) >> PDC_COLOR_SHIFT)

# define addch(c)		waddch(stdscr, c)
# define addchstr(c)		addchnstr(c, -1)
# define addstr(str)		waddstr(stdscr, str)
# define addnstr(str, n)	waddnstr(stdscr, str, n)
# define attroff(attr)		wattroff(stdscr, attr)
# define attron(attr)		wattron(stdscr, attr)
# define attrset(attr)		wattrset(stdscr, attr)
# define bkgd(c)		wbkgd(stdscr, c)
# define bkgdset(c)		wbkgdset(stdscr, c)
# define border(ls,rs,ts,bs,tl,tr,bl,br) wborder(stdscr,ls,rs,ts,bs,tl,tr,bl,br)
# define box(w, v, h)		wborder(w, v, v, h, h, 0, 0, 0, 0)
# define clear()		wclear(stdscr)
# define clrtobot()		wclrtobot(stdscr)
# define clrtoeol()		wclrtoeol(stdscr)
# define color_set(a, b)	wcolor_set(stdscr, a, b)
# define delay_output		napms
# define delch()		wdelch(stdscr)
# define deleteln()		wdeleteln(stdscr)
# define derwin(w,nl,nc,by,bx)	subwin((w), (nl), (nc),\
					(by + (w)->_begy), (bx + (w)->_begx))
# define echochar(c)		(addch((chtype)c)==ERR?ERR:refresh())
# define erase()		werase(stdscr)
# define getbkgd(w)		((w)->_bkgd)
# define getstr(str)		wgetstr(stdscr, str)
# define getnstr(str, num)	wgetnstr(stdscr, str, num)
# define has_colors()           (SP->mono ? FALSE : TRUE)
# define inch()			(stdscr->_y[stdscr->_cury][stdscr->_curx])
# define inchstr(c)		inchnstr(c, stdscr->_maxx - stdscr->_curx)
# define innstr(str, n)		winnstr(stdscr, str, n)
# define insch(c)		winsch(stdscr, c)
# define insdelln(n)		winsdelln(stdscr, n)
# define insertln()		winsertln(stdscr)
# define insnstr(s, n)		winsnstr(stdscr, s, n)
# define insstr(s)		winsnstr(stdscr, s, -1)
# define instr(str)		winnstr(stdscr, str, stdscr->_maxx)
# define isendwin()		(SP->alive ? FALSE : TRUE)
# define mvaddch(y, x, c)	(move(y, x)==ERR?ERR:addch(c))
# define mvaddchstr(y, x, c)	(move(y, x)==ERR?ERR:addchnstr(c, -1))
# define mvaddchnstr(y,x,c,n)	(move(y, x)==ERR?ERR:addchnstr(c, n))
# define mvaddstr(y, x, str)	(move(y, x)==ERR?ERR:addstr(str))
# define mvaddnstr(y,x,str,n)	(move(y, x)==ERR?ERR:addnstr(str, n))
# define mvdelch(y, x)		(move(y, x)==ERR?ERR:wdelch(stdscr))
# define mvgetch(y, x)		(move(y, x)==ERR?ERR:wgetch(stdscr))
# define mvgetstr(y, x, str)	(move(y, x)==ERR?ERR:wgetstr(stdscr, str))
# define mvhline(y, x, c, n)	(move(y, x)==ERR?ERR:hline(c, n))
# define mvinch(y, x)		(move(y, x)==ERR?((chtype)ERR):\
					(stdscr->_y[y][x]))
# define mvinchstr(y, x, c)	(move(y, x)==ERR?ERR:inchnstr(c,\
					stdscr->_maxx - stdscr->_curx))
# define mvinchnstr(y, x, c, n)	(move(y, x)==ERR?ERR:inchnstr(c, n))
# define mvinsch(y,x,c)		(move(y, x)==ERR?ERR:winsch(stdscr, c))
# define mvinsnstr(y, x, s, n)	(move(y, x)==ERR?ERR:winsnstr(stdscr, s, n))
# define mvinsstr(y, x, s)	(move(y, x)==ERR?ERR:winsnstr(stdscr, s, -1))
# define mvinstr(y, x, str)	(move(y, x)==ERR?ERR:winnstr(stdscr,\
					str, stdscr->_maxx))
# define mvinnstr(y, x, str, n)	(move(y, x)==ERR?ERR:winnstr(stdscr, str, n))
# define mvvline(y, x, c, n)	(move(y, x)==ERR?ERR:vline(c, n))
# define mvwaddch(w, y, x, c)	(wmove(w, y, x)==ERR?ERR:waddch(w, c))
# define mvwaddchstr(w,y,x,c)	(wmove(w, y, x)==ERR?ERR:waddchnstr(w, c, -1))
# define mvwaddchnstr(w,y,x,c,n) (wmove(w, y, x)==ERR?ERR:waddchnstr(w, c, n))
# define mvwaddstr(w,y,x,str)	(wmove(w, y, x)==ERR?ERR:waddstr(w, str))
# define mvwdelch(w, y, x)	(wmove(w, y, x)==ERR?ERR:wdelch(w))
# define mvwgetch(w, y, x)	(wmove(w, y, x)==ERR?ERR:wgetch(w))
# define mvwgetstr(w,y,x,str)	(wmove(w, y, x)==ERR?ERR:wgetstr(w, str))
# define mvwgetnstr(w,y,x,str,n) (wmove(w, y, x)==ERR?ERR:wgetnstr(w, str, n))
# define mvwhline(w,y,x,c,n)	(wmove(w, y, x)==ERR?ERR:whline(w, c, n))
# define mvwinch(w, y, x)	(wmove(w, y, x)==ERR?(chtype)ERR:(w)->_y[y][x])
# define mvwinchstr(w, y, x, c)	(wmove(w, y, x)==ERR?ERR:winchnstr(w,\
					c, (w)->_maxx - (w)->_curx))
# define mvwinchnstr(w,y,x,c,n)	(wmove(w, y, x)==ERR?ERR:winchnstr(w, c, n))
# define mvwinsch(w, y, x, c)	(wmove(w, y, x)==ERR?ERR:winsch(w, c))
# define mvwinstr(w, y, x, str)	(wmove(w, y, x)==ERR?ERR:winnstr(w,\
					str, (w)->_maxx))
# define mvwinnstr(w,y,x,str,n)	(wmove(w, y, x)==ERR?ERR:winnstr(w, str, n))
# define mvwinsnstr(w,y,x,s,n)	(wmove(w, y, x)==ERR?ERR:winsnstr(w, s, n))
# define mvwinsstr(w, y, x, s)	(wmove(w, y, x)==ERR?ERR:winsnstr(w, s, -1))
# define mvwvline(w,y,x,c,n)	(wmove(w, y, x)==ERR?ERR:wvline(w, c, n))
# define redrawwin(w)		wredrawln(w, 0, (w)->_maxy)
# define refresh()		wrefresh(stdscr)
# define scrl(n)		wscrl(stdscr, n)
# define scroll(w)		wscrl(w, 1)
# define scrollok(w, flag)	((w)->_scroll = flag)
# define setscrreg(top, bot)	wsetscrreg(stdscr, top, bot)
# define standend()		wattrset(stdscr, A_NORMAL)
# define standout()		wattrset(stdscr, A_STANDOUT)
# define timeout(n)		wtimeout(stdscr, n)
# define vw_printw		vwprintw
# define vw_scanw		vwscanw
# define waddch(w, c)		PDC_chadd(w, (chtype)c,\
					(bool)!(SP->raw_out), TRUE)
# define waddchstr(w, c)	waddchnstr(w, c, -1)
# define werase(w)		(wmove(w, 0, 0), wclrtobot(w))
# define wclear(w)		((w)->_clear = TRUE, werase(w))
# define wechochar(w, c)	(waddch(w, (chtype)c)==ERR?ERR:wrefresh(w))
# define winch(w)		((w)->_y[(w)->_cury][(w)->_curx])
# define winchstr(w, c)		winchnstr(w, c, (w)->_maxx - (w)->_curx)
# define winsstr(w, str)	winsnstr(w, str, -1)
# define winstr(w, str)		winnstr(w, str, (w)->_maxx)
# define wstandend(w)		wattrset(w, A_NORMAL)
# define wstandout(w)		wattrset(w, A_STANDOUT)

#endif /* !defined(MACROS) && !defined(PDCDEBUG) */

/* Some quasi-standard aliases for standard functions */

#define crmode			cbreak
#define nocrmode		nocbreak
#define draino			napms
#define resetterm		reset_shell_mode
#define fixterm			reset_prog_mode
#define saveterm		def_prog_mode

/* PDCurses-specific */

#define is_termresized()	(SP->resized)
#define waddrawch(w, c)		PDC_chadd(w, (chtype)c, FALSE, TRUE)
#define winsrawch(w, c)		PDC_chins(w, (chtype)c, FALSE)
#define addrawch(c)		waddrawch(stdscr, c)
#define insrawch(c)		winsrawch(stdscr, c)
#define mvwaddrawch(w,y,x,c)	(wmove(w, y, x)==ERR?ERR:waddrawch(w, c))
#define PDC_save_key_modifiers(flag) (SP->save_key_modifiers = flag)
#define PDC_return_key_modifiers(flag) (SP->return_key_modifiers = flag)

/* return codes from PDC_getclipboard() and PDC_setclipboard() calls */

enum
{
	PDC_CLIP_SUCCESS, PDC_CLIP_ACCESS_ERROR,
	PDC_CLIP_EMPTY, PDC_CLIP_MEMORY_ERROR
};

/* PDCurses key modifier masks */

#define PDC_KEY_MODIFIER_SHIFT		1
#define PDC_KEY_MODIFIER_CONTROL	2
#define PDC_KEY_MODIFIER_ALT		4
#define PDC_KEY_MODIFIER_NUMLOCK	8

/* Load up curspriv.h. We allow anyone who defines CURSES_LIBRARY
   to have access to our internal routines. */

#ifdef CURSES_LIBRARY
# include <curspriv.h>
#endif

#endif  /* __PDCURSES__ */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
# undef bool
}
#endif
