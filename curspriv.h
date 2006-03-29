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

/* $Id: curspriv.h,v 1.63 2006/03/29 20:06:40 wmcbrine Exp $ */

/*                         CURSPRIV.H

   Private definitions and declarations for use within PDCurses.
   These should generally not be referenced by applications. */

#ifndef __CURSES_INTERNALS__
#define __CURSES_INTERNALS__ 1

#include <assert.h>

/*----------------------------------------------------------------------
 *	MEMORY MODEL SUPPORT:
 *
 *	MODELS
 *		TINY	cs,ds,ss all in 1 segment (not enough memory!)
 *		SMALL	cs:1 segment, ds:1 segment
 *		MEDIUM	cs:many segments, ds:1 segment
 *		COMPACT	cs:1 segment, ds:many segments
 *		LARGE	cs:many segments, ds:many segments
 *		HUGE	cs:many segments, ds:segments > 64K
 */

#ifdef __TINY__
# define SMALL 1
#endif
#ifdef __SMALL__
# define SMALL 1
#endif
#ifdef __MEDIUM__
# define MEDIUM 1
#endif
#ifdef __COMPACT__
# define COMPACT 1
#endif
#ifdef __LARGE__
# define LARGE 1
#endif
#ifdef __HUGE__
# define HUGE 1
#endif


/*----------------------------------------*/
#ifdef DOS

# include <dos.h>

# ifdef MX386
typedef union REGS16 Regs;
# else
typedef union REGS Regs;
# endif

extern Regs regs;

# ifdef __DJGPP__		/* Note: works only in plain DOS... */
#  if DJGPP == 2
#   define _FAR_POINTER(s,o)	((((int)(s)) << 4) + ((int)(o)))
#  else
#   define _FAR_POINTER(s,o)	(0xe0000000 + (((int)(s)) << 4) + ((int)(o)))
#  endif
#  define _FP_SEGMENT(p)	(unsigned short)((((long)p) >> 4) & 0xffff)
# else
#  ifdef __TURBOC__
#   define _FAR_POINTER(s,o)	MK_FP(s,o)
#  else
#   ifdef MX386
#    define _FAR_POINTER(s,o)	((((int)(s)) << 4) + ((int)(o)))
#   else
#    if defined(__WATCOMC__) && defined(__FLAT__)
#     define _FAR_POINTER(s,o)	((((int)(s)) << 4) + ((int)(o)))
#    else
#     define _FAR_POINTER(s,o)	(((long)s << 16) | (long)o)
#    endif
#   endif
#  endif
#  define _FP_SEGMENT(p)	(unsigned short)(((long)p) >> 4)
# endif
# define _FP_OFFSET(p)		((unsigned short)p & 0x000f)

# ifdef __DJGPP__
unsigned char getdosmembyte(int offs);	/* see: private\_dosmem.c */
unsigned short getdosmemword(int offs);
void setdosmembyte(int offs, unsigned char b);
void setdosmemword(int offs, unsigned short w);
# else
#  if SMALL || MEDIUM || MSC || defined(__PACIFIC__)
#   define getdosmembyte(offs) \
	(*((unsigned char far *) _FAR_POINTER(0,offs)))
#   define getdosmemword(offs) \
	(*((unsigned short far *) _FAR_POINTER(0,offs)))
#   define setdosmembyte(offs,x) \
	(*((unsigned char far *) _FAR_POINTER(0,offs)) = (x))
#   define setdosmemword(offs,x) \
	(*((unsigned short far *) _FAR_POINTER(0,offs)) = (x))
#  else
#   define getdosmembyte(offs) \
	(*((unsigned char *) _FAR_POINTER(0,offs)))
#   define getdosmemword(offs) \
	(*((unsigned short *) _FAR_POINTER(0,offs)))
#   define setdosmembyte(offs,x) \
	(*((unsigned char *) _FAR_POINTER(0,offs)) = (x))
#   define setdosmemword(offs,x) \
	(*((unsigned short *) _FAR_POINTER(0,offs)) = (x))
#  endif
# endif
#endif


/*----------------------------------------------------------------------
 *	MALLOC DEBUGGING SUPPORT:
 *
 *	Set EMALLOC and EMALLOC_MAGIC in order to use your private
 *	versions of malloc(), calloc(), and free().  This can help,
 *	but not solve, your malloc problems when debugging...
 *
 */

#if defined(INTERNAL) || defined(CURSES_LIBRARY)
# define EMALLOC 0		/* Enable/Disable External Malloc	*/
# define EMALLOC_MAGIC  0x0C0C	/* Our magic indicator that we should	*/
				/* use our external malloc rather than	*/
				/* the runtime's malloc.		*/
#else
# define EMALLOC 0		/* Disable External Malloc		*/
#endif


/*----------------------------------------------------------------------*/
/* window properties */

#define _SUBWIN		0x01	/* window is a subwindow		*/
#define _ENDLINE	0x02	/* last winline is last screen line	*/
#define _FULLWIN	0x04	/* window fills screen			*/
#define _SCROLLWIN	0x08	/* window lwr rgt is screen lwr rgt	*/
#define _PAD		0x10	/* X/Open Pad.				*/
#define _SUBPAD		0x20	/* X/Open subpad.			*/


/*----------------------------------------------------------------------*/
/* Miscellaneous */

#define _INBUFSIZ	512	/* size of terminal input buffer	*/
#define _NO_CHANGE	-1	/* flags line edge unchanged		*/

#define _BCHAR		0x03	/* Break char	    (^C)		*/
#define _ECHAR		0x08	/* Erase char	    (^H)		*/
#define _DWCHAR		0x17	/* Delete Word char (^W)		*/
#define _DLCHAR		0x15	/* Delete Line char (^U)		*/
#define _GOCHAR		0x11	/* ^Q character				*/
#define _PRINTCHAR	0x10	/* ^P character				*/
#define _STOPCHAR	0x13	/* ^S character				*/
#define  NUNGETCH	256	/* max # chars to ungetch()		*/


/* Setmode stuff */
struct cttyset
{
	bool	been_set;
	SCREEN	saved;
};

extern struct cttyset c_sh_tty;		/* tty modes for shell_mode	*/
extern struct cttyset c_pr_tty;		/* tty modes for prog_mode	*/
extern struct cttyset c_save_tty;
extern struct cttyset c_save_trm;

/* tracing flag */
extern bool trace_on;


/* Monitor (terminal) type information */
#define _NONE           0x00
#define _MDA            0x01
#define _CGA            0x02
#define _EGACOLOR       0x04
#define _EGAMONO        0x05
#define _VGACOLOR       0x07
#define _VGAMONO        0x08
#define _MCGACOLOR      0x0a
#define _MCGAMONO       0x0b
#define _MDS_GENIUS     0x30
#define _UNIX_COLOR     0x40
#define _UNIX_MONO      0x41

/* Text-mode font size information */
#define _FONT8  8
#define _FONT14 14
#define _FONT15 15              /* GENIUS */
#define _FONT16 16


/*----------------------------------------------------------------------
 *	ANSI C prototypes.
 */

void	PDC_beep(void);
bool	PDC_breakout(void);
int	PDC_chadd(WINDOW *, chtype, bool, bool);
bool	PDC_check_bios_key(void);
int	PDC_chg_attrs(WINDOW *, chtype, int, int, int, int);
int	PDC_chins(WINDOW *, chtype, bool);
int	PDC_copy_win(const WINDOW *, WINDOW *, int, int, int,
		     int, int, int, bool);
int	PDC_cursor_off(void);
int	PDC_cursor_on(void);
int	PDC_curs_set(int);
int	PDC_fix_cursor(int);
int	PDC_get_bios_key(void);
int	PDC_get_columns(void);
bool	PDC_get_ctrl_break(void);
int	PDC_get_cur_col(void);
int	PDC_get_cur_row(void);
int	PDC_get_cursor_pos(int *, int *);
int	PDC_get_cursor_mode(void);
int	PDC_get_font(void);
int	PDC_get_rows(void);
int	PDC_get_buffer_rows(void);
int	PDC_gotoxy(int, int);
void	PDC_init_atrtab(void);
WINDOW *PDC_makenew(int, int, int, int);
int	PDC_mouse_in_slk(int, int);
int	PDC_newline(WINDOW *, int);
int	PDC_putc(chtype, chtype);
int	PDC_putctty(chtype, chtype);
int	PDC_rawgetch(void);
int	PDC_reset_prog_mode(void);
int	PDC_reset_shell_mode(void);
int	PDC_resize_screen(int, int);
int	PDC_sanity_check(int);
int	PDC_scr_close(void);
int	PDC_scr_open(SCREEN *, bool);
int	PDC_scroll(int, int, int, int, int, chtype);
int	PDC_set_80x25(void);
int	PDC_set_ctrl_break(bool);
int	PDC_set_cursor_mode(int, int);
int	PDC_set_font(int);
int	PDC_set_rows(int);
void	PDC_slk_calc(void);
void	PDC_sync(WINDOW *);
int	PDC_sysgetch(void);
void	PDC_transform_line(int);
int	PDC_validchar(int);

#if defined(OS2) && !defined(EMXVIDEO)
int	PDC_set_scrn_mode(VIOMODEINFO);
bool	PDC_scrn_modes_equal(VIOMODEINFO, VIOMODEINFO);
int	PDC_get_scrn_mode(VIOMODEINFO *);
int	PDC_query_adapter_type(VIOCONFIGINFO *);
int	PDC_get_keyboard_info(KBDINFO *);
int	PDC_set_keyboard_binary(void);
int	PDC_set_keyboard_default(void);
int	PDC_reset_shell_mode(void);
int	PDC_reset_prog_mode(void);
#else
int	PDC_set_scrn_mode(int);
int	PDC_get_scrn_mode(void);
int	PDC_query_adapter_type(void);
#endif

#ifdef XCURSES
int	XCurses_display_cursor(int, int, int, int, int);
int	XCurses_rawgetch(int);
bool	XCurses_kbhit(void);
int	XCurses_get_input_fd(void);
int	XCursesInstruct(int);
int	XCursesInstructAndWait(int);
int	XCurses_transform_line(const chtype *, int, int, int);
int	XCursesInitscr(const char *, int, char **);
int	XCursesEndwin(void);
void	XCursesCleanupCursesProcess(int);
int	XCursesResizeScreen(int, int);
int	XCurses_get_cols(void);
int	XCurses_get_rows(void);
int	XCurses_refresh_scrollbar(void);
void	XCurses_set_title(const char *);
int	XCurses_getclipboard(char **, long *);
int	XCurses_setclipboard(const char *, long);
int	XCurses_clearclipboard(void);
unsigned long XCurses_get_key_modifiers(void);
#endif

#ifdef __PACIFIC__
void	movedata(unsigned, unsigned, unsigned, unsigned, unsigned);
#endif

#ifdef PDCDEBUG
void PDC_debug(const char *, ...);
# define PDC_LOG(x) if (trace_on) PDC_debug x
# define RCSID(x) static const char *rcsid = x
#else
# define PDC_LOG(x)
# define RCSID(x)
#endif

#define PDC_COLORS		8

/* Internal macros for attributes */
#ifdef CHTYPE_LONG
# define PDC_COLOR_PAIRS	64
# define PDC_OFFSET		32
# define chtype_attr(ch)	(atrtab[((ch >> PDC_ATTR_SHIFT) & 0xFFFF)] << 8)
#else
# define PDC_COLOR_PAIRS	32
# define PDC_OFFSET		8
# define chtype_attr(ch)	(atrtab[((ch >> PDC_ATTR_SHIFT) & 0xFF)] << 8)
#endif

#define MAX_ATRTAB		(PDC_COLOR_PAIRS * PDC_OFFSET)

/* Internal mouse handling macros */
#define TRAPPED_MOUSE_X_POS	  (Trapped_Mouse_status.x)
#define TRAPPED_MOUSE_Y_POS	  (Trapped_Mouse_status.y)
#define TRAPPED_A_BUTTON_CHANGED  (Trapped_Mouse_status.changes & 7)
#define TRAPPED_MOUSE_MOVED	  (Trapped_Mouse_status.changes & 8)
#define TRAPPED_MOUSE_POS_REPORT  (Trapped_Mouse_status.changes & 16)
#define TRAPPED_BUTTON_CHANGED(x) (Trapped_Mouse_status.changes & (1<<((x)-1)))
#define TRAPPED_BUTTON_STATUS(x)  (Trapped_Mouse_status.button[(x) - 1])

#define ACTUAL_MOUSE_X_POS	  (Actual_Mouse_status.x)
#define ACTUAL_MOUSE_Y_POS	  (Actual_Mouse_status.y)
#define ACTUAL_A_BUTTON_CHANGED   (Actual_Mouse_status.changes & 7)
#define ACTUAL_MOUSE_MOVED	  (Actual_Mouse_status.changes & 8)
#define ACTUAL_MOUSE_POS_REPORT   (Actual_Mouse_status.changes & 16)
#define ACTUAL_BUTTON_CHANGED(x)  (Actual_Mouse_status.changes & (1<<((x)-1)))
#define ACTUAL_BUTTON_STATUS(x)   (Actual_Mouse_status.button[(x) - 1])

#define TEMP_MOUSE_X_POS	  (Temp_Mouse_status.x)
#define TEMP_MOUSE_Y_POS	  (Temp_Mouse_status.y)
#define TEMP_A_BUTTON_CHANGED	  (Temp_Mouse_status.changes & 7)
#define TEMP_MOUSE_MOVED	  (Temp_Mouse_status.changes & 8)
#define TEMP_MOUSE_POS_REPORT	  (Temp_Mouse_status.changes & 16)
#define TEMP_BUTTON_CHANGED(x)	  (Temp_Mouse_status.changes & (1<<((x)-1)))
#define TEMP_BUTTON_STATUS(x)	  (Temp_Mouse_status.button[(x) - 1])

#ifdef XCURSES
#define CURSES_EXIT			999999
#define CURSES_REFRESH			999998
#define CURSES_CHILD			999997
#define CURSES_CURSOR			999996
#define CURSES_CONTINUE			999995
#define CURSES_BELL			999994
#define CURSES_FLASH			999993
#define CURSES_CLEAR			999992
#define CURSES_RESIZE			999991
#define CURSES_REFRESH_SCROLLBAR	999990
#define CURSES_TITLE			999989
#define CURSES_GET_SELECTION		999988
#define CURSES_SET_SELECTION		999987
#define CURSES_DISPLAY_CURSOR		999986
#define CURSES_CLEAR_SELECTION		999985
#endif

#endif /* __CURSES_INTERNALS__*/
