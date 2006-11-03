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

/* $Id: curspriv.h,v 1.134 2006/11/03 13:41:14 wmcbrine Exp $ */

/*                         CURSPRIV.H

   Private definitions and declarations for use within PDCurses.
   These should generally not be referenced by applications. */

#ifndef __CURSES_INTERNALS__
#define __CURSES_INTERNALS__ 1

#define CURSES_LIBRARY
#include <curses.h>

#if defined(__TURBOC__) || defined(__EMX__) || defined(__DJGPP__) || \
    defined(__CYGWIN32__) || defined(__MINGW32__) || \
    defined(__WATCOMC__) || defined(__PACIFIC__)
#  ifndef HAVE_VSSCANF
#    define HAVE_VSSCANF	/* have vsscanf() */
#  endif
#endif

#if defined(__CYGWIN32__) || defined(__MINGW32__) || \
    defined(__LCC__) || defined(__WATCOMC__)
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF	/* have vsnprintf() */
#  endif
#endif

/*----------------------------------------------------------------------*/

/* Window properties */

#define _SUBWIN		0x01	/* window is a subwindow		*/
#define _PAD		0x10	/* X/Open Pad.				*/
#define _SUBPAD		0x20	/* X/Open subpad.			*/

/* Miscellaneous */

#define _NO_CHANGE	-1	/* flags line edge unchanged		*/

#define _ECHAR		0x08	/* Erase char	    (^H)		*/
#define _DWCHAR		0x17	/* Delete Word char (^W)		*/
#define _DLCHAR		0x15	/* Delete Line char (^U)		*/

extern bool pdc_trace_on;	/* tracing flag				*/
extern bool pdc_color_started;
extern unsigned long pdc_key_modifiers;
extern MOUSE_STATUS pdc_mouse_status;
extern unsigned char *pdc_atrtab;

/*----------------------------------------------------------------------*/

/* Platform implementation functions */

void	PDC_beep(void);
bool	PDC_can_change_color(void);
int	PDC_color_content(short, short *, short *, short *);
bool	PDC_check_bios_key(void);
int	PDC_curs_set(int);
void	PDC_flushinp(void);
int	PDC_get_bios_key(void);
int	PDC_get_columns(void);
bool	PDC_get_ctrl_break(void);
int	PDC_get_cursor_pos(int *, int *);
int	PDC_get_cursor_mode(void);
int	PDC_get_rows(void);
void	PDC_gotoyx(int, int);
void	PDC_init_atrtab(void);
int	PDC_init_color(short, short, short, short);
void	PDC_napms(int);
void	PDC_reset_prog_mode(void);
void	PDC_reset_shell_mode(void);
int	PDC_resize_screen(int, int);
void	PDC_restore_screen_mode(int);
void	PDC_save_screen_mode(int);
void	PDC_scr_close(void);
void	PDC_scr_exit(void);
int	PDC_scr_open(int, char **);
int	PDC_set_ctrl_break(bool);
void	PDC_set_keyboard_binary(bool);
void	PDC_transform_line(int, int, int, const chtype *);
const char *PDC_sysname(void);

/* Internal cross-module functions */

int	PDC_chadd(WINDOW *, chtype, bool, bool);
WINDOW *PDC_makenew(int, int, int, int);
int	PDC_mouse_in_slk(int, int);
void	PDC_slk_free(void);
void	PDC_slk_initialize(void);
void	PDC_sync(WINDOW *);

#ifdef PDCDEBUG
void PDC_debug(const char *, ...);
# define PDC_LOG(x) if (pdc_trace_on) PDC_debug x
# define RCSID(x) static const char *rcsid = x
#else
# define PDC_LOG(x)
# define RCSID(x)
#endif

/* Internal macros for attributes */

#ifdef CHTYPE_LONG
# define PDC_COLOR_PAIRS	256
# define PDC_OFFSET		32
#else
# define PDC_COLOR_PAIRS	32
# define PDC_OFFSET		8
#endif

#define chtype_attr(ch)		(pdc_atrtab[(chtype)(ch) >> PDC_ATTR_SHIFT])
#define MAX_ATRTAB		(PDC_COLOR_PAIRS * PDC_OFFSET)

#ifndef max
# define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
# define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define DIVROUND(num, divisor) ((num) + ((divisor) >> 1)) / (divisor)

#endif /* __CURSES_INTERNALS__*/
