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
/* 
$Id: curses.h,v 1.7 2001/10/16 10:33:06 mark Exp $
*/
/*
*----------------------------------------------------------------------
*				PDCurses
*----------------------------------------------------------------------
* MH
*	991212	2.4
*		Key modifiers pressed by themselves are now returned as keys
*			KEY_SHIFT_L KEY_SHIFT_R 
*			KEY_CONTROL_L KEY_CONTROL_R
*			KEY_ALT_L KEY_ALT_R
*			This works on Win32 and X11 ports only
*		Added support for lcc-win32, supplied by Matthias Burian
*		Added support for MingW32, and CygnusW32 compilers
*		Added clipboard support for X11, Win32 and OS/2 ports
*		Added ability to get to the input file descriptor
*		New function for X11; Xinitscr() which allows standard X11
*			switches to be passed to the application
*		Added X11 shared library support
*
* MH
*	960920	2.3	Added more System V R4 functions
*
*			Rewrote X11 port
*
*			Added Win32 port - original port by Chris Szurgot
*				some updates by Mark Hessling
*				some updates by Gurusamy Sarathy
*
*			slk() functions added by Kim Huron
*
*			Support for more and newer versions of compilers
*
*			Added mouse support for X11 and Win32 ports
*
*			Size of chtype now long
*
* MH
*	950126	2.2	Added more System V R4 functions
*
*			Added beta Xwindows port
*
*			Changed chtype to long
*
*			Incorporated panels library
*
*			Support for more and newer versions of compilers
*
* MH
*	930531	2.1	Added support for djgpp
*
*			Added beta Unix version
*
*			Added OS/2 DLL support.
*
*			Changed behaviour of overlay(), overwrite() and typeahead()
*
*	921120	2.0	Changed #if to #ifdef/#if defined to make it
*			easier to add new platforms/compilers.
*
*			Added System V colour support.
*
*			Added OS/2 port.
*-------
* Frotz
*	911221	2.0 pre-beta	Changed back from short to int. (int is the
*			correct size for the default platform.  Short
*			might be too short on some platforms.  This
*			is more portable.  I, also, made this mistake.)
*
*			Many functions are now macros.  If you want
*			the real thing, #undef the macro. (X/Open
*			requirement.)
*
*			Merged many sources into current release.
*
*			Added many X/Open routines (not quite all yet).
*
*			Added internal documentation to all routines.
*
*			Added a HISTORY file to the environment.
*
*			Added a CONTRIB file to the environment.
*-------
* bl	900114	1.4	Window origin mod in overlay() and overwrite(), on
*			public (and very reasonable) request. Swapped
*			#define'd values of OK and ERR; OK now 1, and
*			ERR is 0/NULL. Conforms better to UNIX
*			versions.  borderchars[] removed from WINDOW
*			struct since the border() functions were
*			redefined. Use of short wherever possible.
*			Portability improvements, mispelled name of
*			[w]setscrreg().
*
*	881005	1.3	All modules lint-checked with MSC '-W3' and
*			turbo'C' '-w -w-pro' switches. Support for
*			border(), wborder() functions.
*
*	881002	1.2	Rcsid[] string in all modules, for maintenance.
*
*	880306	1.1	'Raw' output routines, revision info in curses.h.
*
*	870515	1.0	Initial Release.
*
*----------------------------------------------------------------------
*/
#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
 extern "C" 
   {
# define bool _bool
#endif

#ifndef  __PDCURSES__
#define	__PDCURSES__ 1

/*man-start*********************************************************************

All defines are "defined" here.  All compiler and environment
specific definitions are defined into generic class defines.
These defines are to be given values so that the code can
rely on #if, rather than a complicated set of #if defined() or
#ifdefs...

PDCurses definitions list:  (Only define those needed)

	REGISTERWINDOWS True for auto window update registery.
	FAST_VIDEO      True if display is memory mapped, or
	                we can utilize the fast video update routines.
	DOS             True if compiling for DOS.
	OS2             True if compiling for OS/2.
	WIN32           True if compiling for Windoze 95 or Windoze NT
	FLEXOS          True if compiling for Flexos.
	HC              True if using a Metaware compiler.
	TC              True if using a Borland compiler.
	MSC             True if using a Microsoft compiler.
	HAVE_PROTO      True if the compiler supports ANSI prototypes.
	CPLUSPLUS       True if the compiler supports C++.
	PDC_BUILD????   Defines API build version.

PDCurses portable platform definitions list:

	PDCURSES        Enables access to PDCurses-only routines.
	XOPEN           Always true.
	SYSVcurses      True if you are compiling for SYSV portability.
	BSDcurses       True if you are compiling for BSD portability.
	PDCURSES_WCLR   Makes behaviour of wclrtoeol() and wclrtoeof()
	                unique to PDCurses. By default Unix behavior is set.
	                See notes in wclrtoeol() and wclrtoeof().
**man-end**********************************************************************/

#define PDC_BUILD	2503
#define	PDCURSES	1	/* PDCurses-only routines	*/
#define	XOPEN		1	/* X/Open Curses routines	*/
#define	SYSVcurses	1	/* System V Curses routines	*/
#define	BSDcurses	1	/* BSD Curses routines		*/
#define	CHTYPE_LONG	1	/* size of chtype; long		*/

#define	PDC_CDECL		/* default define this as empty */

/*----------------------------------------
*       BORLAND COMPILERS       Turbo C[++], Borland C[++]
*
*       Borland definitions:
*               TC
*               DOS
*               OS2
*               CPLUSPLUS
*
*               __TURBOC__, __MSDOS__, 
*               __OS2__ and __WIN32__
*               are predefined by compiler.
*/
#ifdef __TURBOC__              /* Borland gives defines this as a value*/
#  define TC    __TURBOC__     /* Define a value for TC                */
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1       /* Borland supports ANSI C prototypes   */
#  endif
#  ifdef __MSDOS__
#    define DOS 6              /* Major release of DOS supported       */
#    include <bios.h>
#    include <dos.h>
#  endif
#  ifdef __OS2__
#    define OS2 3              /* Major release of OS/2 supported      */
#    define INCL_VIO
#    define INCL_KBD
#    include <os2.h>
#  endif
#  ifdef __WIN32__
#    ifdef INCLUDE_WINDOWS_H           /* only include for WIN32 files */
#      include <windows.h>
#      ifdef MOUSE_MOVED                 /* get rid of Windows #define */
#        undef MOUSE_MOVED
#      endif
#    endif
#    ifndef WIN32
#      define WIN32 1
#    endif
#  endif
#  if __TURBOC__ >= 0x290
#    ifdef  __BCPLUSPLUS__
#      define CPLUSPLUS   1    /* Define that we are compiling C++.    */
#    endif
#    ifdef  __TCPLUSPLUS__
#      define CPLUSPLUS   1    /* Define that we are compiling C++.    */
#    endif
#    ifdef  __CPLUSPLUS__
#      define CPLUSPLUS   1    /* Define that we are compiling C++.    */
#    endif
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF                             /* have vsscanf() */
#  endif
#endif


/*----------------------------------------
*       METAWARE COMPILERS      High C
*
*       Metaware definitions:
*               HC
*               DOS
*               FLEXOS
*/
#ifdef __HIGHC__
#  define HC    1
#  pragma off(prototype_override_warnings)
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1      /* Metaware supports ANSI C prototypes   */
#  endif
#  ifdef __MSDOS__
#    define DOS 6              /* Major release of DOS supported       */
#    include <bios.h>
#    include <dos.h>
#  endif
#  ifdef __FLEXOS__            /* define this on the command line      */
#    define FLEXOS 2           /* or latest major release value.       */
#  endif
#endif

/*----------------------------------------
*       MICROSOFT COMPILERS      MSC
*
*       Microsoft definitions:
*               MSC
*               DOS || OS2
*/
#ifdef _MSC_VER                                 /* defined by compiler */
#  define MSC   1
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1     /* Microsoft supports ANSI C prototypes   */
#  endif
#  ifdef __OS2__                /* You will have to define in makefile */
#    define USE_OS2_H 1             /* Use the os2.h for the compiler  */
#    define OS2 3                   /* Major release of OS/2 supported */
#    define INCL_VIO
#    define INCL_KBD
#    include <os2.h>
#    define FARKeyword far
#    define APIRET USHORT
#  else                            /* no __OS2__ define, so assume DOS */
#    ifdef _WIN32	
#      ifndef WIN32
#        define WIN32 
#      endif
#      ifdef INCLUDE_WINDOWS_H         /* only include for WIN32 files */
#        include <windows.h>
#        ifdef MOUSE_MOVED               /* get rid of Windows #define */
#          undef MOUSE_MOVED
#        endif
#      endif
#      ifndef HAVE_MEMORY_H
#        define HAVE_MEMORY_H                       /* have <memory.h> */
#      endif
#      ifndef HAVE_STDARG_H
#        define HAVE_STDARG_H                       /* have <stdarg.h> */
#      endif
#    else	
#      define DOS 6                  /* Major release of DOS supported */
#      include <bios.h>
#      include <dos.h>
#      undef  HAVE_VSSCANF        /* vsscanf() function NOT in library */
#    endif
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#endif

/*----------------------------------------
*       MICROSOFT QUICK C COMPILERS      QC
*
*/
#ifdef _QC                                      /* defined by compiler */
#  define MSC   1
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1        /* QuickC supports ANSI C prototypes   */
#  endif
#  define DOS 6                      /* Major release of DOS supported */
#  include <bios.h>
#  include <dos.h>
#  undef  HAVE_VSSCANF            /* vsscanf() function NOT in library */
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#endif

/*----------------------------------------
*       TOPSPEED compilers     TSC
*
*       TOPSPEED definitions:
*               TSC
*               DOS || OS2
*/
#ifdef __TSC__                   /* You may have to define in makefile */
#  define TSC   1
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1      /* TopSpeed supports ANSI C prototypes   */
#  endif
#  ifdef __OS2__
#    define OS2 3                   /* Major release of OS/2 supported */
#    define INCL_VIO
#    define INCL_KBD
#    include <os2.h>
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#endif

/*----------------------------------------
*       IBM C Set/2 Compiler   CSET2
*
*       IBM definitions:
*               CSET2
*               OS2
*/
#ifdef __IBMC__
#  define CSET2 1
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1        /* C Set/2 supports ANSI C prototypes  */
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifdef __OS2__
#    define OS2 3                   /* Major release of OS/2 supported */
#    define INCL_VIO
#    define INCL_KBD
#    include <os2.h>
#  endif
#endif

/*----------------------------------------
*       GNU compilers     emx
*
*       emx definitions:
*               EMX
*               OS2
*/
#ifdef __EMX__                   /* You may have to define in makefile */
#  define EMX   1
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1            /* emx supports ANSI C prototypes  */
#  endif
#  define __OS2__                      /* EMX does not define this :-( */
#  define OS2 3                     /* Major release of OS/2 supported */
#  define CURSES__32BIT__
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H                           /* have <unistd.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF                             /* have vsscanf() */
#  endif
#  if defined(EMXVIDEO)
#    include <stdlib.h>
#    include <sys/video.h>
#  else
#    define INCL_VIO
#    define INCL_KBD
#    include <os2.h>
#  endif
#endif

/*----------------------------------------
*       GNU compilers     djgpp
*
*		djgpp definitions:
*				GO32
*				DOS
*/
#ifdef GO32 				  /* You may have to define in makefile */
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1         /* DJGPP supports ANSI C prototypes   */
#  endif
#  define DOS	6
#  include <dos.h>
#  ifdef __FLEXOS__ 		   /* define this on the command line	   */
#    define FLEXOS 2           /* or latest major release value.       */
#  endif
#  define CURSES__32BIT__
#  ifdef _cplusplus
#    define CPLUSPLUS 1
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H                           /* have <unistd.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifndef HAVE_USLEEP
#    define HAVE_USLEEP                               /* have usleep() */
#  endif
#endif


/*----------------------------------------
*       GNU compilers     Cygnus Win32
*
*		cygnus definitions:
*				WIN32
*/
#ifdef __CYGWIN32__  /* You may have to define in makefile */
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1    /* Cygnuc GCC supports ANSI C prototypes   */
#  endif
#  ifdef INCLUDE_WINDOWS_H
#    include <windows.h>
#    ifdef MOUSE_MOVED
#      undef MOUSE_MOVED
#    endif
#  endif
#  ifndef WIN32
#    define WIN32
#  endif
#  ifndef INT_MAX
#    define INT_MAX 32762
#  endif
#  define CURSES__32BIT__
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H                           /* have <unistd.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifndef HAVE_USLEEP
#    define HAVE_USLEEP                               /* have usleep() */
#  endif
#endif

/*----------------------------------------
*       GNU compilers     Ming Win32
*
*		Ming definitions:
*				WIN32
*/
#ifdef __MINGW32__
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1    /* Ming GCC supports ANSI C prototypes   */
#  endif
#  ifdef INCLUDE_WINDOWS_H
#    include <windows.h>
#    ifdef MOUSE_MOVED
#      undef MOUSE_MOVED
#    endif
#  endif
#  ifndef WIN32
#    define WIN32
#  endif
#  define CURSES__32BIT__
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H                           /* have <unistd.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#endif

/*----------------------------------------
*       LCC WIN32
*
*/
#ifdef __LCC__  /* should already be defined by the compiler */
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1         /* LCC-Win32 supports ANSI C prototypes   */
#  endif
#  ifdef INCLUDE_WINDOWS_H
#    include <windows.h>
#    ifdef MOUSE_MOVED
#      undef MOUSE_MOVED
#    endif
#  endif
#  ifndef WIN32
#    define WIN32
#  endif
#  ifndef INT_MAX
#    define INT_MAX 32762
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#endif

/*----------------------------------------
*       Watcom C/C++ 10.6 compiler
*
*       WATCOM definitions:
*               WATCOMC
*               OS2
*               WIN32
*/
#ifdef __WATCOMC__
#  define WATCOMC  1
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1      /* Watcom C supports ANSI C prototypes   */
#  endif
#  define CURSES__32BIT__
#  if defined( __DOS__ ) || defined( __DOS4G__ )
#    define DOS 7                    /* Major release of DOS supported */
#    include <bios.h>
#    include <dos.h>
#    include <i86.h>
#    ifdef __386__
#      define int86 int386
#      define int86x int386x
#    endif
#  endif
#  if defined( __OS2__ ) || defined( __OS2V2__ )
#    define OS2 3                   /* Major release of OS/2 supported */
#    define CURSES__32BIT__
#    define INCL_VIO
#    define INCL_KBD
#    include <os2.h>
#  endif
#  ifdef __NT__                         /* This specifies WIN32 target */
#    ifdef INCLUDE_WINDOWS_H
#      include <windows.h>
#      ifdef MOUSE_MOVED
#        undef MOUSE_MOVED
#      endif
#    endif
#    ifndef WIN32
#      define WIN32
#    endif

/*
#    ifdef PDC_CDECL
#      undef PDC_CDECL
#      define PDC_CDECL cdecl
#    endif
*/

#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_MEMORY_H
#    define HAVE_MEMORY_H                           /* have <memory.h> */
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF                             /* have vsscanf() */
#  endif
#endif

#if 0
/*----------------------------------------
*       gcc under UNIX
*
*       GNU definitions:
*               UNIX
*/
#ifdef UNIX
#  define HAVE_PROTO 1                  /* GNU C supports ANSI C prototypes  */
#  ifdef SUNOS
#    define NO_VSSCANF
#    define NO_MEMMOVE
#    undef BSD
#  endif
#  ifdef linux
#    undef BSD
#  endif
#endif
#endif

/*----------------------------------------
*       MicroWay NDP C/C++ 386 4.2.0 compiler
*/
#ifdef MX386
#  define HAVE_PROTO 1
#  define NDP	1
#  include 	<bios.h>
#  ifdef DOS
#    define MK_FP(seg,ofs)	( (((int) (seg)) << 4) + ((int) (ofs)) )
#    ifdef __i860
       typedef void _int;
#    else
       typedef int      _int;
#    endif
#  endif
#    undef  NO_VSSCANF            /* vsscanf() function NOT in library */
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#endif

/*---------------------------------------------------------------------*/
#include <stdio.h>		/* Required by X/Open usage below	*/
/*----------------------------------------------------------------------
 *
 *	PDCurses Manifest Constants
 *
 */
#ifndef FALSE			/* booleans		 */
#  define	FALSE	0
#endif
#ifndef	TRUE			/* booleans		 */
#  define	TRUE	!FALSE
#endif
#ifndef	NULL
#  define NULL	0	/* Null pointer		 */
#endif
#ifndef	ERR
#  define	 ERR	0		/* general error flag	 */
#endif
#ifndef	OK
#  define	 OK	1		/* general OK flag	 */
#endif


/*----------------------------------------------------------------------
 *
 *	PDCurses Type Declarations
 *
 */
typedef unsigned char bool;	/* PDCurses Boolean type	*/

#ifdef CHTYPE_LONG
typedef unsigned long chtype;	/* 16-bit attr + 16-bit char	*/
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


#define BUTTON_RELEASED        0000
#define BUTTON_PRESSED         0001
#define BUTTON_CLICKED         0002
#define BUTTON_DOUBLE_CLICKED  0003
#define BUTTON_TRIPLE_CLICKED  0004
#define BUTTON_MOVED           0005  /* PDCurses enhancement */
#define BUTTON_ACTION_MASK     0007  /* PDCurses enhancement */
#define BUTTON_SHIFT           0010  /* PDCurses enhancement */
#define BUTTON_CONTROL         0020  /* PDCurses enhancement */
#define BUTTON_ALT             0040  /* PDCurses enhancement */
#define BUTTON_MODIFIER_MASK   0070  /* PDCurses enhancement */

#define MOUSE_X_POS            (Mouse_status.x)
#define MOUSE_Y_POS            (Mouse_status.y)
#define A_BUTTON_CHANGED       (Mouse_status.changes & 7)
#define MOUSE_MOVED            (Mouse_status.changes & 8)
#define MOUSE_POS_REPORT       (Mouse_status.changes & 16)
#define BUTTON_CHANGED(x)      (Mouse_status.changes & (1 << ((x) - 1)))
#define BUTTON_STATUS(x)       (Mouse_status.button[(x)-1])

/* mouse bit-masks */
#define BUTTON1_RELEASED        000000000001L
#define BUTTON1_PRESSED         000000000002L
#define BUTTON1_CLICKED         000000000004L
#define BUTTON1_DOUBLE_CLICKED  000000000010L
#define BUTTON1_TRIPLE_CLICKED  000000000020L
#define BUTTON1_MOVED           000000000020L /* PDCurses enhancement */
#define BUTTON2_RELEASED        000000000040L
#define BUTTON2_PRESSED         000000000100L
#define BUTTON2_CLICKED         000000000200L
#define BUTTON2_DOUBLE_CLICKED  000000000400L
#define BUTTON2_TRIPLE_CLICKED  000000001000L
#define BUTTON2_MOVED           000000001000L /* PDCurses enhancement */
#define BUTTON3_RELEASED        000000002000L
#define BUTTON3_PRESSED         000000004000L
#define BUTTON3_CLICKED         000000010000L
#define BUTTON3_DOUBLE_CLICKED  000000020000L
#define BUTTON3_TRIPLE_CLICKED  000000040000L
#define BUTTON3_MOVED           000000040000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_SHIFT   000000100000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_CONTROL 000000200000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_ALT     000000400000L /* PDCurses enhancement */
#define ALL_MOUSE_EVENTS        000000777777L
#define REPORT_MOUSE_POSITION   000001000000L

/*----------------------------------------------------------------------
 *
 *	PDCurses Structure Definitions:
 *
 */
typedef struct _win		/* definition of a window.	   */
{
	int	_cury;		/* current pseudo-cursor	   */
	int	_curx;
	int	_maxy;		/* max window coordinates	   */
	int	_maxx;
	int	_pmaxy;		/* max physical size		   */
	int	_pmaxx;
	int	_begy;		/* origin on screen		   */
	int	_begx;
	int	_lastpy;	/* last y coordinate of upper left pad display area */
	int	_lastpx;	/* last x coordinate of upper left pad display area */
	int	_lastsy1;	/* last upper y coordinate of screen window for pad */
	int	_lastsx1;	/* last upper x coordinate of screen window for pad */
	int	_lastsy2;	/* last lower y coordinate of screen window for pad */
	int	_lastsx2;	/* last lower x coordinate of screen window for pad */
	int	_flags;		/* window properties		   */
	attr_t	_attrs;		/* standard A_STANDOUT attributes and colors  */
	chtype	_bkgd;		/* wrs(4/6/93) background, normally blank */
	int	_tabsize;	/* tab character size		   */
	bool	_clear;		/* causes clear at next refresh	   */
	bool	_leaveit;		/* leaves cursor where it is	   */
	bool	_scroll;	/* allows window scrolling	   */
	bool	_nodelay;	/* input character wait flag	   */
	bool	_immed;	/* immediate update flag	   */
	bool	_sync;	/* synchronise window ancestors	   */
	bool	_use_keypad;	/* flags keypad key mode active	   */
	bool	_use_idl;	/* True if Ins/Del line can be used*/
	bool	_use_idc;	/* True if Ins/Del character can be used*/
	chtype**_y;		/* pointer to line pointer array   */
	int*	_firstch;	/* first changed character in line */
	int*	_lastch;	/* last changed character in line  */
	int	_tmarg;	/* top of scrolling region	   */
	int	_bmarg;	/* bottom of scrolling region	   */
	char*	_title;		/* window title			   */
	char	_title_ofs;	/* window title offset from left   */
	attr_t	_title_attr;	/* window title attributes	   */
	chtype	_blank;		/* window's blank character	   */
	int	_parx, _pary;	/* coords relative to parent (0,0) */
struct	_win*	_parent;	/* subwin's pointer to parent win  */
}	WINDOW;



/*----------------------------------------------------------------------
*
*	Private structures that are necessary for correct
*	macro construction.
*
*/

#ifdef	REGISTERWINDOWS
typedef struct _ref		/* Refresh Window Structure	 */
{
	WINDOW*	win;
struct	_ref*	next;
struct	_ref*	tail;
}	ACTIVE;

typedef struct _wins
{
	WINDOW*		w;	/* pointer to a visible window	    */
	struct _wins*	next;	/* Next visible window pointer	    */
	struct _wins*	prev;	/* Next visible window pointer	    */
	struct _wins*	tail;	/* Last visible window pointer	    */
				/* Only head window (stdscr) has    */
				/* a valid tail pointer.	    */
}	WINDS;
#endif

typedef struct		/* structure for ripped off lines */
{
	int line;
	int (*init)();
} RIPPEDOFFLINE;


typedef struct
{
	bool	alive;		/* if initscr() called		    */
	bool	autocr;		/* if lf -> crlf		    */
	bool	cbreak;		/* if terminal unbuffered	    */
	bool	echo;		/* if terminal echo		    */
	bool	raw_inp;	/* raw input mode (v. cooked input) */
	bool	raw_out;	/* raw output mode (7 v. 8 bits)    */
	bool	refrbrk;	/* if premature refresh brk allowed */
	bool	orgcbr;		/* original MSDOS ^-BREAK setting   */
	bool	visible_cursor; /* TRUE if cursor is visible	    */
	bool	audible;	/* FALSE if the bell is visual	    */
	bool	full_redraw;	/* TRUE for bad performance	    */
	bool	direct_video;	/* Allow Direct Screen Memory writes*/
	bool	mono;		/* TRUE if current screen is mono.  */
	bool	sizeable;	/* TRUE if adapter is resizeable.   */
	bool	resized;	/* TRUE if TERM has been resized */
	bool	bogus_adapter;	/* TRUE if adapter has insane values*/
	bool	shell;		/* TRUE if reset_prog_mode() needs  */
				/*	to be called.		    */
	chtype	blank;		/* Background character		    */
	attr_t	orig_attr;	/* Original screen attributes	    */
	int	cursrow;	/* position of physical cursor	    */
	int	curscol;	/* position of physical cursor	    */
	int	cursor;		/* Current Cursor definition	    */
	int	visibility;		/* Visibility of cursor	*/
	int	video_page;	/* Current PC video page	    */
	int	orig_emulation; /* Original cursor emulation value  */
	int	orig_cursor;	/* Original cursor size		    */
	int	font;		/* default font size		    */
	int	orig_font;	/* Original font size		    */
	int	lines;		/* New value for LINES		    */
	int	cols;		/* New value for COLS		    */
	unsigned long _trap_mbe;		/* trap these mouse button events */
	unsigned long _map_mbe_to_key;	/* map mouse buttons to slk */
	int	slklines;		/* Lines in use by slk_init() */
	WINDOW *	slk_winptr;		/* window for slk */
	int	linesrippedoff;		/* Lines ripped off via ripoffline() */
	int	linesrippedoffontop;		/* Lines ripped off on top via ripoffline() */
	int	delaytenths;		/* 1/10ths second to wait block getch() for */
	bool	_preserve;		/* TRUE if screen background to be preserved */
	bool	_restore;		/* TRUE if screen background to be restored */
	bool	save_key_modifiers;		/* TRUE if each key modifiers saved with each key press */
	bool	return_key_modifiers;		/* TRUE if modifier keys are returned as "real" keys */

#ifdef OS2
# ifdef EMXVIDEO			/* nop if using EMX builtins */
	int tahead; 			/* Type-ahead value */
	int adapter;			/* Screen type	*/
# else
	VIOMODEINFO scrnmode;	/* default screen mode			*/
	VIOCONFIGINFO adapter;	/* Screen type				*/
# endif
#else
	int adapter;			/* Screen type	*/
#endif

#if defined(DOS) || defined(WIN32)
	int	scrnmode;	/* default screen mode		    */
	unsigned video_seg;	/* video base segment		    */
	unsigned video_ofs;	/* video base offset		    */
#endif

#if defined(OS2) || defined(WIN32)
	int	num_mouse_buttons;	/* number of mouse buttons */
#endif

#if defined (XCURSES)
	int	XcurscrSize; /* size of Xcurscr shared memory block */
	bool	sb_on;
	int	sb_viewport_y;
	int	sb_viewport_x;
	int	sb_total_y;
	int	sb_total_x;
	int	sb_cur_y;
	int	sb_cur_x;
#endif

#ifdef	REGISTERWINDOWS
	WINDS*	visible;	/* List of visible windows	    */
	bool	refreshall;	/* Refresh all registered windows?  */
#endif

	short	line_color;	/* Color of line attributes - default white */

}	SCREEN;


/* external variables */
#if !defined(PDC_STATIC_BUILD) && defined(_MSC_VER) && defined(WIN32) && !defined(CURSES_LIBRARY)
__declspec(dllimport)	int	LINES;		/* terminal height		*/
__declspec(dllimport)	int	COLS;		/* terminal width		*/
__declspec(dllimport)	WINDOW*	stdscr;		/* the default screen window	*/
__declspec(dllimport)	WINDOW*	curscr;		/* the current screen image	*/
__declspec(dllimport)	SCREEN	*SP;	/* curses variables		*/
__declspec(dllimport)	int	use_emalloc;	/* set to true to use your own malloc,etc */
__declspec(dllimport)	MOUSE_STATUS Mouse_status;
__declspec(dllimport)	int COLORS;
__declspec(dllimport)	int COLOR_PAIRS;
#else
# if !defined(PDC_STATIC_BUILD) && defined(_MSC_VER) && defined(WIN32)
__declspec(dllexport) extern	int	LINES;		/* terminal height		*/
__declspec(dllexport) extern	int	COLS;		/* terminal width		*/
__declspec(dllexport) extern	WINDOW*	stdscr;		/* the default screen window	*/
__declspec(dllexport) extern	WINDOW*	curscr;		/* the current screen image	*/
__declspec(dllexport) extern	SCREEN	*SP;	/* curses variables		*/
__declspec(dllexport) extern	int	use_emalloc;	/* set to true to use your own malloc,etc */
__declspec(dllexport) extern	MOUSE_STATUS Mouse_status;
__declspec(dllexport) extern	int COLORS;
__declspec(dllexport) extern	int COLOR_PAIRS;
# else
extern	int	LINES;		/* terminal height		*/
extern	int	COLS;		/* terminal width		*/
extern	WINDOW*	stdscr;		/* the default screen window	*/
extern	WINDOW*	curscr;		/* the current screen image	*/
extern	SCREEN	*SP;	/* curses variables		*/
extern	int	use_emalloc;	/* set to true to use your own malloc,etc */
extern	MOUSE_STATUS Mouse_status;
extern	int COLORS,COLOR_PAIRS;
# endif
#endif

#if	defined (CURSES_LIBRARY)
extern	int	_default_lines;	/* For presetting maximum lines	*/
#endif

#ifdef	REGISTERWINDOWS
extern	ACTIVE*	CurWins;	/* Currently Visible Windows	*/
#endif




/*man-start*********************************************************************

PDCurses Text Attributes:

To include colour in PDCurses, a number of things had to be sacrificed
from the strict Unix and System V support.
The main problem is fitting all character attributes and colour into
an unsigned char (all 8 bits!). On System V, chtype is a long on
PDCurses it is a short int.

The following is the structure of a win->_attrs chtype:

-------------------------------------------------------------------------------------------------
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
-------------------------------------------------------------------------------------------------
     colour number      |     modifiers         |      character eg 'a' (potential for DBCS)

The available attribute modifiers are bold, underline, invisible, protect, reverse 
and blink.

**man-end**********************************************************************/

/* Video attribute definitions. */
#ifdef CHTYPE_LONG
# define A_NORMAL        0x00000000L
# define A_UNDERLINE     0x00100000L
# define A_REVERSE       0x00200000L
# define A_BLINK         0x00400000L
# define A_BOLD          0x00800000L
# define A_RIGHTLINE     0x00010000L
# define A_DIM           0x00020000L
# define A_ALTCHARSET    0x00040000L
# define A_INVIS         0x00080000L
# define A_ATTRIBUTES    0xFFFF0000L
# define A_CHARTEXT      0x0000FFFFL
# define A_COLOR         0xFF000000L

# define A_LEFTLINE      A_DIM
# define A_ITALIC        A_INVIS
# define A_STANDOUT      ( A_BOLD | A_REVERSE )
# define A_PROTECT       ( A_UNDERLINE | A_LEFTLINE | A_RIGHTLINE )

#else

# define A_NORMAL      (chtype)0x0000                  /* System V */
# define A_ALTCHARSET  (chtype)0x0000                  /* X/Open   */
# define A_BLINK       (chtype)0x0400                  /* X/Open   */
# define A_BLANK       (chtype)0x0000                  /* X/Open   */
# define A_BOLD        (chtype)0x0100                  /* X/Open   */
# define A_DIM         (chtype)0x0000                  /* X/Open   */
# define A_PROTECT     (chtype)0x0000                  /* X/Open   */
# define A_REVERSE     (chtype)0x0200                  /* X/Open   */
# define A_STANDOUT    ((chtype)(A_REVERSE | A_BOLD))  /* X/Open   */
# define A_UNDERLINE   (chtype)0x0000                  /* X/Open   */
# define A_COLOR       (chtype)0xF800                  /* System V  */
# define A_CHARTEXT    (chtype)(0xFF)                  /* X/Open   */
# define A_ATTRIBUTES  (chtype)(~A_CHARTEXT)           /* X/Open   */
#endif

#define CHR_MSK		A_CHARTEXT		/* Obsolete	*/
#define ATR_MSK		A_ATTRIBUTES	/* Obsolete	*/
#define ATR_NRM		A_NORMAL			/* Obsolete	*/

#ifdef XCURSES
extern chtype *acs_map;

# define ACS_BSSB	(acs_map['l'])
# define ACS_SSBB	(acs_map['m'])
# define ACS_BBSS	(acs_map['k'])
# define ACS_SBBS	(acs_map['j'])
# define ACS_SBSS	(acs_map['u'])
# define ACS_SSSB	(acs_map['t'])
# define ACS_SSBS	(acs_map['v'])
# define ACS_BSSS	(acs_map['w'])
# define ACS_BSBS	(acs_map['q'])
# define ACS_SBSB	(acs_map['x'])
# define ACS_SSSS	(acs_map['n'])

# define ACS_ULCORNER	ACS_BSSB
# define ACS_LLCORNER	ACS_SSBB
# define ACS_URCORNER	ACS_BBSS
# define ACS_LRCORNER	ACS_SBBS
# define ACS_RTEE	    ACS_SBSS
# define ACS_LTEE	    ACS_SSSB
# define ACS_BTEE	    ACS_SSBS
# define ACS_TTEE	    ACS_BSSS
# define ACS_HLINE	   ACS_BSBS
# define ACS_VLINE	   ACS_SBSB
# define ACS_PLUS	    ACS_SSSS

# define ACS_S1	(acs_map['o'])
# define ACS_S9	(acs_map['s'])
# define ACS_DIAMOND	(acs_map['\''])
# define ACS_CKBOARD	(acs_map['a'])
# define ACS_DEGREE	(acs_map['f'])
# define ACS_PLMINUS	(acs_map['g'])
# define ACS_BULLET	(acs_map['~'])
# define ACS_LARROW	(acs_map[','])
# define ACS_RARROW	(acs_map['+'])
# define ACS_DARROW	(acs_map['.'])
# define ACS_UARROW	(acs_map['-'])
# define ACS_BOARD	(acs_map['h'])
# define ACS_LANTERN	(acs_map['i'])
# define ACS_BLOCK	(acs_map['0'])

# define ACS_S3	(acs_map['p'])
# define ACS_S7	(acs_map['r'])
# define ACS_LEQUAL	(acs_map['y'])
# define ACS_GEQUAL	(acs_map['z'])
# define ACS_PI	(acs_map['{'])
# define ACS_NEQUAL	(acs_map['|'])
# define ACS_STERLING	(acs_map['}'])
#endif

#if defined(DOS) || defined(OS2) || defined(WIN32)
/* ALTCHARSET definitions from jshumate@wrdis01.robins.af.mil */
# define ACS_ULCORNER	(chtype)0xda			/* SysV		*/
# define ACS_LLCORNER	(chtype)0xc0			/* SysV		*/
# define ACS_URCORNER	(chtype)0xbf			/* SysV		*/
# define ACS_LRCORNER	(chtype)0xd9			/* SysV		*/
# define ACS_RTEE	(chtype)0xb4			/* SysV		*/
# define ACS_LTEE	(chtype)0xc3			/* SysV		*/
# define ACS_BTEE	(chtype)0xc1			/* SysV		*/
# define ACS_TTEE	(chtype)0xc2			/* SysV		*/
# define ACS_HLINE	(chtype)0xc4			/* SysV		*/
# define ACS_VLINE	(chtype)0xb3			/* SysV		*/
# define ACS_PLUS	(chtype)0xc5			/* SysV		*/
# define ACS_S1	(chtype)0x2d			/* SysV		*/
# define ACS_S9	(chtype)0x5f			/* SysV		*/
# define ACS_DIAMOND	(chtype)0xc5			/* SysV		*/
# define ACS_CKBOARD	(chtype)0xb2			/* SysV		*/
# define ACS_DEGREE	(chtype)0xf8			/* SysV		*/
# define ACS_PLMINUS	(chtype)0xf1			/* SysV		*/
# define ACS_BULLET	(chtype)0xf9			/* SysV		*/
# define ACS_LARROW	(chtype)0x3c			/* SysV		*/
# define ACS_RARROW	(chtype)0x3e			/* SysV		*/
# define ACS_DARROW	(chtype)0x76			/* SysV		*/
# define ACS_UARROW	(chtype)0x5e			/* SysV		*/
# define ACS_BOARD	(chtype)0x23			/* SysV		*/
# define ACS_LANTERN	(chtype)0x23			/* SysV		*/
# define ACS_BLOCK	(chtype)0x23			/* SysV		*/

/* the following definitions can be used if you have set raw_output()
   or are using the PDCurses *raw*() functions                        */
# if 0
#  define ACS_DIAMOND	(chtype)0x04			/* SysV		*/
#  define ACS_LARROW	(chtype)0x1b			/* SysV		*/
#  define ACS_RARROW	(chtype)0x1a			/* SysV		*/
#  define ACS_DARROW	(chtype)0x19			/* SysV		*/
#  define ACS_UARROW	(chtype)0x18			/* SysV		*/
#  define ACS_BOARD	(chtype)0xb0			/* SysV		*/
#  define ACS_LANTERN	(chtype)0x0f			/* SysV		*/
#  define ACS_BLOCK	(chtype)0xdb			/* SysV		*/
# endif
#endif

/* colour attributes */
#if defined (XCURSES)
# define COLOR_BLACK		0
# define COLOR_RED		1
# define COLOR_GREEN		2
# define COLOR_YELLOW		3
# define COLOR_BLUE		4
# define COLOR_MAGENTA		5
# define COLOR_CYAN		6
# define COLOR_WHITE		7
#endif

#if defined(DOS) || defined(OS2)
# define COLOR_BLACK		0
# define COLOR_BLUE		1
# define COLOR_GREEN		2
# define COLOR_CYAN		3
# define COLOR_RED		4
# define COLOR_MAGENTA		5
# define COLOR_YELLOW		6
# define COLOR_WHITE		7
#endif

#if defined(WIN32)
# define MS_MOUSE_MOVED	0x0001
/*
 * These defines taken directly from windows.h to reduce
 * compilation time by only #include'ing <windows.h>
 * when absolutely necesssary. Cygnus-W32 #defines all of these
 * so we need to check for that.
 */
# ifndef FOREGROUND_BLUE
#  define FOREGROUND_BLUE	0x0001
# endif
# ifndef FOREGROUND_GREEN
#  define FOREGROUND_GREEN	0x0002
# endif
# ifndef FOREGROUND_RED
#  define FOREGROUND_RED	0x0004
# endif
# ifndef FOREGROUND_INTENSITY
#  define FOREGROUND_INTENSITY	0x0008		/* BOLD */
# endif
# ifndef BACKGROUND_BLUE
#  define BACKGROUND_BLUE	0x0010
# endif
# ifndef BACKGROUND_GREEN
#  define BACKGROUND_GREEN	0x0020
# endif
# ifndef BACKGROUND_RED
#  define BACKGROUND_RED	0x0040
# endif
# ifndef BACKGROUND_INTENSITY
#  define BACKGROUND_INTENSITY	0x0080		/* BLINK */
# endif

# define COLOR_BLACK		0
# define COLOR_BLUE		FOREGROUND_BLUE
# define COLOR_RED		FOREGROUND_RED
# define COLOR_GREEN		FOREGROUND_GREEN
# define COLOR_CYAN		FOREGROUND_BLUE | FOREGROUND_GREEN
# define COLOR_MAGENTA		FOREGROUND_RED | FOREGROUND_BLUE
# define COLOR_YELLOW		FOREGROUND_RED | FOREGROUND_GREEN
# define COLOR_WHITE		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#endif

#ifdef CHTYPE_LONG
#define COLOR_PAIR(n)  ((chtype)(n) << 24)
#define PAIR_NUMBER(n) (((n) & A_COLOR) >> 24)
#else
#define COLOR_PAIR(n)  (((n) << 11) & A_ATTRIBUTES)
#define PAIR_NUMBER(n) (((n) & A_COLOR) >> 11)
#endif

/*----------------------------------------------------------------------
 *
 *	Function and Keypad Key Definitions.
 *	Many are just for compatibility.
 *
 */
#define KEY_MIN         0x101   /* Minimum curses key value      */
#define KEY_BREAK       0x101   /* Not on PC KBD                 */
#define KEY_DOWN        0x102   /* Down arrow key                */
#define KEY_UP          0x103   /* Up arrow key                  */
#define KEY_LEFT        0x104   /* Left arrow key                */
#define KEY_RIGHT       0x105   /* Right arrow key               */
#define KEY_HOME        0x106   /* home key                      */
#define KEY_BACKSPACE   0x107   /* not on pc                     */
#define KEY_F0          0x108   /* function keys. space for      */
#define KEY_F(n)    (KEY_F0+(n))/* 64 keys are reserved.         */
#define KEY_DL         0x148   /* delete line                   */
#define KEY_IL         0x149   /* insert line                   */
#define KEY_DC         0x14a   /* delete character              */
#define KEY_IC         0x14b   /* insert char or enter ins mode */
#define KEY_EIC        0x14c   /* exit insert char mode         */
#define KEY_CLEAR      0x14d   /* clear screen                  */
#define KEY_EOS        0x14e   /* clear to end of screen        */
#define KEY_EOL        0x14f   /* clear to end of line          */
#define KEY_SF         0x150   /* scroll 1 line forward         */
#define KEY_SR         0x151   /* scroll 1 line back (reverse)  */
#define KEY_NPAGE      0x152   /* next page                     */
#define KEY_PPAGE      0x153   /* previous page                 */
#define KEY_STAB       0x154   /* set tab                       */
#define KEY_CTAB       0x155   /* clear tab                     */
#define KEY_CATAB      0x156   /* clear all tabs                */
#define KEY_ENTER      0x157   /* enter or send (unreliable)    */
#define KEY_SRESET     0x158   /* soft/reset (partial/unreliable)*/
#define KEY_RESET      0x159   /* reset/hard reset (unreliable) */
#define KEY_PRINT      0x15a   /* print/copy                    */
#define KEY_LL         0x15b   /* home down/bottom (lower left) */
#define KEY_ABORT      0x15c   /* abort/terminate key (any)     */
#define KEY_SHELP      0x15d   /* short help                    */
#define KEY_LHELP      0x15e   /* long help                     */
#define KEY_BTAB       0x15f   /* Back tab key                  */
#define KEY_BEG        0x160   /* beg(inning) key               */
#define KEY_CANCEL     0x161   /* cancel key                    */
#define KEY_CLOSE      0x162   /* close key                     */
#define KEY_COMMAND    0x163   /* cmd (command) key             */
#define KEY_COPY       0x164   /* copy key                      */
#define KEY_CREATE     0x165   /* create key                    */
#define KEY_END        0x166   /* end key                       */
#define KEY_EXIT       0x167   /* exit key                      */
#define KEY_FIND       0x168   /* find key                      */
#define KEY_HELP       0x169   /* help key                      */
#define KEY_MARK       0x16a   /* mark key                      */
#define KEY_MESSAGE    0x16b   /* message key                   */
#define KEY_MOVE       0x16c   /* move key                      */
#define KEY_NEXT       0x16d   /* next object key               */
#define KEY_OPEN       0x16e   /* open key                      */
#define KEY_OPTIONS    0x16f   /* options key                   */
#define KEY_PREVIOUS   0x170   /* previous object key           */
#define KEY_REDO       0x171   /* redo key                      */
#define KEY_REFERENCE  0x172   /* ref(erence) key               */
#define KEY_REFRESH    0x173   /* refresh key                   */
#define KEY_REPLACE    0x174   /* replace key                   */
#define KEY_RESTART    0x175   /* restart key                   */
#define KEY_RESUME     0x176   /* resume key                    */
#define KEY_SAVE       0x177   /* save key                      */
#define KEY_SBEG       0x178   /* shifted beginning key         */
#define KEY_SCANCEL    0x179   /* shifted cancel key            */
#define KEY_SCOMMAND   0x17a   /* shifted command key           */
#define KEY_SCOPY      0x17b   /* shifted copy key              */
#define KEY_SCREATE    0x17c   /* shifted create key            */
#define KEY_SDC        0x17d   /* shifted delete char key       */
#define KEY_SDL        0x17e   /* shifted delete line key       */
#define KEY_SELECT     0x17f   /* select key                    */
#define KEY_SEND       0x180   /* shifted end key               */
#define KEY_SEOL       0x181   /* shifted clear line key        */
#define KEY_SEXIT      0x182   /* shifted exit key              */
#define KEY_SFIND      0x183   /* shifted find key              */
#define KEY_SHOME      0x184   /* shifted home key              */
#define KEY_SIC        0x185   /* shifted input key             */
#define KEY_SLEFT      0x187   /* shifted left arrow key        */
#define KEY_SMESSAGE   0x188   /* shifted message key           */
#define KEY_SMOVE      0x189   /* shifted move key              */
#define KEY_SNEXT      0x18a   /* shifted next key              */
#define KEY_SOPTIONS   0x18b   /* shifted options key           */
#define KEY_SPREVIOUS  0x18c   /* shifted prev key              */
#define KEY_SPRINT     0x18d   /* shifted print key             */
#define KEY_SREDO      0x18e   /* shifted redo key              */
#define KEY_SREPLACE   0x18f   /* shifted replace key           */
#define KEY_SRIGHT     0x190   /* shifted right arrow           */
#define KEY_SRSUME     0x191   /* shifted resume key            */
#define KEY_SSAVE      0x192   /* shifted save key              */
#define KEY_SSUSPEND   0x193   /* shifted suspend key           */
#define KEY_SUNDO      0x194   /* shifted undo key              */
#define KEY_SUSPEND    0x195   /* suspend key                   */
#define KEY_UNDO       0x196   /* undo key                      */

/* PDCurses specific key definitions */

#define ALT_0           0x197   /* Alt-0                PC only  */
#define ALT_1           0x198   /* Alt-1                PC only  */
#define ALT_2           0x199   /* Alt-2                PC only  */
#define ALT_3           0x19a   /* Alt-3                PC only  */
#define ALT_4           0x19b   /* Alt-4                PC only  */
#define ALT_5           0x19c   /* Alt-5                PC only  */
#define ALT_6           0x19d   /* Alt-6                PC only  */
#define ALT_7           0x19e   /* Alt-7                PC only  */
#define ALT_8           0x19f   /* Alt-8                PC only  */
#define ALT_9           0x1a0   /* Alt-9                PC only  */
#define ALT_A           0x1a1   /* Alt-A                PC only  */
#define ALT_B           0x1a2   /* Alt-B                PC only  */
#define ALT_C           0x1a3   /* Alt-C                PC only  */
#define ALT_D           0x1a4   /* Alt-D                PC only  */
#define ALT_E           0x1a5   /* Alt-E                PC only  */
#define ALT_F           0x1a6   /* Alt-F                PC only  */
#define ALT_G           0x1a7   /* Alt-G                PC only  */
#define ALT_H           0x1a8   /* Alt-H                PC only  */
#define ALT_I           0x1a9   /* Alt-I                PC only  */
#define ALT_J           0x1aa   /* Alt-J                PC only  */
#define ALT_K           0x1ab   /* Alt-K                PC only  */
#define ALT_L           0x1ac   /* Alt-L                PC only  */
#define ALT_M           0x1ad   /* Alt-M                PC only  */
#define ALT_N           0x1ae   /* Alt-N                PC only  */
#define ALT_O           0x1af   /* Alt-O                PC only  */
#define ALT_P           0x1b0   /* Alt-P                PC only  */
#define ALT_Q           0x1b1   /* Alt-Q                PC only  */
#define ALT_R           0x1b2   /* Alt-R                PC only  */
#define ALT_S           0x1b3   /* Alt-S                PC only  */
#define ALT_T           0x1b4   /* Alt-T                PC only  */
#define ALT_U           0x1b5   /* Alt-U                PC only  */
#define ALT_V           0x1b6   /* Alt-V                PC only  */
#define ALT_W           0x1b7   /* Alt-W                PC only  */
#define ALT_X           0x1b8   /* Alt-X                PC only  */
#define ALT_Y           0x1b9   /* Alt-Y                PC only  */
#define ALT_Z           0x1ba   /* Alt-Z                PC only  */
#define CTL_LEFT        0x1bb   /* Control-Left-Arrow   PC only  */
#define CTL_RIGHT       0x1bc   /* Control-Right-Arrow  PC only  */
#define CTL_PGUP        0x1bd   /* Control-PgUp         PC only  */
#define CTL_PGDN        0x1be   /* Control-PgDn         PC only  */
#define CTL_HOME        0x1bf   /* Control-Home         PC only  */
#define CTL_END         0x1c0   /* Control-End          PC only  */

#if defined(FLEXOS)
# define KEY_MOUSE       0x1c1 /* "mouse" key  */
# define KEY_A1          KEY_HOME/* upper left on Virtual keypad  */
# define KEY_A2          KEY_UP  /* upper middle on Virt. keypad  */
# define KEY_A3          KEY_PPAGE/* upper right on Vir. keypad   */
# define KEY_B1          KEY_LEFT/* middle left on Virt. keypad   */
# define KEY_B2          0x00    /* center on Virt. keypad        */
# define KEY_B3          KEY_RIGHT/* middle right on Vir. keypad  */
# define KEY_C1          KEY_LL  /* lower left on Virt. keypad    */
# define KEY_C2          KEY_DOWN /* lower middle on Virt. keypad */
# define KEY_C3          KEY_NPAGE /* lower right on Vir. keypad  */
# define KEY_MAX         KEY_MOUSE /* Maximum curses key        */
#endif


#if defined(DOS)  || defined (OS2) || defined(XCURSES) || defined(WIN32)
# define KEY_A1          0x1c1   /* upper left on Virtual keypad  */
# define KEY_A2          0x1c2   /* upper middle on Virt. keypad  */
# define KEY_A3          0x1c3   /* upper right on Vir. keypad    */
# define KEY_B1          0x1c4   /* middle left on Virt. keypad   */
# define KEY_B2          0x1c5   /* center on Virt. keypad        */
# define KEY_B3          0x1c6   /* middle right on Vir. keypad   */
# define KEY_C1          0x1c7   /* lower left on Virt. keypad    */
# define KEY_C2          0x1c8   /* lower middle on Virt. keypad  */
# define KEY_C3          0x1c9   /* lower right on Vir. keypad    */
# define PADSLASH        0x1ca   /* slash on keypad               */
# define PADENTER        0x1cb   /* enter on keypad               */
# define CTL_PADENTER    0x1cc   /* ctl-enter on keypad           */
# define ALT_PADENTER    0x1cd   /* alt-enter on keypad           */
# define PADSTOP         0x1ce   /* stop on keypad                */
# define PADSTAR         0x1cf   /* star on keypad                */
# define PADMINUS        0x1d0   /* minus on keypad               */
# define PADPLUS         0x1d1   /* plus on keypad                */
# define CTL_PADSTOP     0x1d2   /* ctl-stop on keypad            */
# define CTL_PADCENTER   0x1d3   /* ctl-enter on keypad           */
# define CTL_PADPLUS     0x1d4   /* ctl-plus on keypad            */
# define CTL_PADMINUS    0x1d5   /* ctl-minus on keypad           */
# define CTL_PADSLASH    0x1d6   /* ctl-slash on keypad           */
# define CTL_PADSTAR     0x1d7   /* ctl-star on keypad            */
# define ALT_PADPLUS     0x1d8   /* alt-plus on keypad            */
# define ALT_PADMINUS    0x1d9   /* alt-minus on keypad           */
# define ALT_PADSLASH    0x1da   /* alt-slash on keypad           */
# define ALT_PADSTAR     0x1db   /* alt-star on keypad            */
# define ALT_PADSTOP     0x1dc   /* alt-star on keypad            */
# define CTL_INS         0x1dd   /* ctl-insert                    */
# define ALT_DEL         0x1de   /* alt-delete                    */
# define ALT_INS         0x1df   /* alt-insert                    */
# define CTL_UP          0x1e0   /* ctl-up arrow                  */
# define CTL_DOWN        0x1e1   /* ctl-down arrow                */
# define CTL_TAB         0x1e2   /* ctl-tab                       */
# define ALT_TAB         0x1e3   /* alt-tab                       */
# define ALT_MINUS       0x1e4   /* alt-minus                     */
# define ALT_EQUAL       0x1e5   /* alt-equal                     */
# define ALT_HOME        0x1e6   /* alt-home                      */
# define ALT_PGUP        0x1e7   /* alt-pgup                      */
# define ALT_PGDN        0x1e8   /* alt-pgdn                      */
# define ALT_END         0x1e9   /* alt-end                       */
# define ALT_UP          0x1ea   /* alt-up arrow                  */
# define ALT_DOWN        0x1eb   /* alt-down arrow                */
# define ALT_RIGHT       0x1ec   /* alt-right arrow               */
# define ALT_LEFT        0x1ed   /* alt-left arrow                */
# define ALT_ENTER       0x1ee   /* alt-enter                     */
# define ALT_ESC         0x1ef   /* alt-escape                    */
# define ALT_BQUOTE      0x1f0   /* alt-back quote                */
# define ALT_LBRACKET    0x1f1   /* alt-left bracket              */
# define ALT_RBRACKET    0x1f2   /* alt-right bracket             */
# define ALT_SEMICOLON   0x1f3   /* alt-semi-colon                */
# define ALT_FQUOTE      0x1f4   /* alt-forward quote             */
# define ALT_COMMA       0x1f5   /* alt-comma                     */
# define ALT_STOP        0x1f6   /* alt-stop                      */
# define ALT_FSLASH      0x1f7   /* alt-forward slash             */
# define ALT_BKSP        0x1f8   /* alt-backspace                 */
# define CTL_BKSP        0x1f9   /* ctl-backspace                 */
# define PAD0            0x1fa   /* ctl-keypad 0                  */
# define CTL_PAD0        0x1fb   /* ctl-keypad 0                  */
# define CTL_PAD1        0x1fc   /* ctl-keypad 1                  */
# define CTL_PAD2        0x1fd   /* ctl-keypad 2                  */
# define CTL_PAD3        0x1fe   /* ctl-keypad 3                  */
# define CTL_PAD4        0x1ff   /* ctl-keypad 4                  */
# define CTL_PAD5        0x200   /* ctl-keypad 5                  */
# define CTL_PAD6        0x201   /* ctl-keypad 6                  */
# define CTL_PAD7        0x202   /* ctl-keypad 7                  */
# define CTL_PAD8        0x203   /* ctl-keypad 8                  */
# define CTL_PAD9        0x204   /* ctl-keypad 9                  */
# define ALT_PAD0        0x205   /* ctl-keypad 0                  */
# define ALT_PAD1        0x206   /* ctl-keypad 1                  */
# define ALT_PAD2        0x207   /* ctl-keypad 2                  */
# define ALT_PAD3        0x208   /* ctl-keypad 3                  */
# define ALT_PAD4        0x209   /* ctl-keypad 4                  */
# define ALT_PAD5        0x20a   /* ctl-keypad 5                  */
# define ALT_PAD6        0x20b   /* ctl-keypad 6                  */
# define ALT_PAD7        0x20c   /* ctl-keypad 7                  */
# define ALT_PAD8        0x20d   /* ctl-keypad 8                  */
# define ALT_PAD9        0x20e   /* ctl-keypad 9                  */
# define CTL_DEL         0x20f   /* clt-delete                    */
# define ALT_BSLASH      0x210   /* alt-back slash                */
# define CTL_ENTER       0x211   /* ctl-enter                     */
# define SHF_PADENTER    0x212   /* shift-enter on keypad         */
# define SHF_PADSLASH    0x213   /* shift-slash on keypad         */
# define SHF_PADSTAR     0x214   /* shift-star  on keypad         */
# define SHF_PADPLUS     0x215   /* shift-plus  on keypad         */
# define SHF_PADMINUS    0x216   /* shift-minus on keypad         */
# define SHF_UP          0x217   /* shift-up on keypad            */
# define SHF_DOWN        0x218   /* shift-down on keypad          */
# define SHF_IC          0x219   /* shift-insert on keypad        */
# define SHF_DC          0x21a   /* shift-delete on keypad        */
# define KEY_MOUSE       0x21b   /* "mouse" key                   */
# define KEY_SHIFT_L     0x21c   /* Left-shift                    */
# define KEY_SHIFT_R     0x21d   /* Right-shift                   */
# define KEY_CONTROL_L   0x21e   /* Left-control                  */
# define KEY_CONTROL_R   0x21f   /* Right-control                 */
# define KEY_ALT_L       0x220   /* Left-alt                      */
# define KEY_ALT_R       0x221   /* Right-alt                     */
# define KEY_RESIZE      0x222   /* Window resize                 */
# define KEY_SUP         0x223   /* Shifted up arrow              */
# define KEY_SDOWN       0x224   /* Shifted down arrow            */
# define KEY_MAX         0x224   /* Maximum curses key            */
#endif

/*----------------------------------------------------------------------
*       PDCurses function declarations
*/
#ifdef HAVE_PROTO
int     PDC_CDECL	addchnstr( chtype *, int );
int     PDC_CDECL	baudrate( void );
int     PDC_CDECL	beep( void );
int     PDC_CDECL	border( chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype );
int     PDC_CDECL	can_change_color ( void );
int     PDC_CDECL	clearok( WINDOW*, bool );
int     PDC_CDECL	color_content( short, short*, short*, short* );
int     PDC_CDECL	copywin( WINDOW*, WINDOW*, int, int, int, int, int, int, int );
int     PDC_CDECL	curs_set( int );
int     PDC_CDECL	def_prog_mode( void );
int     PDC_CDECL	def_shell_mode( void );
int     PDC_CDECL	delay_output(  int  );
int     PDC_CDECL	delwin( WINDOW* );
WINDOW* PDC_CDECL	derwin( WINDOW*, int, int, int, int );
int     PDC_CDECL	doupdate( void );
WINDOW* PDC_CDECL	dupwin( WINDOW* );
int     PDC_CDECL	endwin( void );
int     PDC_CDECL	erase( void );
char    PDC_CDECL	erasechar( void );
int     PDC_CDECL	flash( void );
int     PDC_CDECL	flushinp( void );
int     PDC_CDECL	getsyx( int*, int* );
int     PDC_CDECL	halfdelay( int );
bool    PDC_CDECL	has_colors( void );
int     PDC_CDECL	hline( chtype, int );
int     PDC_CDECL	immedok( WINDOW*, bool );
int     PDC_CDECL	inchnstr( chtype *, int );
int     PDC_CDECL	init_color( short, short, short, short );
int     PDC_CDECL	init_pair( short, short, short );
WINDOW* PDC_CDECL	initscr( void );
#ifdef XCURSES
WINDOW* PDC_CDECL	Xinitscr( int, char ** );
#endif
int     PDC_CDECL	intrflush(  WINDOW*, bool  );
int     PDC_CDECL	is_linetouched(WINDOW *,int);
int     PDC_CDECL	is_wintouched(WINDOW *);
char*   PDC_CDECL	keyname(  int  );
char    PDC_CDECL	killchar( void );
char*   PDC_CDECL	longname( void );
int     PDC_CDECL	meta( WINDOW*, bool );
int     PDC_CDECL	mvcur( int, int, int, int );
int     PDC_CDECL	mvderwin( WINDOW*, int, int );
#ifdef HAVE_STDARG_H
int     PDC_CDECL	mvprintw( int, int, char*,... );
int     PDC_CDECL	mvscanw( int, int, char*,... );
#endif
int     PDC_CDECL	mvwaddnstr( WINDOW*,int,int,char*,int );
int     PDC_CDECL	mvwin( WINDOW*, int, int );
int     PDC_CDECL	mvwinsertln( WINDOW*, int, int );
#ifdef HAVE_STDARG_H
int     PDC_CDECL	mvwprintw( WINDOW*, int, int, char*,... );
int     PDC_CDECL	mvwscanw( WINDOW*, int, int, char*,... );
#endif
WINDOW* PDC_CDECL	newpad( int, int );
SCREEN* PDC_CDECL	newterm( char*, FILE*, FILE* );
WINDOW* PDC_CDECL	newwin( int, int, int, int );
int     PDC_CDECL	noraw( void );
int     PDC_CDECL	notimeout( WINDOW *, bool );
int     PDC_CDECL	overlay( WINDOW*, WINDOW* );
int     PDC_CDECL	overwrite( WINDOW*, WINDOW* );
int     PDC_CDECL	pair_content( int, short*, short* );
int     PDC_CDECL	pnoutrefresh( WINDOW*, int, int, int, int, int, int );
int     PDC_CDECL	prefresh( WINDOW*, int, int, int, int, int, int );
#ifdef HAVE_STDARG_H
int     PDC_CDECL	printw( char*,... );
int     PDC_CDECL	scanw( char*,... );
#endif
int     PDC_CDECL	raw( void );
int     PDC_CDECL	refresh( void );
int     PDC_CDECL	reset_prog_mode( void );
int     PDC_CDECL	reset_shell_mode( void );
int     PDC_CDECL	resetty( void );
int     PDC_CDECL	ripoffline(int, int (*)(WINDOW *, int));
int     PDC_CDECL	savetty( void );
int     PDC_CDECL	sb_init(void);
int     PDC_CDECL	sb_set_horz(int, int, int);
int     PDC_CDECL	sb_set_vert(int, int, int);
int     PDC_CDECL	sb_get_horz(int *, int *, int *);
int     PDC_CDECL	sb_get_vert(int *, int *, int *);
int     PDC_CDECL	sb_refresh(void);
int     PDC_CDECL	scroll( WINDOW* );
SCREEN* PDC_CDECL	set_term( SCREEN* );
int     PDC_CDECL	setsyx( int, int );
int     PDC_CDECL	start_color( void );
int     PDC_CDECL	slk_init(int);
int     PDC_CDECL	slk_set(int, char *, int);
int     PDC_CDECL	slk_refresh(void);
int     PDC_CDECL	slk_noutrefresh(void);
char*   PDC_CDECL	slk_label(int);
int     PDC_CDECL	slk_clear(void);
int     PDC_CDECL	slk_restore(void);
int     PDC_CDECL	slk_touch(void);
int     PDC_CDECL	slk_attron(attr_t);
int     PDC_CDECL	slk_attrset(attr_t);
int     PDC_CDECL	slk_attroff(attr_t);
WINDOW* PDC_CDECL	subpad( WINDOW*, int, int, int, int );
WINDOW* PDC_CDECL	subwin( WINDOW*, int, int, int, int );
int     PDC_CDECL	syncok(WINDOW*, bool);
attr_t  PDC_CDECL	termattrs( void );
char*   PDC_CDECL	termname( void );
int     PDC_CDECL	touchline( WINDOW*, int ,int );
int     PDC_CDECL	touchwin( WINDOW* );
#if !defined (CURSES_LIBRARY)
void    PDC_CDECL	traceoff( void );
void    PDC_CDECL	traceon( void );
#endif
int     PDC_CDECL	typeahead( int );
char*   PDC_CDECL	unctrl( chtype );
int     PDC_CDECL	vline( chtype, int );
int     PDC_CDECL	waddchnstr( WINDOW*, chtype*, int );
int     PDC_CDECL	waddnstr( WINDOW*, char*, int );
int     PDC_CDECL	waddstr( WINDOW*, char* );
int     PDC_CDECL	wattroff( WINDOW*, attr_t );
int     PDC_CDECL	wattron( WINDOW*, attr_t );
int     PDC_CDECL	wattrset( WINDOW*, attr_t );
int     PDC_CDECL	wbkgd(WINDOW*, chtype);
void    PDC_CDECL	wbkgdset(WINDOW*, chtype);
int     PDC_CDECL	wborder( WINDOW*, chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype );
int     PDC_CDECL	wclear( WINDOW* );
int     PDC_CDECL	wclrtobot( WINDOW* );
int     PDC_CDECL	wclrtoeol( WINDOW* );
void    PDC_CDECL	wcursyncup(WINDOW*);
int     PDC_CDECL	wdelch( WINDOW* );
int     PDC_CDECL	wdeleteln( WINDOW* );
int     PDC_CDECL	werase( WINDOW* );
int     PDC_CDECL	wgetch( WINDOW* );
int     PDC_CDECL	wgetnstr( WINDOW*, char*, int );
int     PDC_CDECL	wgetstr( WINDOW*, char* );
int     PDC_CDECL	whline( WINDOW*, chtype, int );
int     PDC_CDECL	winchnstr( WINDOW*, chtype*, int );
int     PDC_CDECL	winnstr( WINDOW*, char*, int );
int     PDC_CDECL	winsch( WINDOW*, chtype );
int     PDC_CDECL	winsdelln( WINDOW*, int );
int     PDC_CDECL	winsertln( WINDOW* );
int     PDC_CDECL	winsnstr( WINDOW*, char*, int );
int     PDC_CDECL	wmove( WINDOW*, int, int );
int     PDC_CDECL	wnoutrefresh( WINDOW* );
char    PDC_CDECL	wordchar( void );
#ifdef HAVE_STDARG_H
int     PDC_CDECL	wprintw( WINDOW*, char*,... );
int     PDC_CDECL	wscanw( WINDOW*, char*,... );
#endif
int     PDC_CDECL	wredrawln( WINDOW*, int ,int );
int     PDC_CDECL	wrefresh( WINDOW* );
int     PDC_CDECL	wscrl( WINDOW*, int );
int     PDC_CDECL	wsetscrreg( WINDOW*, int, int );
int     PDC_CDECL	wtimeout( WINDOW *, int );
int     PDC_CDECL	wtouchln(WINDOW *, int, int, int);
void    PDC_CDECL	wsyncdown(WINDOW*);
void    PDC_CDECL	wsyncup(WINDOW*);
int     PDC_CDECL	wvline( WINDOW*, chtype, int );

#ifdef  PDCURSES
int     PDC_CDECL	raw_output( bool );
int     PDC_CDECL	resize_term( int, int );
WINDOW* PDC_CDECL	resize_window( WINDOW*, int, int );
#endif

#ifdef OBSOLETE_OR_TBD
char    PDC_CDECL	breakchar( /* void */ );
int     PDC_CDECL	cursoff( /* void */ );
int     PDC_CDECL	curson( /* void */ );
int     PDC_CDECL	gettmode( /* void */ );
int     PDC_CDECL	mvaddrawch( /* int, int, chtype */ );
int     PDC_CDECL	mvaddrawstr( /* int, int, char* */ );
int     PDC_CDECL	mvwinsrawch( /* WINDOW*, int, int, chtype */ );
int     PDC_CDECL	mvinsrawch( /* int, int, chtype */ );
int     PDC_CDECL	saveoldterm( /* void */ );
int     PDC_CDECL	tabsize( /* int */ );
int     PDC_CDECL	waddrawstr( /* WINDOW*, char* */ );
int     PDC_CDECL	wtabsize( /* WINDOW*, int */ );
int     PDC_CDECL	win_print( /* WINDOW*, int */ );
#endif

#ifdef  XCURSES
void    XCursesExit( void );
int     nocbreak( void );
int     cbreak( void );
int     nocrmode( void );
int     crmode( void );
int     noecho( void );
int     echo( void );
int     nodelay( WINDOW *,bool );
#endif

int     PDC_CDECL	mouse_set( unsigned long );
int     PDC_CDECL	mouse_on( unsigned long );
int     PDC_CDECL	mouse_off( unsigned long );
int     PDC_CDECL	request_mouse_pos( void );
int     PDC_CDECL	map_button( unsigned long );
void    PDC_CDECL	wmouse_position( WINDOW *, int *, int * );
unsigned long PDC_CDECL	getmouse( void );
unsigned long PDC_CDECL	getbmap( void );

/*
*       Keep the compiler happy with our macros below...
*/
int     PDC_CDECL	PDC_chadd( WINDOW*, chtype, bool, bool );
int     PDC_CDECL	PDC_chins( WINDOW*, chtype, bool );
int     PDC_CDECL	PDC_ungetch( int );

void    PDC_CDECL	PDC_set_title( char * );

int     PDC_CDECL	PDC_getclipboard( char **, long * );
int     PDC_CDECL	PDC_setclipboard( char *, long );
int     PDC_CDECL	PDC_freeclipboard( char * );

int     PDC_CDECL	PDC_get_input_fd( void );
int     PDC_CDECL	PDC_curs_set( int );
unsigned long PDC_CDECL	PDC_get_key_modifiers( void );

int     PDC_CDECL	PDC_wunderline( WINDOW*, int, bool );
int     PDC_CDECL	PDC_wleftline( WINDOW*, int, bool );
int     PDC_CDECL	PDC_wrightline( WINDOW*, int, bool );
int     PDC_CDECL	PDC_set_line_color( short );

#else

int     PDC_CDECL	addchnstr( /* chtype *, int */ );
int     PDC_CDECL	baudrate( /* void */ );
int     PDC_CDECL	beep( /* void */ );
int     PDC_CDECL	border( /* chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype */ );
int     PDC_CDECL	can_change_color ( /* void */ );
int     PDC_CDECL	clearok( /* WINDOW*, bool */ );
int     PDC_CDECL	color_content( /* int, short*, short*, short* */ );
int     PDC_CDECL	copywin( /* WINDOW*, WINDOW*, int, int, int, int, int, int, int */ );
int     PDC_CDECL	curs_set( /* int */ );
int     PDC_CDECL	def_prog_mode( /* void */ );
int     PDC_CDECL	def_shell_mode( /* void */ );
int     PDC_CDECL	delay_output( /*  int  */ );
int     PDC_CDECL	delwin( /* WINDOW* */ );
WINDOW* PDC_CDECL	derwin( /* WINDOW*, int, int, int, int */ );
int     PDC_CDECL	doupdate( /* void */ );
WINDOW* PDC_CDECL	dupwin( /* WINDOW* */ );
int     PDC_CDECL	endwin( /* void */ );
int     PDC_CDECL	erase( /* void */ );
char    PDC_CDECL	erasechar( /* void */ );
int     PDC_CDECL	flash( /* void */ );
int     PDC_CDECL	flushinp( /* void */ );
int     PDC_CDECL	getsyx( /* int*, int* */ );
int     PDC_CDECL	halfdelay( /* int */ );
bool    PDC_CDECL	has_colors( /* void */ );
int     PDC_CDECL	hline( /* chtype, int */ );
int     PDC_CDECL	immedok( /* WINDOW*, bool */ );
int     PDC_CDECL	inchnstr( /* chtype *, int */ );
int     PDC_CDECL	init_color( /* short, short, short, short */ );
int     PDC_CDECL	init_pair( /* short, short, short */ );
WINDOW* PDC_CDECL	initscr( /* void */ );
#ifdef XCURSES
WINDOW* PDC_CDECL	Xinitscr( /* int, char ** */ );
#endif
int     PDC_CDECL	intrflush( /*  WINDOW*, bool  */ );
int     PDC_CDECL	is_linetouched( /*WINDOW *,int*/ );
int     PDC_CDECL	is_wintouched( /*WINDOW **/ );
char*   PDC_CDECL	keyname( /*  int  */ );
char    PDC_CDECL	killchar( /* void */ );
char*   PDC_CDECL	longname( /* void */ );
int     PDC_CDECL	meta( /* WINDOW*, bool */ );
int     PDC_CDECL	mvcur( /* int, int, int, int */ );
int     PDC_CDECL	mvderwin( /* WINDOW*, int, int */ );
int     PDC_CDECL	mvprintw( /* int, int, char*,... */ );
int     PDC_CDECL	mvscanw( /* int, int, char*,... */ );
int     PDC_CDECL	mvwaddnstr(/* WINDOW*,int,int,char*,int */ );
int     PDC_CDECL	mvwin( /* WINDOW*, int, int */ );
int     PDC_CDECL	mvwinsertln( /* WINDOW*, int, int */ );
int     PDC_CDECL	mvwprintw( /* WINDOW*, int, int, char*,... */ );
int     PDC_CDECL	mvwscanw( /* WINDOW*, int, int, char*,... */ );
WINDOW* PDC_CDECL	newpad( /* int, int */ );
SCREEN* PDC_CDECL	newterm( /* char*, FILE*, FILE* */ );
WINDOW* PDC_CDECL	newwin( /* int, int, int, int */ );
int     PDC_CDECL	noraw( /* void */ );
int     PDC_CDECL	notimeout( /* WINDOW *, bool */ );
int     PDC_CDECL	overlay( /* WINDOW*, WINDOW* */ );
int     PDC_CDECL	overwrite( /* WINDOW*, WINDOW* */ );
int     PDC_CDECL	pair_content( /* int, short*, short* */ );
int     PDC_CDECL	pnoutrefresh( /* WINDOW*, int, int, int, int, int, int */ );
int     PDC_CDECL	prefresh( /* WINDOW*, int, int, int, int, int, int */ );
int     PDC_CDECL	printw( /* char*,... */ );
int     PDC_CDECL	raw( /* void */ );
int     PDC_CDECL	refresh( /* void */ );
int     PDC_CDECL	reset_prog_mode( /* void */ );
int     PDC_CDECL	reset_shell_mode( /* void */ );
int     PDC_CDECL	resetty( /* void */ );
int     PDC_CDECL	ripoffline( /* int, int (*)(WINDOW *, int) */);
int     PDC_CDECL	savetty( /* void */ );
int     PDC_CDECL	sb_init( /* void */ );
int     PDC_CDECL	sb_set_horz( /* int, int, int */ );
int     PDC_CDECL	sb_set_vert( /* int, int, int */ );
int     PDC_CDECL	sb_get_horz( /* int *, int *, int * */ );
int     PDC_CDECL	sb_get_vert( /* int *, int *, int * */ );
int     PDC_CDECL	sb_refresh( /* void */ );
int     PDC_CDECL	scanw( /* char*,... */ );
int     PDC_CDECL	scroll( /* WINDOW* */ );
int     PDC_CDECL	setsyx( /* int, int */ );
SCREEN* PDC_CDECL	set_term( /* SCREEN* */ );
int     PDC_CDECL	slk_init(/* int */);
int     PDC_CDECL	slk_set(/* int, char *, int */);
int     PDC_CDECL	slk_refresh(/* void */);
int     PDC_CDECL	slk_noutrefresh(/* void */);
char*   PDC_CDECL	slk_label(/* int */);
int     PDC_CDECL	slk_clear(/* void */);
int     PDC_CDECL	slk_restore(/* void */);
int     PDC_CDECL	slk_touch(/* void */);
int     PDC_CDECL	slk_attron(/* attr_t */);
int     PDC_CDECL	slk_attrset(/* attr_t */);
int     PDC_CDECL	slk_attroff(/* attr_t */);
int     PDC_CDECL	start_color( /* void */ );
WINDOW* PDC_CDECL	subpad( /* WINDOW*, int, int, int, int */ );
WINDOW* PDC_CDECL	subwin( /* WINDOW*, int, int, int, int */ );
int     PDC_CDECL	syncok(/* WINDOW*, bool */);
attr_t  PDC_CDECL	termattrs( /* void */ );
char*   PDC_CDECL	termname( /* void */ );
int     PDC_CDECL	touchline( /* WINDOW*, int ,int */ );
int     PDC_CDECL	touchwin( /* WINDOW* */ );
#if !defined (CURSES_LIBRARY)
void    PDC_CDECL	traceoff(/* void */);
void    PDC_CDECL	traceon(/* void */);
#endif
int     PDC_CDECL	typeahead( /* int */ );
char*   PDC_CDECL	unctrl( /* chtype */ );
int     PDC_CDECL	vline( /* chtype, int */ );
int     PDC_CDECL	waddchnstr( /* WINDOW*, chtype*, int */ );
int     PDC_CDECL	waddnstr( /* WINDOW*, char*, int */ );
int     PDC_CDECL	waddstr( /* WINDOW*, char* */ );
int     PDC_CDECL	wattroff( /* WINDOW*, attr_t */ );
int     PDC_CDECL	wattron( /* WINDOW*, attr_t */ );
int     PDC_CDECL	wattrset( /* WINDOW*, attr_t */ );
int     PDC_CDECL	wbkgd( /*WINDOW*, chtype*/ );
void    PDC_CDECL	wbkgdset( /*WINDOW*, chtype*/ );
int     PDC_CDECL	wborder( /* WINDOW*, chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype */ );
int     PDC_CDECL	wclear( /* WINDOW* */ );
int     PDC_CDECL	wclrtobot( /* WINDOW* */ );
int     PDC_CDECL	wclrtoeol( /* WINDOW* */ );
void    PDC_CDECL	wcursyncup(/* WINDOW* */);
int     PDC_CDECL	wdelch( /* WINDOW* */ );
int     PDC_CDECL	wdeleteln( /* WINDOW* */ );
int     PDC_CDECL	werase( /* WINDOW* */ );
int     PDC_CDECL	wgetch( /* WINDOW* */ );
int     PDC_CDECL	wgetnstr( /* WINDOW*, char*, int */ );
int     PDC_CDECL	wgetstr( /* WINDOW*, char* */ );
int     PDC_CDECL	whline( /* WINDOW*, chtype, int */ );
int     PDC_CDECL	winchnstr( /* WINDOW*, chtype*, int */ );
int     PDC_CDECL	winnstr( /* WINDOW*, char*, int */ );
int     PDC_CDECL	winsch( /* WINDOW*, chtype */ );
int     PDC_CDECL	winsdelln( /* WINDOW*, int */ );
int     PDC_CDECL	winsertln( /* WINDOW* */ );
int     PDC_CDECL	winsnstr( /* WINDOW*, char*, int */ );
int     PDC_CDECL	wmove( /* WINDOW*, int, int */ );
int     PDC_CDECL	wnoutrefresh( /* WINDOW* */ );
char    PDC_CDECL	wordchar( /* void */ );
int     PDC_CDECL	wprintw( /* WINDOW*, char*,... */ );
int     PDC_CDECL	wredrawln( /* WINDOW*, int ,int */ );
int     PDC_CDECL	wrefresh( /* WINDOW* */ );
int     PDC_CDECL	wscanw( /* WINDOW*, char*,... */ );
int     PDC_CDECL	wscrl( /* WINDOW*, int */ );
int     PDC_CDECL	wsetscrreg( /* WINDOW*, int, int */ );
int     PDC_CDECL	wtimeout( /* WINDOW *, int */ );
int     PDC_CDECL	wtouchln( /*WINDOW *, int, int, int*/ );
void    PDC_CDECL	wsyncdown(/* WINDOW* */);
void    PDC_CDECL	wsyncup(/* WINDOW* */);
int     PDC_CDECL	wvline( /* WINDOW*, chtype, int */ );

#ifdef  PDCURSES
int     PDC_CDECL	raw_output( /* bool */ );
int     PDC_CDECL	resize_term( /* int, int */ );
WINDOW* PDC_CDECL	resize_window( /* WINDOW*, int, int */ );
#endif

#ifdef OBSOLETE_OR_TBD
char    PDC_CDECL	breakchar( /* void */ );
int     PDC_CDECL	cursoff( /* void */ );
int     PDC_CDECL	curson( /* void */ );
int     PDC_CDECL	gettmode( /* void */ );
int     PDC_CDECL	mvaddrawch( /* int, int, chtype */ );
int     PDC_CDECL	mvaddrawstr( /* int, int, char* */ );
int     PDC_CDECL	mvwinsrawch( /* WINDOW*, int, int, chtype */ );
int     PDC_CDECL	mvinsrawch( /* int, int, chtype */ );
int     PDC_CDECL	saveoldterm( /* void */ );
int     PDC_CDECL	tabsize( /* int */ );
int     PDC_CDECL	waddrawstr( /* WINDOW*, char* */ );
int     PDC_CDECL	wtabsize( /* WINDOW*, int */ );
int     PDC_CDECL	win_print( /* WINDOW*, int */ );
#endif

#ifdef  XCURSES
void    XCursesExit(/* void */);
int     nocbreak( /* void */ );
int     cbreak( /* void */ );
int     nocrmode( /* void */ );
int     crmode( /* void */ );
int     noecho( /* void */ );
int     echo( /* void */ );
int     nodelay( /* WINDOW *,bool */ );
#endif

int     PDC_CDECL	mouse_set( /* long */ );
int     PDC_CDECL	mouse_on( /* long */ );
int     PDC_CDECL	mouse_off( /* long */ );
int     PDC_CDECL	request_mouse_pos( /* void */ );
int     PDC_CDECL	map_button( /* unsigned long */ );
void    PDC_CDECL	wmouse_position( /* WINDOW *, int *, int * */ );
unsigned long PDC_CDECL	getmouse( /* void */ );
unsigned long PDC_CDECL	getbmap( /* void */ );

/*
*       Keep the compiler happy with our macros below...
*/
int     PDC_CDECL	PDC_chadd( /* WINDOW*, chtype, bool, bool */ );
int     PDC_CDECL	PDC_chins( /* WINDOW*, chtype, bool */ );
int     PDC_CDECL	PDC_ungetch( /* int */ );

void    PDC_CDECL	PDC_set_title( /* char * */ );
int     PDC_CDECL	PDC_getclipboard( /* char **, long * */ );
int     PDC_CDECL	PDC_setclipboard( /* char *, long */ );

int     PDC_CDECL	PDC_get_input_fd( /* void */ );
int     PDC_CDECL	PDC_curs_set( /* int */ );
unsigned long PDC_CDECL	PDC_get_key_modifiers( /* void */ );

int     PDC_CDECL	PDC_wunderline( /* WINDOW*, int, bool */ );
int     PDC_CDECL	PDC_wleftline( /* WINDOW*, int, bool */ );
int     PDC_CDECL	PDC_wrightline( /* WINDOW*, int, bool */ );
int     PDC_CDECL	PDC_set_line_color( /* short */ );

#endif


#ifndef max
# define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
# define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/*
*       Functions defined as macros
*/

#define addch( c )              waddch( stdscr, c )
#define addchstr( c )           addchnstr( c, -1 )
#define addstr(str)             waddstr( stdscr, str )
#define addnstr(str, n)         waddnstr( stdscr, str, n )
#define attroff(attr)           wattroff( stdscr, attr )
#define attron(attr)            wattron( stdscr, attr )
#define attrset(attr)           wattrset( stdscr, attr )
#define bkgd(c)                 wbkgd(stdscr,c)
#define bkgdset(c)              wbkgdset(stdscr,c)
#define border(ls,rs,ts,bs,tl,tr,bl,br)  wborder(stdscr,ls,rs,ts,bs,tl,tr,bl,br)
#define box( w, v, h )          wborder( w, v, v, h, h, 0, 0, 0, 0 )
#define clear()                 wclear( stdscr )
#define clrtobot()              wclrtobot( stdscr )
#define clrtoeol()              wclrtoeol( stdscr )
#define delch()                 wdelch( stdscr )
#define deleteln()              wdeleteln( stdscr )
#define derwin(w,nl,nc,by,bx)   subwin((w),(nl),(nc),(by+(w)->_begy),(bx+(w)->_begx))
#define draino(ms)              delay_output(ms)
#define echochar(c)             (addch((chtype)c)==ERR?ERR:refresh())
#define erase()                 werase( stdscr )
#define fixterm()               reset_prog_mode()
#define getbegx(w)              (w)->_begx
#define getbegy(w)              (w)->_begy
#define getbegyx(w,y,x)         ( y = (w)->_begy, x = (w)->_begx )
#define getbkgd(w)              ((w)->_bkgd)
#define getch()                 wgetch(stdscr)
#define getmaxx(w)              (w)->_maxx
#define getmaxy(w)              (w)->_maxy
#define getmaxyx(w,y,x)         ( y = (w)->_maxy, x = (w)->_maxx )
#define getparx(w)              (w)->_parx
#define getpary(w)              (w)->_pary
#define getparyx(w,y,x)         ( y = (w)->_pary, x = (w)->_parx )
#define getstr(str)             wgetstr( stdscr, str )
#define getnstr(str,num)        wgetnstr( stdscr, str, num )
#define getyx(w,y,x)            ( y = (w)->_cury, x = (w)->_curx )
#define has_colors()            ((SP->mono) ? FALSE : TRUE)
#define idcok(w,flag)           OK
#define idlok(w,flag)           OK
#define inch()                  (stdscr->_y[stdscr->_cury][stdscr->_curx])
#define inchstr( c )            inchnstr( c, stdscr->_maxx-stdscr->_curx )
#define innstr(str,n)           winnstr(stdscr,(str),(n))
#define insch( c )              winsch( stdscr, c )
#define insdelln(n)             winsdelln(stdscr,n)
#define insertln()              winsertln( stdscr )
#define insnstr(s,n)            winsnstr(stdscr,s,n)
#define insstr(s)               winsnstr(stdscr,s,(-1))
#define instr(str)              winnstr(stdscr,(str),stdscr->_maxx)
#define isendwin()              ((SP->alive) ? FALSE : TRUE)
#define is_termresized()        (SP->resized)
#define keypad(w,flag)          (w->_use_keypad  = flag)
#define leaveok(w,flag)         (w->_leaveit = flag)
#define move(y,x)               wmove( stdscr, y, x )
#define mvaddch(y,x,c)          (move( y, x )==ERR?ERR:addch( c ))
#define mvaddchstr(y,x,c)       (move( y, x )==ERR?ERR:addchnstr( c, -1 ))
#define mvaddchnstr(y,x,c,n)    (move( y, x )==ERR?ERR:addchnstr( c, n ))
#define mvaddstr(y,x,str)       (move( y, x )==ERR?ERR:addstr( str ))
#define mvaddnstr(y,x,str,n)    (move( y, x )==ERR?ERR:addnstr( str, n ))
#define mvdelch(y,x)            (move( y, x )==ERR?ERR:wdelch( stdscr ))
#define mvgetch(y,x)            (move( y, x )==ERR?ERR:wgetch(stdscr))
#define mvgetstr(y,x,str)       (move( y, x )==ERR?ERR:wgetstr( stdscr, str ))
#define mvinch(y,x)             (move( y, x )==ERR?ERR:(stdscr->_y[y][x]))
#define mvinchstr(y,x,c)        (move( y, x )==ERR?ERR:inchnstr( c, stdscr->_maxx-stdscr->_curx ))
#define mvinchnstr(y,x,c,n)     (move( y, x )==ERR?ERR:inchnstr( c, n ))
#define mvinsch(y,x,c)          (move( y, x )==ERR?ERR:winsch( stdscr, c ))
#define mvinsnstr(y,x,s,n)      (move( y, x )==ERR?ERR:winsnstr(stdscr,s,n))
#define mvinsstr(y,x,s)         (move( y, x )==ERR?ERR:winsnstr(stdscr,s,(-1)))
#define mvinstr(y,x,str)        (move( y, x )==ERR?ERR:winnstr(stdscr,(str),stdscr->_maxx))
#define mvinnstr(y,x,str,n)     (move( y, x )==ERR?ERR:winnstr(stdscr,(str),(n)))
#define mvwaddch(w,y,x,c)       (wmove( w, y, x )==ERR?ERR:waddch( w, c ))
#define mvwaddchstr(w,y,x,c)    (wmove( w, y, x )==ERR?ERR:waddchnstr( w, c, -1 ))
#define mvwaddchnstr(w,y,x,c,n) (wmove( w, y, x )==ERR?ERR:waddchnstr( w, c, n ))
#define mvwaddrawch(w,y,x,c)    (wmove( w, y, x )==ERR?ERR:waddrawch( w, c ))
#define mvwaddrawstr(w,y,x,str) (wmove( w, y, x )==ERR?ERR:waddrawstr( w, str ))
#define mvwaddstr(w,y,x,str)    (wmove( w, y, x )==ERR?ERR:waddstr( w, str ))
#define mvwdelch(w,y,x)         (wmove( w, y, x )==ERR?ERR:wdelch( w ))
#define mvwgetch(w,y,x)         (wmove( w, y, x )==ERR?ERR:wgetch( w ))
#define mvwgetstr(w,y,x,str)    (wmove( w, y, x )==ERR?ERR:wgetstr( w, str ))
#define mvwinch(w,y,x)          (wmove( w, y, x )==ERR?ERR:((w)->_y[y][x]))
#define mvwinchstr(w,y,x,c)     (wmove( w, y, x )==ERR?ERR:winchnstr( w, c, (w)->_maxx-(w)->_curx ))
#define mvwinchnstr(w,y,x,c,n)  (wmove( w, y, x )==ERR?ERR:winchnstr( w, c, n ))
#define mvwinsch(w,y,x,c)       (wmove( w, y, x )==ERR?ERR:winsch( w, c ))
#define mvwinstr(w,y,x,str)     (wmove( w, y, x )==ERR?ERR:winnstr(w,str,(w)->_maxx))
#define mvwinnstr(w,y,x,str,n)  (wmove( w, y, x )==ERR?ERR:winnstr(w,str,n))
#define mvwinsnstr(w,y,x,s,n)   (wmove( w, y, x )==ERR?ERR:winsnstr(w,s,n))
#define mvwinsstr(w,y,x,s)      (wmove( w, y, x )==ERR?ERR:winsnstr(w,s,(-1)))
#define napms(ms)               delay_output(ms)
#define nl()                    (SP->autocr = TRUE)
#define nonl()                  (SP->autocr = FALSE)
#define pechochar(w,c)          (waddch(w,(chtype)c)==ERR?ERR:prefresh(w))
#define redrawwin(w)            wredrawln((w),0,(win)->_maxy)
#define refresh()               wrefresh( stdscr )
#define resetterm()             reset_shell_mode()
#define saveterm()              def_prog_mode()
#define scrl(n)                 wscrl(stdscr,n)
#define scroll(w)               wscrl((w),1)
#define scrollok(w,flag)        ((w)->_scroll  = flag)
#define setscrreg(top, bot)     wsetscrreg( stdscr, top, bot )
#define standend()              wattrset(stdscr, A_NORMAL)
#define standout()              wattrset(stdscr, A_STANDOUT)
#define timeout(n)              wtimeout( stdscr, n )
#define touchline(w,y,n)        wtouchln((w),(y),(n),TRUE)
#define touchwin(w)             wtouchln((w),0,(w)->_maxy,TRUE)
#if defined (CURSES_LIBRARY)
# define traceoff()             {trace_on = FALSE;}
# define traceon()              {trace_on = TRUE;}
#endif
#define ungetch(ch)             PDC_ungetch(ch)
#define untouchwin(w)           wtouchln((w),0,((w)->_maxy),FALSE)
#define waddch(w, c)            PDC_chadd( w, (chtype)c, (bool)!(SP->raw_out), TRUE )
#define waddchstr(w, c)         (waddchnstr( w, c, -1 ))
#define werase(w)               (wmove((w),0,0), wclrtobot(w))
#define wclear(w)               ((w)->_clear = TRUE , werase(w))
#define wechochar(w,c)          (waddch(w,(chtype)c)==ERR?ERR:wrefresh(w))
#define winch(w)                ((w)->_y[(w)->_cury][(w)->_curx])
#define winchstr(w, c)          (winchnstr( w, c, (w)->_maxx-(w)->_curx ) )
#define winsstr(w,str)          winsnstr((w),(str),(-1))
#define winstr(w,str)           winnstr((w),str,(w)->_maxx)
#define wstandend(w)            wattrset(w, A_NORMAL)
#define wstandout(w)            wattrset(w, A_STANDOUT)

#if !defined(UNIX) && !defined(XCURSES)
# define nocbreak()             (SP->cbreak = FALSE)
# define cbreak()               (SP->cbreak = TRUE)
# define nocrmode()             (SP->cbreak = FALSE)
# define crmode()               (SP->cbreak = TRUE)
# define noecho()               (SP->echo = FALSE)
# define echo()                 (SP->echo = TRUE)
# define nodelay(w,flag)        (w->_nodelay = flag)
#endif

#if defined(PDCURSES)
# define addrawch( c )          waddrawch( stdscr, c )
# define addrawstr(str)         waddrawstr( stdscr, str )
# define insrawch( c )          winsrawch( stdscr, c )
# define waddrawch(w, c)        PDC_chadd( w, (chtype)c, FALSE, TRUE )
# define winsrawch(w, c)        PDC_chins( w, (chtype)c, FALSE )
# define PDC_save_key_modifiers(flag) (SP->save_key_modifiers = flag)
# define PDC_return_key_modifiers(flag) (SP->return_key_modifiers = flag)

/*
 *      FYI: Need to document these functions...
 */
# define title(s,a)             wtitle( stdscr, s, (chtype)a )
# define titleofs(ofs)          wtitleofs( stdscr, ofs )
# define wtitle(w,s,a)          (w->_title = s, w->_title_attr = (chtype)a)
# define wtitleofs(w,ofs)       (w->_title_ofs = ofs)
#endif

/*
 * return codes from PDC_getclipboard() and PDC_setclipboard() calls.
 */
#define PDC_CLIP_SUCCESS        0
#define PDC_CLIP_ACCESS_ERROR   1
#define PDC_CLIP_EMPTY          2
#define PDC_CLIP_MEMORY_ERROR   3

/*
 * PDCurses key modifier masks
 */
#define PDC_KEY_MODIFIER_SHIFT     (1 << 0)
#define PDC_KEY_MODIFIER_CONTROL   (1 << 1)
#define PDC_KEY_MODIFIER_ALT       (1 << 2)
#define PDC_KEY_MODIFIER_NUMLOCK   (1 << 3)

/*
 *      Load up curspriv.h.     This should be in the same place as
 *      stdlib.h.  We allow anyone who defines CURSES_LIBRARY to have
 *      access to our internal routines.  This provides quick
 *      PC applications at the expense of portability.
 */
#if defined(CURSES_LIBRARY)
# include <curspriv.h>
#endif

#include <stdlib.h>

#endif  /* __PDCURSES__ */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
# undef bool
   }
#endif
