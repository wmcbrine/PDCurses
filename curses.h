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

/* $Id: curses.h,v 1.167 2006/04/24 20:59:11 wmcbrine Exp $ */

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

#define KEY_BREAK	0x101	/* Not on PC KBD		*/
#define KEY_DOWN	0x102	/* Down arrow key		*/
#define KEY_UP		0x103	/* Up arrow key			*/
#define KEY_LEFT	0x104	/* Left arrow key		*/
#define KEY_RIGHT	0x105	/* Right arrow key		*/
#define KEY_HOME	0x106	/* home key			*/
#define KEY_BACKSPACE	0x107	/* not on pc			*/
#define KEY_F0		0x108	/* function keys; 64 reserved	*/

#define KEY_DL		0x148	/* delete line			*/
#define KEY_IL		0x149	/* insert line			*/
#define KEY_DC		0x14a	/* delete character		*/
#define KEY_IC		0x14b	/* insert char or enter ins mode */
#define KEY_EIC		0x14c	/* exit insert char mode	*/
#define KEY_CLEAR	0x14d	/* clear screen			*/
#define KEY_EOS		0x14e	/* clear to end of screen	*/
#define KEY_EOL		0x14f	/* clear to end of line		*/
#define KEY_SF		0x150	/* scroll 1 line forward	*/
#define KEY_SR		0x151	/* scroll 1 line back (reverse)	*/
#define KEY_NPAGE	0x152	/* next page			*/
#define KEY_PPAGE	0x153	/* previous page		*/
#define KEY_STAB	0x154	/* set tab			*/
#define KEY_CTAB	0x155	/* clear tab			*/
#define KEY_CATAB	0x156	/* clear all tabs		*/
#define KEY_ENTER	0x157	/* enter or send (unreliable)	*/
#define KEY_SRESET	0x158	/* soft/reset (partial/unreliable) */
#define KEY_RESET	0x159	/* reset/hard reset (unreliable) */
#define KEY_PRINT	0x15a	/* print/copy			*/
#define KEY_LL		0x15b	/* home down/bottom (lower left) */
#define KEY_ABORT	0x15c	/* abort/terminate key (any)	*/
#define KEY_SHELP	0x15d	/* short help			*/
#define KEY_LHELP	0x15e	/* long help			*/
#define KEY_BTAB	0x15f	/* Back tab key			*/
#define KEY_BEG		0x160	/* beg(inning) key		*/
#define KEY_CANCEL	0x161	/* cancel key			*/
#define KEY_CLOSE	0x162	/* close key			*/
#define KEY_COMMAND	0x163	/* cmd (command) key		*/
#define KEY_COPY	0x164	/* copy key			*/
#define KEY_CREATE	0x165	/* create key			*/
#define KEY_END		0x166	/* end key			*/
#define KEY_EXIT	0x167	/* exit key			*/
#define KEY_FIND	0x168	/* find key			*/
#define KEY_HELP	0x169	/* help key			*/
#define KEY_MARK	0x16a	/* mark key			*/
#define KEY_MESSAGE	0x16b	/* message key			*/
#define KEY_MOVE	0x16c	/* move key			*/
#define KEY_NEXT	0x16d	/* next object key		*/
#define KEY_OPEN	0x16e	/* open key			*/
#define KEY_OPTIONS	0x16f	/* options key			*/
#define KEY_PREVIOUS	0x170	/* previous object key		*/
#define KEY_REDO	0x171	/* redo key			*/
#define KEY_REFERENCE	0x172	/* ref(erence) key		*/
#define KEY_REFRESH	0x173	/* refresh key			*/
#define KEY_REPLACE	0x174	/* replace key			*/
#define KEY_RESTART	0x175	/* restart key			*/
#define KEY_RESUME	0x176	/* resume key			*/
#define KEY_SAVE	0x177	/* save key			*/
#define KEY_SBEG	0x178	/* shifted beginning key	*/
#define KEY_SCANCEL	0x179	/* shifted cancel key		*/
#define KEY_SCOMMAND	0x17a	/* shifted command key		*/
#define KEY_SCOPY	0x17b	/* shifted copy key		*/
#define KEY_SCREATE	0x17c	/* shifted create key		*/
#define KEY_SDC		0x17d	/* shifted delete char key	*/
#define KEY_SDL		0x17e	/* shifted delete line key	*/
#define KEY_SELECT	0x17f	/* select key			*/
#define KEY_SEND	0x180	/* shifted end key		*/
#define KEY_SEOL	0x181	/* shifted clear line key	*/
#define KEY_SEXIT	0x182	/* shifted exit key		*/
#define KEY_SFIND	0x183	/* shifted find key		*/
#define KEY_SHOME	0x184	/* shifted home key		*/
#define KEY_SIC		0x185	/* shifted input key		*/

#define KEY_SLEFT	0x187	/* shifted left arrow key	*/
#define KEY_SMESSAGE	0x188	/* shifted message key		*/
#define KEY_SMOVE	0x189	/* shifted move key		*/
#define KEY_SNEXT	0x18a	/* shifted next key		*/
#define KEY_SOPTIONS	0x18b	/* shifted options key		*/
#define KEY_SPREVIOUS	0x18c	/* shifted prev key		*/
#define KEY_SPRINT	0x18d	/* shifted print key		*/
#define KEY_SREDO	0x18e	/* shifted redo key		*/
#define KEY_SREPLACE	0x18f	/* shifted replace key		*/
#define KEY_SRIGHT	0x190	/* shifted right arrow		*/
#define KEY_SRSUME	0x191	/* shifted resume key		*/
#define KEY_SSAVE	0x192	/* shifted save key		*/
#define KEY_SSUSPEND	0x193	/* shifted suspend key		*/
#define KEY_SUNDO	0x194	/* shifted undo key		*/
#define KEY_SUSPEND	0x195	/* suspend key			*/
#define KEY_UNDO	0x196	/* undo key			*/

/* PDCurses-specific key definitions -- PC only */

#define ALT_0		0x197
#define ALT_1		0x198
#define ALT_2		0x199
#define ALT_3		0x19a
#define ALT_4		0x19b
#define ALT_5		0x19c
#define ALT_6		0x19d
#define ALT_7		0x19e
#define ALT_8		0x19f
#define ALT_9		0x1a0
#define ALT_A		0x1a1
#define ALT_B		0x1a2
#define ALT_C		0x1a3
#define ALT_D		0x1a4
#define ALT_E		0x1a5
#define ALT_F		0x1a6
#define ALT_G		0x1a7
#define ALT_H		0x1a8
#define ALT_I		0x1a9
#define ALT_J		0x1aa
#define ALT_K		0x1ab
#define ALT_L		0x1ac
#define ALT_M		0x1ad
#define ALT_N		0x1ae
#define ALT_O		0x1af
#define ALT_P		0x1b0
#define ALT_Q		0x1b1
#define ALT_R		0x1b2
#define ALT_S		0x1b3
#define ALT_T		0x1b4
#define ALT_U		0x1b5
#define ALT_V		0x1b6
#define ALT_W		0x1b7
#define ALT_X		0x1b8
#define ALT_Y		0x1b9
#define ALT_Z		0x1ba

#define CTL_LEFT	0x1bb	/* Control-Left-Arrow	*/
#define CTL_RIGHT	0x1bc
#define CTL_PGUP	0x1bd
#define CTL_PGDN	0x1be
#define CTL_HOME	0x1bf
#define CTL_END		0x1c0

#define KEY_A1		0x1c1	/* upper left on Virtual keypad	*/
#define KEY_A2		0x1c2	/* upper middle on Virt. keypad	*/
#define KEY_A3		0x1c3	/* upper right on Vir. keypad	*/
#define KEY_B1		0x1c4	/* middle left on Virt. keypad	*/
#define KEY_B2		0x1c5	/* center on Virt. keypad	*/
#define KEY_B3		0x1c6	/* middle right on Vir. keypad	*/
#define KEY_C1		0x1c7	/* lower left on Virt. keypad	*/
#define KEY_C2		0x1c8	/* lower middle on Virt. keypad */
#define KEY_C3		0x1c9	/* lower right on Vir. keypad	*/

#define PADSLASH	0x1ca	/* slash on keypad		*/
#define PADENTER	0x1cb	/* enter on keypad		*/
#define CTL_PADENTER	0x1cc	/* ctl-enter on keypad		*/
#define ALT_PADENTER	0x1cd	/* alt-enter on keypad		*/
#define PADSTOP		0x1ce	/* stop on keypad		*/
#define PADSTAR		0x1cf	/* star on keypad		*/
#define PADMINUS	0x1d0	/* minus on keypad		*/
#define PADPLUS		0x1d1	/* plus on keypad		*/
#define CTL_PADSTOP	0x1d2	/* ctl-stop on keypad		*/
#define CTL_PADCENTER	0x1d3	/* ctl-enter on keypad		*/
#define CTL_PADPLUS	0x1d4	/* ctl-plus on keypad		*/
#define CTL_PADMINUS	0x1d5	/* ctl-minus on keypad		*/
#define CTL_PADSLASH	0x1d6	/* ctl-slash on keypad		*/
#define CTL_PADSTAR	0x1d7	/* ctl-star on keypad		*/
#define ALT_PADPLUS	0x1d8	/* alt-plus on keypad		*/
#define ALT_PADMINUS	0x1d9	/* alt-minus on keypad		*/
#define ALT_PADSLASH	0x1da	/* alt-slash on keypad		*/
#define ALT_PADSTAR	0x1db	/* alt-star on keypad		*/
#define ALT_PADSTOP	0x1dc	/* alt-stop on keypad		*/
#define CTL_INS		0x1dd	/* ctl-insert			*/
#define ALT_DEL		0x1de	/* alt-delete			*/
#define ALT_INS		0x1df	/* alt-insert			*/
#define CTL_UP		0x1e0	/* ctl-up arrow			*/
#define CTL_DOWN	0x1e1	/* ctl-down arrow		*/
#define CTL_TAB		0x1e2	/* ctl-tab			*/
#define ALT_TAB		0x1e3
#define ALT_MINUS	0x1e4
#define ALT_EQUAL	0x1e5
#define ALT_HOME	0x1e6
#define ALT_PGUP	0x1e7
#define ALT_PGDN	0x1e8
#define ALT_END		0x1e9
#define ALT_UP		0x1ea	/* alt-up arrow			*/
#define ALT_DOWN	0x1eb	/* alt-down arrow		*/
#define ALT_RIGHT	0x1ec	/* alt-right arrow		*/
#define ALT_LEFT	0x1ed	/* alt-left arrow		*/
#define ALT_ENTER	0x1ee	/* alt-enter			*/
#define ALT_ESC		0x1ef	/* alt-escape			*/
#define ALT_BQUOTE	0x1f0	/* alt-back quote		*/
#define ALT_LBRACKET	0x1f1	/* alt-left bracket		*/
#define ALT_RBRACKET	0x1f2	/* alt-right bracket		*/
#define ALT_SEMICOLON	0x1f3	/* alt-semi-colon		*/
#define ALT_FQUOTE	0x1f4	/* alt-forward quote		*/
#define ALT_COMMA	0x1f5	/* alt-comma			*/
#define ALT_STOP	0x1f6	/* alt-stop			*/
#define ALT_FSLASH	0x1f7	/* alt-forward slash		*/
#define ALT_BKSP	0x1f8	/* alt-backspace		*/
#define CTL_BKSP	0x1f9	/* ctl-backspace		*/
#define PAD0		0x1fa	/* keypad 0			*/

#define CTL_PAD0	0x1fb	/* ctl-keypad 0			*/
#define CTL_PAD1	0x1fc
#define CTL_PAD2	0x1fd
#define CTL_PAD3	0x1fe
#define CTL_PAD4	0x1ff
#define CTL_PAD5	0x200
#define CTL_PAD6	0x201
#define CTL_PAD7	0x202
#define CTL_PAD8	0x203
#define CTL_PAD9	0x204

#define ALT_PAD0	0x205	/* alt-keypad 0			*/
#define ALT_PAD1	0x206
#define ALT_PAD2	0x207
#define ALT_PAD3	0x208
#define ALT_PAD4	0x209
#define ALT_PAD5	0x20a
#define ALT_PAD6	0x20b
#define ALT_PAD7	0x20c
#define ALT_PAD8	0x20d
#define ALT_PAD9	0x20e

#define CTL_DEL		0x20f	/* clt-delete			*/
#define ALT_BSLASH	0x210	/* alt-back slash		*/
#define CTL_ENTER	0x211	/* ctl-enter			*/

#define SHF_PADENTER	0x212	/* shift-enter on keypad	*/
#define SHF_PADSLASH	0x213	/* shift-slash on keypad	*/
#define SHF_PADSTAR	0x214	/* shift-star  on keypad	*/
#define SHF_PADPLUS	0x215	/* shift-plus  on keypad	*/
#define SHF_PADMINUS	0x216	/* shift-minus on keypad	*/
#define SHF_UP		0x217	/* shift-up on keypad		*/
#define SHF_DOWN	0x218	/* shift-down on keypad		*/
#define SHF_IC		0x219	/* shift-insert on keypad	*/
#define SHF_DC		0x21a	/* shift-delete on keypad	*/

#define KEY_MOUSE	0x21b	/* "mouse" key			*/
#define KEY_SHIFT_L	0x21c	/* Left-shift			*/
#define KEY_SHIFT_R	0x21d	/* Right-shift			*/
#define KEY_CONTROL_L	0x21e	/* Left-control			*/
#define KEY_CONTROL_R	0x21f	/* Right-control		*/
#define KEY_ALT_L	0x220	/* Left-alt			*/
#define KEY_ALT_R	0x221	/* Right-alt			*/
#define KEY_RESIZE	0x222	/* Window resize		*/
#define KEY_SUP		0x223	/* Shifted up arrow		*/
#define KEY_SDOWN	0x224	/* Shifted down arrow		*/

#define KEY_MIN         KEY_BREAK       /* Minimum curses key value     */
#define KEY_MAX         KEY_SDOWN       /* Maximum curses key           */

#define KEY_F(n)        (KEY_F0 + (n))

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
