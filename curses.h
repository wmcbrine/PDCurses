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
$Id: curses.h,v 1.73 2006/01/12 06:45:43 wmcbrine Exp $
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
	HC              True if using a Metaware compiler.
	TC              True if using a Borland compiler.
	MSC             True if using a Microsoft compiler.
	HAVE_PROTO      True if the compiler supports ANSI prototypes.
	CPLUSPLUS       True if the compiler supports C++.
	PDC_BUILD       Defines API build version.

PDCurses portable platform definitions list:

	PDCURSES        Enables access to PDCurses-only routines.
	XOPEN           Always true.
	SYSVcurses      True if you are compiling for SYSV portability.
	BSDcurses       True if you are compiling for BSD portability.
	PDCURSES_WCLR   Makes behaviour of wclrtoeol() and wclrtoeof()
	                unique to PDCurses. By default Unix behavior is set.
	                See notes in wclrtoeol() and wclrtoeof().
**man-end**********************************************************************/

#define PDC_BUILD 2703
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
#  ifndef HAVE_STRING_H
#    define HAVE_STRING_H
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
#  ifndef __OS2__
#    define __OS2__                    /* EMX does not define this :-( */
#  endif
#  define OS2 3                     /* Major release of OS/2 supported */
#  define CURSES__32BIT__
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_STRING_H
#    define HAVE_STRING_H
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
*				__DJGPP__
*				GO32 (deprecated)
*				DOS
*/
#ifdef __DJGPP__ 			  /* You may have to define in makefile */
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1         /* DJGPP supports ANSI C prototypes   */
#  endif
#  define DOS	6
#  include <dos.h>
#  define CURSES__32BIT__
#  ifdef _cplusplus
#    define CPLUSPLUS 1
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H                           /* have <stdarg.h> */
#  endif
#  ifndef HAVE_STRING_H
#    define HAVE_STRING_H
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
#  if !defined( WIN32 ) && !defined(XCURSES)
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
#  ifndef HAVE_STRING_H
#    define HAVE_STRING_H
#  endif
#  ifndef HAVE_UNISTD_H
#    define HAVE_UNISTD_H                           /* have <unistd.h> */
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
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
#  ifndef HAVE_STRING_H                             /* have <string.h> */
#    define HAVE_STRING_H
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
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H                           /* have <limits.h> */
#  endif
#  ifndef HAVE_STRING_H                             /* have <string.h> */
#    define HAVE_STRING_H
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
#    ifndef  DISPLAY_MONOCHROME
#     define DISPLAY_MONOCHROME 0
#    endif
#    ifndef  DISPLAY_CGA
#     define DISPLAY_CGA        1
#    endif
#    ifndef  DISPLAY_EGA
#     define DISPLAY_EGA        2
#    endif
#    ifndef  DISPLAY_VGA
#     define DISPLAY_VGA        3
#    endif
#    ifndef  DISPLAY_8514A
#     define DISPLAY_8514A      7
#    endif
#    ifndef  DISPLAY_XGA
#     define DISPLAY_XGA        9
#    endif
#  endif
#  if defined( __NT__ ) || defined ( WIN32 )
#    ifdef INCLUDE_WINDOWS_H
#      include <windows.h>
#      ifdef MOUSE_MOVED
#        undef MOUSE_MOVED
#      endif
#    endif
#    ifndef WIN32
#      define WIN32
#    endif
#    ifdef PDC_CDECL
#      undef PDC_CDECL
#      define PDC_CDECL cdecl   /* needed for compatibility to other systems */
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
#  ifndef HAVE_STRING_H
#    define HAVE_STRING_H
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE                             /* have memmove() */
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF                             /* have vsscanf() */
#  endif
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

/*
* HI-TECH COMPILERS Pacific C
*
* Hi-Tech definitions:
*  PC
*
*  __PACIFIC__, DOS
*  are predefined by compiler.
*/
#ifdef __PACIFIC__
#  define PC    __PACIFIC__
#  ifndef HAVE_PROTO
#    define HAVE_PROTO 1
#  endif
#  include <dos.h>
#  include <ctype.h>
#  include <string.h>
#  include <stdlib.h>
#  ifndef __SMALL__
#    define __SMALL__
#  endif
#  ifndef HAVE_LIMITS_H
#    define HAVE_LIMITS_H
#  endif
#  ifndef HAVE_STDARG_H
#    define HAVE_STDARG_H
#  endif
#  ifndef HAVE_MEMMOVE
#    define HAVE_MEMMOVE
#  endif
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF
#  endif
#endif

/*---------------------------------------------------------------------*/
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
#  define NULL	(void*)0	/* Null pointer		 */
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
#define WHEEL_SCROLLED         0006  /* PDCurses enhancement */
#define BUTTON_ACTION_MASK     0007  /* PDCurses enhancement */
#define BUTTON_SHIFT           0010  /* PDCurses enhancement */
#define BUTTON_CONTROL         0020  /* PDCurses enhancement */
#define BUTTON_ALT             0040  /* PDCurses enhancement */
#define BUTTON_MODIFIER_MASK   0070  /* PDCurses enhancement */

#define MOUSE_X_POS            (Mouse_status.x)
#define MOUSE_Y_POS            (Mouse_status.y)
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
#define PDC_MOUSE_MOVED         8
#define PDC_MOUSE_POSITION     16
#define PDC_MOUSE_WHEEL_UP     32
#define PDC_MOUSE_WHEEL_DOWN   64
#define A_BUTTON_CHANGED       (Mouse_status.changes & 7)
#define MOUSE_MOVED            (Mouse_status.changes & PDC_MOUSE_MOVED)
#define MOUSE_POS_REPORT       (Mouse_status.changes & PDC_MOUSE_POSITION)
#define BUTTON_CHANGED(x)      (Mouse_status.changes & (1 << ((x) - 1)))
#define BUTTON_STATUS(x)       (Mouse_status.button[(x)-1])
#define MOUSE_WHEEL_UP         (Mouse_status.changes & PDC_MOUSE_WHEEL_UP)
#define MOUSE_WHEEL_DOWN       (Mouse_status.changes & PDC_MOUSE_WHEEL_DOWN)

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
#define MOUSE_WHEEL_SCROLL      000000100000L /* PDCurses enhancement */
#define ALL_MOUSE_EVENTS        000000777777L
#define BUTTON_MODIFIER_SHIFT   000001000000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_CONTROL 000002000000L /* PDCurses enhancement */
#define BUTTON_MODIFIER_ALT     000004000000L /* PDCurses enhancement */
#define REPORT_MOUSE_POSITION   000010000000L

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
	int	_delayms;	/* milliseconds of delay for getch()	*/
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
#ifdef HAVE_PROTO
	int (*init)(WINDOW *, int);
#else
	int (*init)();
#endif
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
	attr_t	orig_attr;	/* Original screen attributes - top 16 bits background, bottom 16 bits foreground */
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
	int	_restore;		/* specifies if screen background to be restored and how*/
	bool	save_key_modifiers;		/* TRUE if each key modifiers saved with each key press */
	bool	return_key_modifiers;		/* TRUE if modifier keys are returned as "real" keys */

#ifdef OS2
# ifdef EMXVIDEO			/* nop if using EMX builtins */
	int tahead; 			/* Type-ahead value */
	int adapter;			/* Screen type	*/
# else
	VIOMODEINFO scrnmode;	/* default screen mode			*/
	VIOCONFIGINFO adapter;	/* Screen type				*/
	KBDINFO kbdinfo;	/* keyboard info */
# endif
#else
	int adapter;			/* Screen type	*/
#endif

#if defined(DOS) || defined(WIN32)
	int	scrnmode;	/* default screen mode		    */
	unsigned video_seg;	/* video base segment		    */
	unsigned video_ofs;	/* video base offset		    */
	unsigned long os_version; /* Win32 Version */
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
#if defined(PDC_DLL_BUILD)
# if !defined(CURSES_LIBRARY)
__declspec(dllimport)	int	LINES;		/* terminal height		*/
__declspec(dllimport)	int	COLS;		/* terminal width		*/
__declspec(dllimport)	WINDOW*	stdscr;		/* the default screen window	*/
__declspec(dllimport)	WINDOW*	curscr;		/* the current screen image	*/
__declspec(dllimport)	SCREEN	*SP;	/* curses variables		*/
__declspec(dllimport)	int	use_emalloc;	/* set to true to use your own malloc,etc */
__declspec(dllimport)	MOUSE_STATUS Mouse_status;
__declspec(dllimport)	int COLORS;
__declspec(dllimport)	int COLOR_PAIRS;
# else
__declspec(dllexport) extern	int	LINES;		/* terminal height		*/
__declspec(dllexport) extern	int	COLS;		/* terminal width		*/
__declspec(dllexport) extern	WINDOW*	stdscr;		/* the default screen window	*/
__declspec(dllexport) extern	WINDOW*	curscr;		/* the current screen image	*/
__declspec(dllexport) extern	SCREEN	*SP;	/* curses variables		*/
__declspec(dllexport) extern	int	use_emalloc;	/* set to true to use your own malloc,etc */
__declspec(dllexport) extern	MOUSE_STATUS Mouse_status;
__declspec(dllexport) extern	int COLORS;
__declspec(dllexport) extern	int COLOR_PAIRS;
# endif
#else
extern	int	LINES;		/* terminal height		*/
extern	int	COLS;		/* terminal width		*/
extern	WINDOW*	stdscr;		/* the default screen window	*/
extern	WINDOW*	curscr;		/* the current screen image	*/
extern	SCREEN	*SP;	/* curses variables		*/
extern	int	use_emalloc;	/* set to true to use your own malloc,etc */
extern	MOUSE_STATUS Mouse_status;
extern	int COLORS,COLOR_PAIRS;
#endif

#if	defined (CURSES_LIBRARY)
extern	int	_default_lines;	/* For presetting maximum lines	*/
#endif

#ifdef	REGISTERWINDOWS
extern	ACTIVE*	CurWins;	/* Currently Visible Windows	*/
#endif




/*man-start*********************************************************************

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

-------------------------------------------------------------------------------------------------
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
-------------------------------------------------------------------------------------------------
     colour number      |     modifiers         |      character eg 'a' (potential for DBCS)

The available attribute modifiers are bold, underline, invisible, 
protect, reverse and blink. 256 (8 bits) colour pairs, 8 bits for 
modifiers, and 16 bits for character data. (In practice, only 64 colour 
pairs are available, and only the lower 8 bits of character data are
currently used.)

**man-end**********************************************************************/

/*** Video attribute macros ***/

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
# define A_COLOR       (chtype)0xF800                  /* System V */
# define A_CHARTEXT    (chtype)(0xFF)                  /* X/Open   */
# define A_ATTRIBUTES  (chtype)(~A_CHARTEXT)           /* X/Open   */

# define A_LEFTLINE    (chtype)0x0000
# define A_RIGHTLINE   (chtype)0x0000
# define A_ITALIC      (chtype)0x0000
# define A_INVIS       (chtype)0x0000
#endif

#define CHR_MSK		A_CHARTEXT			/* Obsolete */
#define ATR_MSK		A_ATTRIBUTES			/* Obsolete */
#define ATR_NRM		A_NORMAL			/* Obsolete */

/*** Alternate character set macros ***/

/* ALTCHARSET definitions for PC from jshumate@wrdis01.robins.af.mil
   These match code page 437 and compatible pages (CP850, CP852, etc.)
*/

#ifdef XCURSES
extern chtype *acs_map;
#endif

/* VT100-compatible symbols -- box chars */

#ifdef XCURSES
# define ACS_ULCORNER	(acs_map['l'])
# define ACS_LLCORNER	(acs_map['m'])
# define ACS_URCORNER	(acs_map['k'])
# define ACS_LRCORNER	(acs_map['j'])
# define ACS_RTEE	(acs_map['u'])
# define ACS_LTEE	(acs_map['t'])
# define ACS_BTEE	(acs_map['v'])
# define ACS_TTEE	(acs_map['w'])
# define ACS_HLINE	(acs_map['q'])
# define ACS_VLINE	(acs_map['x'])
# define ACS_PLUS	(acs_map['n'])
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
# define ACS_S1		(acs_map['o'])
# define ACS_S9		(acs_map['s'])
# define ACS_DIAMOND	(acs_map['\''])
# define ACS_CKBOARD	(acs_map['a'])
# define ACS_DEGREE	(acs_map['f'])
# define ACS_PLMINUS	(acs_map['g'])
# define ACS_BULLET	(acs_map['~'])
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
   for optimum portability.
*/
#ifdef XCURSES
# define ACS_LARROW	(acs_map[','])
# define ACS_RARROW	(acs_map['+'])
# define ACS_DARROW	(acs_map['.'])
# define ACS_UARROW	(acs_map['-'])
# define ACS_BOARD	(acs_map['h'])
# define ACS_LANTERN	(acs_map['i'])
# define ACS_BLOCK	(acs_map['0'])
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
   code pages as those above.
*/
#ifdef XCURSES
# define ACS_S3		(acs_map['p'])
# define ACS_S7		(acs_map['r'])
# define ACS_LEQUAL	(acs_map['y'])
# define ACS_GEQUAL	(acs_map['z'])
# define ACS_PI		(acs_map['{'])
# define ACS_NEQUAL	(acs_map['|'])
# define ACS_STERLING	(acs_map['}'])
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

#if defined(XCURSES)	/* RGB */
# define COLOR_RED	1
# define COLOR_GREEN	2
# define COLOR_BLUE	4
#else			/* BGR */
# define COLOR_BLUE	1
# define COLOR_GREEN	2
# define COLOR_RED	4
#endif

#define COLOR_CYAN      (COLOR_BLUE | COLOR_GREEN)
#define COLOR_MAGENTA   (COLOR_RED | COLOR_BLUE)
#define COLOR_YELLOW    (COLOR_RED | COLOR_GREEN)

#define COLOR_WHITE	7

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

/* PDCurses-specific key definitions -- PC only */

#define ALT_0           0x197
#define ALT_1           0x198
#define ALT_2           0x199
#define ALT_3           0x19a
#define ALT_4           0x19b
#define ALT_5           0x19c
#define ALT_6           0x19d
#define ALT_7           0x19e
#define ALT_8           0x19f
#define ALT_9           0x1a0
#define ALT_A           0x1a1
#define ALT_B           0x1a2
#define ALT_C           0x1a3
#define ALT_D           0x1a4
#define ALT_E           0x1a5
#define ALT_F           0x1a6
#define ALT_G           0x1a7
#define ALT_H           0x1a8
#define ALT_I           0x1a9
#define ALT_J           0x1aa
#define ALT_K           0x1ab
#define ALT_L           0x1ac
#define ALT_M           0x1ad
#define ALT_N           0x1ae
#define ALT_O           0x1af
#define ALT_P           0x1b0
#define ALT_Q           0x1b1
#define ALT_R           0x1b2
#define ALT_S           0x1b3
#define ALT_T           0x1b4
#define ALT_U           0x1b5
#define ALT_V           0x1b6
#define ALT_W           0x1b7
#define ALT_X           0x1b8
#define ALT_Y           0x1b9
#define ALT_Z           0x1ba
#define CTL_LEFT        0x1bb   /* Control-Left-Arrow    */
#define CTL_RIGHT       0x1bc   /* Control-Right-Arrow   */
#define CTL_PGUP        0x1bd   /* Control-PgUp          */
#define CTL_PGDN        0x1be   /* Control-PgDn          */
#define CTL_HOME        0x1bf   /* Control-Home          */
#define CTL_END         0x1c0   /* Control-End           */

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
# define ALT_PADSTOP     0x1dc   /* alt-stop on keypad            */
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
# define PAD0            0x1fa   /* keypad 0                      */
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
# define ALT_PAD0        0x205   /* alt-keypad 0                  */
# define ALT_PAD1        0x206   /* alt-keypad 1                  */
# define ALT_PAD2        0x207   /* alt-keypad 2                  */
# define ALT_PAD3        0x208   /* alt-keypad 3                  */
# define ALT_PAD4        0x209   /* alt-keypad 4                  */
# define ALT_PAD5        0x20a   /* alt-keypad 5                  */
# define ALT_PAD6        0x20b   /* alt-keypad 6                  */
# define ALT_PAD7        0x20c   /* alt-keypad 7                  */
# define ALT_PAD8        0x20d   /* alt-keypad 8                  */
# define ALT_PAD9        0x20e   /* alt-keypad 9                  */
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
# define Args(x) x
#else
# define Args(x) ()
#endif

int     PDC_CDECL addchnstr Args(( const chtype *, int ));
int     PDC_CDECL baudrate Args(( void ));
int     PDC_CDECL beep Args(( void ));
int     PDC_CDECL border Args(( chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype ));
int     PDC_CDECL can_change_color  Args(( void ));
int     PDC_CDECL cbreak Args(( void ));
int     PDC_CDECL clearok Args(( WINDOW*, bool ));
int     PDC_CDECL color_content Args(( short, short*, short*, short* ));
int     PDC_CDECL copywin Args(( const WINDOW*, WINDOW*, int, int, int, int, int, int, int ));
int     PDC_CDECL curs_set Args(( int ));
int     PDC_CDECL def_prog_mode Args(( void ));
int     PDC_CDECL def_shell_mode Args(( void ));
void    PDC_CDECL delscreen Args(( SCREEN * ));
int     PDC_CDECL delwin Args(( WINDOW* ));
WINDOW* PDC_CDECL derwin Args(( WINDOW*, int, int, int, int ));
int     PDC_CDECL doupdate Args(( void ));
WINDOW* PDC_CDECL dupwin Args(( WINDOW* ));
int     PDC_CDECL endwin Args(( void ));
int     PDC_CDECL erase Args(( void ));
char    PDC_CDECL erasechar Args(( void ));
void    PDC_CDECL filter Args(( void ));
int     PDC_CDECL flash Args(( void ));
int     PDC_CDECL flushinp Args(( void ));
attr_t  PDC_CDECL getattrs Args(( WINDOW* ));
int     PDC_CDECL getsyx Args(( int*, int* ));
WINDOW* PDC_CDECL getwin Args(( FILE * ));
int     PDC_CDECL halfdelay Args(( int ));
bool    PDC_CDECL has_colors Args(( void ));
bool 	PDC_CDECL has_ic Args((void));
bool 	PDC_CDECL has_il Args((void));
bool 	PDC_CDECL has_key Args((  int  ));
int     PDC_CDECL hline Args(( chtype, int ));
int     PDC_CDECL idlok Args(( WINDOW *win, bool bf ));
int     PDC_CDECL idcok Args(( WINDOW *win, bool bf ));
int     PDC_CDECL immedok Args(( WINDOW*, bool ));
int     PDC_CDECL inchnstr Args(( chtype *, int ));
int     PDC_CDECL init_color Args(( short, short, short, short ));
int     PDC_CDECL init_pair Args(( short, short, short ));
WINDOW* PDC_CDECL initscr Args(( void ));
#ifdef XCURSES
WINDOW* PDC_CDECL Xinitscr Args(( int, char ** ));
#endif
int     PDC_CDECL intrflush Args((  WINDOW*, bool  ));
bool    PDC_CDECL is_linetouched Args((WINDOW *,int));
bool    PDC_CDECL is_wintouched Args((WINDOW *));
char*   PDC_CDECL keyname Args((  int  ));
char    PDC_CDECL killchar Args(( void ));
char*   PDC_CDECL longname Args(( void ));
int     PDC_CDECL meta Args(( WINDOW*, bool ));
int     PDC_CDECL move Args(( int, int ));
int     PDC_CDECL mvcur Args(( int, int, int, int ));
int     PDC_CDECL mvderwin Args(( WINDOW*, int, int ));
#ifdef HAVE_STDARG_H
int     PDC_CDECL mvprintw Args(( int, int, char*,... ));
int     PDC_CDECL mvscanw Args(( int, int, char*,... ));
#endif
int     PDC_CDECL mvwaddnstr Args(( WINDOW*,int,int,const char*,int ));
int     PDC_CDECL mvwin Args(( WINDOW*, int, int ));
chtype  PDC_CDECL mvwinch Args(( WINDOW*, int, int ));
int     PDC_CDECL mvwinsertln Args(( WINDOW*, int, int ));
#ifdef HAVE_STDARG_H
int     PDC_CDECL mvwprintw Args(( WINDOW*, int, int, char*,... ));
int     PDC_CDECL mvwscanw Args(( WINDOW*, int, int, char*,... ));
#endif
int     PDC_CDECL napms Args(( int ));
WINDOW* PDC_CDECL newpad Args(( int, int ));
SCREEN* PDC_CDECL newterm Args(( char*, FILE*, FILE* ));
WINDOW* PDC_CDECL newwin Args(( int, int, int, int ));
int     PDC_CDECL noraw Args(( void ));
void    PDC_CDECL noqiflush Args(( void ));
int     PDC_CDECL notimeout Args(( WINDOW *, bool ));
int     PDC_CDECL overlay Args(( const WINDOW*, WINDOW* ));
int     PDC_CDECL overwrite Args(( const WINDOW*, WINDOW* ));
int     PDC_CDECL pair_content Args(( int, short*, short* ));
int     PDC_CDECL pechochar Args(( WINDOW *, chtype ));
int     PDC_CDECL pnoutrefresh Args(( WINDOW*, int, int, int, int, int, int ));
int     PDC_CDECL prefresh Args(( WINDOW*, int, int, int, int, int, int ));
#ifdef HAVE_STDARG_H
int     PDC_CDECL printw Args(( char*,... ));
int     PDC_CDECL scanw Args(( char*,... ));
#endif
int     PDC_CDECL putwin Args(( WINDOW *, FILE * ));
void    PDC_CDECL qiflush Args(( void ));
int     PDC_CDECL raw Args(( void ));
int     PDC_CDECL refresh Args(( void ));
int     PDC_CDECL reset_prog_mode Args(( void ));
int     PDC_CDECL reset_shell_mode Args(( void ));
int     PDC_CDECL resetty Args(( void ));
int     PDC_CDECL ripoffline Args((int, int (*)(WINDOW *, int)));
int     PDC_CDECL savetty Args(( void ));
int     PDC_CDECL sb_init Args((void));
int     PDC_CDECL sb_set_horz Args((int, int, int));
int     PDC_CDECL sb_set_vert Args((int, int, int));
int     PDC_CDECL sb_get_horz Args((int *, int *, int *));
int     PDC_CDECL sb_get_vert Args((int *, int *, int *));
int     PDC_CDECL sb_refresh Args((void));
int     PDC_CDECL scr_dump Args((const char *));
int     PDC_CDECL scr_init Args((const char *));
int     PDC_CDECL scr_restore Args((const char *));
int     PDC_CDECL scr_set Args((const char *));
int     PDC_CDECL scroll Args(( WINDOW* ));
SCREEN* PDC_CDECL set_term Args(( SCREEN* ));
int     PDC_CDECL setsyx Args(( int, int ));
int     PDC_CDECL start_color Args(( void ));
int     PDC_CDECL slk_init Args((int));
int     PDC_CDECL slk_set Args((int, const char *, int));
int     PDC_CDECL slk_refresh Args((void));
int     PDC_CDECL slk_noutrefresh Args((void));
char*   PDC_CDECL slk_label Args((int));
int     PDC_CDECL slk_clear Args((void));
int     PDC_CDECL slk_restore Args((void));
int     PDC_CDECL slk_touch Args((void));
int     PDC_CDECL slk_attron Args((attr_t));
int     PDC_CDECL slk_attrset Args((attr_t));
int     PDC_CDECL slk_attroff Args((attr_t));
int     PDC_CDECL slk_color Args((short));
WINDOW* PDC_CDECL subpad Args(( WINDOW*, int, int, int, int ));
WINDOW* PDC_CDECL subwin Args(( WINDOW*, int, int, int, int ));
int     PDC_CDECL syncok Args((WINDOW*, bool));
attr_t  PDC_CDECL termattrs Args(( void ));
char*   PDC_CDECL termname Args(( void ));
int     PDC_CDECL touchline Args(( WINDOW*, int ,int ));
int     PDC_CDECL touchwin Args(( WINDOW* ));
#if !defined (CURSES_LIBRARY)
void    PDC_CDECL traceoff Args(( void ));
void    PDC_CDECL traceon Args(( void ));
#endif
int     PDC_CDECL typeahead Args(( int ));
char*   PDC_CDECL unctrl Args(( chtype ));
void    PDC_CDECL use_env Args(( bool ));
int     PDC_CDECL vidattr Args(( chtype ));
int     PDC_CDECL vidputs Args(( chtype, int (*)(int) ));
int     PDC_CDECL vline Args(( chtype, int ));
int     PDC_CDECL waddch Args(( WINDOW*, const chtype ));
int     PDC_CDECL waddchnstr Args(( WINDOW*, const chtype*, int ));
int     PDC_CDECL waddnstr Args(( WINDOW*, const char*, int ));
int     PDC_CDECL waddstr Args(( WINDOW*, const char* ));
int     PDC_CDECL wattroff Args(( WINDOW*, attr_t ));
int     PDC_CDECL wattron Args(( WINDOW*, attr_t ));
int     PDC_CDECL wattrset Args(( WINDOW*, attr_t ));
int     PDC_CDECL wbkgd Args((WINDOW*, chtype));
void    PDC_CDECL wbkgdset Args((WINDOW*, chtype));
int     PDC_CDECL wborder Args(( WINDOW*, chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype ));
int     PDC_CDECL wclear Args(( WINDOW* ));
int     PDC_CDECL wclrtobot Args(( WINDOW* ));
int     PDC_CDECL wclrtoeol Args(( WINDOW* ));
int	PDC_CDECL wcolor_set Args(( WINDOW*, short, void* ));
void    PDC_CDECL wcursyncup Args((WINDOW*));
int     PDC_CDECL wdelch Args(( WINDOW* ));
int     PDC_CDECL wdeleteln Args(( WINDOW* ));
int     PDC_CDECL werase Args(( WINDOW* ));
int     PDC_CDECL wgetch Args(( WINDOW* ));
int     PDC_CDECL wgetnstr Args(( WINDOW*, char*, int ));
int     PDC_CDECL wgetstr Args(( WINDOW*, char* ));
int     PDC_CDECL whline Args(( WINDOW*, chtype, int ));
int     PDC_CDECL winchnstr Args(( WINDOW*, chtype*, int ));
int     PDC_CDECL winnstr Args(( WINDOW*, char*, int ));
int     PDC_CDECL winsch Args(( WINDOW*, chtype ));
int     PDC_CDECL winsdelln Args(( WINDOW*, int ));
int     PDC_CDECL winsertln Args(( WINDOW* ));
int     PDC_CDECL winsnstr Args(( WINDOW*, const char*, int ));
int     PDC_CDECL wmove Args(( WINDOW*, int, int ));
int     PDC_CDECL wnoutrefresh Args(( WINDOW* ));
char    PDC_CDECL wordchar Args(( void ));
#ifdef HAVE_STDARG_H
int     PDC_CDECL wprintw Args(( WINDOW*, char*,... ));
int     PDC_CDECL wscanw Args(( WINDOW*, char*,... ));
#endif
int     PDC_CDECL wredrawln Args(( WINDOW*, int ,int ));
int     PDC_CDECL wrefresh Args(( WINDOW* ));
/* int     PDC_CDECL wresize Args((WINDOW **win, int lins, int cols)); */
int     PDC_CDECL wscrl Args(( WINDOW*, int ));
int     PDC_CDECL wsetscrreg Args(( WINDOW*, int, int ));
int     PDC_CDECL wtimeout Args(( WINDOW *, int ));
int     PDC_CDECL wtouchln Args((WINDOW *, int, int, int));
void    PDC_CDECL wsyncdown Args((WINDOW*));
void    PDC_CDECL wsyncup Args((WINDOW*));
int     PDC_CDECL wvline Args(( WINDOW*, chtype, int ));

#ifdef  PDCURSES
int     PDC_CDECL raw_output Args(( bool ));
int     PDC_CDECL resize_term Args(( int, int ));
WINDOW* PDC_CDECL resize_window Args(( WINDOW*, int, int ));
#endif

#ifdef  XCURSES
void    PDC_CDECL XCursesExit Args(( void ));
int     PDC_CDECL nocbreak Args(( void ));
int     PDC_CDECL cbreak Args(( void ));
int     PDC_CDECL nocrmode Args(( void ));
int     PDC_CDECL crmode Args(( void ));
int     PDC_CDECL noecho Args(( void ));
int     PDC_CDECL echo Args(( void ));
int     PDC_CDECL nodelay Args(( WINDOW *,bool ));
#endif

int     PDC_CDECL mouse_set Args(( unsigned long ));
int     PDC_CDECL mouse_on Args(( unsigned long ));
int     PDC_CDECL mouse_off Args(( unsigned long ));
int     PDC_CDECL request_mouse_pos Args(( void ));
int     PDC_CDECL map_button Args(( unsigned long ));
void    PDC_CDECL wmouse_position Args(( WINDOW *, int *, int * ));
unsigned long PDC_CDECL getmouse Args(( void ));
unsigned long PDC_CDECL getbmap Args(( void ));

/*
*       Keep the compiler happy with our macros below...
*/
int     PDC_CDECL PDC_chadd Args(( WINDOW*, chtype, bool, bool ));
int     PDC_CDECL PDC_chins Args(( WINDOW*, chtype, bool ));
int     PDC_CDECL PDC_ungetch Args(( int ));

void    PDC_CDECL PDC_set_title Args(( char * ));

int     PDC_CDECL PDC_getclipboard Args(( char **, long * ));
int     PDC_CDECL PDC_setclipboard Args(( char *, long ));
int     PDC_CDECL PDC_freeclipboard Args(( char * ));
int     PDC_CDECL PDC_clearclipboard Args(( void ));

unsigned long PDC_CDECL PDC_get_input_fd Args(( void ));
int     PDC_CDECL PDC_curs_set Args(( int ));
unsigned long PDC_CDECL PDC_get_key_modifiers Args(( void ));

int     PDC_CDECL PDC_wunderline Args(( WINDOW*, int, bool ));
int     PDC_CDECL PDC_wleftline Args(( WINDOW*, int, bool ));
int     PDC_CDECL PDC_wrightline Args(( WINDOW*, int, bool ));
int     PDC_CDECL PDC_set_line_color Args(( short ));

#undef Args

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
#define color_set(a,b)          wcolor_set( stdscr, a, b )
#define delay_output(ms)        napms( ms )
#define delch()                 wdelch( stdscr )
#define deleteln()              wdeleteln( stdscr )
#define derwin(w,nl,nc,by,bx)   subwin((w),(nl),(nc),(by+(w)->_begy),(bx+(w)->_begx))
#define draino(ms)              napms( ms )
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
#define getsyx(y,x)             { if( curscr->_leaveit) (y)=(x)=-1; else getyx(curscr,(y),(x)); }
#define getyx(w,y,x)            ( y = (w)->_cury, x = (w)->_curx )
#define has_colors()            ((SP->mono) ? FALSE : TRUE)
/*#define idcok(w,flag)           OK*/
/*#define idlok(w,flag)           OK*/
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
#define keypad(w,flag)          (w->_use_keypad  = flag, OK)
#define leaveok(w,flag)         (w->_leaveit = flag, OK)
#define move(y,x)               wmove( stdscr, y, x )
#define mvaddch(y,x,c)          (move( y, x )==ERR?ERR:addch( c ))
#define mvaddchstr(y,x,c)       (move( y, x )==ERR?ERR:addchnstr( c, -1 ))
#define mvaddchnstr(y,x,c,n)    (move( y, x )==ERR?ERR:addchnstr( c, n ))
#define mvaddstr(y,x,str)       (move( y, x )==ERR?ERR:addstr( str ))
#define mvaddnstr(y,x,str,n)    (move( y, x )==ERR?ERR:addnstr( str, n ))
#define mvdelch(y,x)            (move( y, x )==ERR?ERR:wdelch( stdscr ))
#define mvgetch(y,x)            (move( y, x )==ERR?ERR:wgetch(stdscr))
#define mvgetstr(y,x,str)       (move( y, x )==ERR?ERR:wgetstr( stdscr, str ))
#define mvhline(y,x,c,n)        (move( y, x )==ERR?ERR:hline( c, n ))
#define mvinch(y,x)             (move( y, x )==ERR?((chtype)ERR):(stdscr->_y[y][x]))
#define mvinchstr(y,x,c)        (move( y, x )==ERR?ERR:inchnstr( c, stdscr->_maxx-stdscr->_curx ))
#define mvinchnstr(y,x,c,n)     (move( y, x )==ERR?ERR:inchnstr( c, n ))
#define mvinsch(y,x,c)          (move( y, x )==ERR?ERR:winsch( stdscr, c ))
#define mvinsnstr(y,x,s,n)      (move( y, x )==ERR?ERR:winsnstr(stdscr,s,n))
#define mvinsstr(y,x,s)         (move( y, x )==ERR?ERR:winsnstr(stdscr,s,(-1)))
#define mvinstr(y,x,str)        (move( y, x )==ERR?ERR:winnstr(stdscr,(str),stdscr->_maxx))
#define mvinnstr(y,x,str,n)     (move( y, x )==ERR?ERR:winnstr(stdscr,(str),(n)))
#define mvvline(y,x,c,n)        (move( y, x )==ERR?ERR:vline( c, n ))
#define mvwaddch(w,y,x,c)       (wmove( w, y, x )==ERR?ERR:waddch( w, c ))
#define mvwaddchstr(w,y,x,c)    (wmove( w, y, x )==ERR?ERR:waddchnstr( w, c, -1 ))
#define mvwaddchnstr(w,y,x,c,n) (wmove( w, y, x )==ERR?ERR:waddchnstr( w, c, n ))
#define mvwaddrawch(w,y,x,c)    (wmove( w, y, x )==ERR?ERR:waddrawch( w, c ))
#define mvwaddstr(w,y,x,str)    (wmove( w, y, x )==ERR?ERR:waddstr( w, str ))
#define mvwdelch(w,y,x)         (wmove( w, y, x )==ERR?ERR:wdelch( w ))
#define mvwgetch(w,y,x)         (wmove( w, y, x )==ERR?ERR:wgetch( w ))
#define mvwgetstr(w,y,x,str)    (wmove( w, y, x )==ERR?ERR:wgetstr( w, str ))
#define mvwgetnstr(w,y,x,str,n) (wmove( w, y, x )==ERR?ERR:wgetnstr( w, str , n ))
#define mvwhline(w,y,x,c,n)     (wmove( w, y, x )==ERR?ERR:whline( w, c, n ))
#define mvwinch(w,y,x)          (wmove( w, y, x )==ERR?((chtype)ERR):((w)->_y[y][x]))
#define mvwinchstr(w,y,x,c)     (wmove( w, y, x )==ERR?ERR:winchnstr( w, c, (w)->_maxx-(w)->_curx ))
#define mvwinchnstr(w,y,x,c,n)  (wmove( w, y, x )==ERR?ERR:winchnstr( w, c, n ))
#define mvwinsch(w,y,x,c)       (wmove( w, y, x )==ERR?ERR:winsch( w, c ))
#define mvwinstr(w,y,x,str)     (wmove( w, y, x )==ERR?ERR:winnstr(w,str,(w)->_maxx))
#define mvwinnstr(w,y,x,str,n)  (wmove( w, y, x )==ERR?ERR:winnstr(w,str,n))
#define mvwinsnstr(w,y,x,s,n)   (wmove( w, y, x )==ERR?ERR:winsnstr(w,s,n))
#define mvwinsstr(w,y,x,s)      (wmove( w, y, x )==ERR?ERR:winsnstr(w,s,(-1)))
#define mvwvline(w,y,x,c,n)     (wmove( w, y, x )==ERR?ERR:wvline( w, c, n ))
#define nl()                    (SP->autocr = TRUE, OK)
#define nonl()                  (SP->autocr = FALSE, OK)
#define redrawwin(w)            wredrawln((w),0,(w)->_maxy)
#define refresh()               wrefresh( stdscr )
#define resetterm()             reset_shell_mode()
#define saveterm()              def_prog_mode()
#define scrl(n)                 wscrl(stdscr,n)
#define scroll(w)               wscrl((w),1)
#define scrollok(w,flag)        ((w)->_scroll  = flag)
#define setscrreg(top, bot)     wsetscrreg( stdscr, top, bot )
#define setsyx(y,x)             { if( (y)==-1 && (x)==-1) curscr->_leaveit=TRUE; else { curscr->_leaveit=FALSE;wmove(curscr,(y),(x));} }
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
#define wresize(w,l,c)          ((w = resize_window(w,l,c)) ? OK : ERR)
#define wstandend(w)            wattrset(w, A_NORMAL)
#define wstandout(w)            wattrset(w, A_STANDOUT)

#if !defined(XCURSES)
# define nocbreak()             (SP->cbreak = FALSE, SP->delaytenths = 0, OK)
# define cbreak()               (SP->cbreak = TRUE, OK)
# define nocrmode()             (SP->cbreak = FALSE, OK)
# define crmode()               (SP->cbreak = TRUE, OK)
# define noecho()               (SP->echo = FALSE, OK)
# define echo()                 (SP->echo = TRUE, OK)
# define nodelay(w,flag)        (w->_nodelay = flag, OK)
#endif

#if defined(PDCURSES)
# define addrawch( c )          waddrawch( stdscr, c )
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
#define PDC_KEY_MODIFIER_SHIFT     1
#define PDC_KEY_MODIFIER_CONTROL   2
#define PDC_KEY_MODIFIER_ALT       4
#define PDC_KEY_MODIFIER_NUMLOCK   8

/*
 *      Load up curspriv.h. We allow anyone who defines CURSES_LIBRARY
 *      to have access to our internal routines.
 */
#if defined(CURSES_LIBRARY)
# include <curspriv.h>
#endif

#endif  /* __PDCURSES__ */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
# undef bool
   }
#endif
