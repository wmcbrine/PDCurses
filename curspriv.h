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

/* $Id: curspriv.h,v 1.114 2006/08/11 22:10:36 wmcbrine Exp $ */

/*                         CURSPRIV.H

   Private definitions and declarations for use within PDCurses.
   These should generally not be referenced by applications. */

#ifndef __CURSES_INTERNALS__
#define __CURSES_INTERNALS__ 1

/*----------------------------------------------------------------------*/
/* window properties */

#define _SUBWIN		0x01	/* window is a subwindow		*/
#define _PAD		0x10	/* X/Open Pad.				*/
#define _SUBPAD		0x20	/* X/Open subpad.			*/


/*----------------------------------------------------------------------*/
/* Miscellaneous */

#define _INBUFSIZ	512	/* size of terminal input buffer	*/
#define _NO_CHANGE	-1	/* flags line edge unchanged		*/

#define _ECHAR		0x08	/* Erase char	    (^H)		*/
#define _DWCHAR		0x17	/* Delete Word char (^W)		*/
#define _DLCHAR		0x15	/* Delete Line char (^U)		*/
#define  NUNGETCH	256	/* max # chars to ungetch()		*/


/* Setmode stuff */
struct cttyset
{
	bool	been_set;
	SCREEN	saved;
};

extern struct cttyset *ctty;

/* tracing flag */
extern bool trace_on;


/* Monitor (terminal) type information */
enum
{
	_NONE, _MDA, _CGA,
	_EGACOLOR = 0x04, _EGAMONO,
	_VGACOLOR = 0x07, _VGAMONO,
	_MCGACOLOR = 0x0a, _MCGAMONO,
	_MDS_GENIUS = 0x30,
	_UNIX_COLOR = 0x40, _UNIX_MONO
};

extern int c_pindex;            /* putter index */
extern int c_gindex;            /* getter index */
extern int c_ungind;            /* ungetch() push index */
extern int c_ungch[NUNGETCH];   /* array of ungotten chars */
extern WINDOW *_getch_win_;
extern unsigned long pdc_key_modifiers;

extern MOUSE_STATUS Trapped_Mouse_status;

extern unsigned char *atrtab;

/*----------------------------------------------------------------------
 *	ANSI C prototypes.
 */

void	PDC_beep(void);
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
WINDOW *PDC_makenew(int, int, int, int);
int	PDC_mouse_in_slk(int, int);
void	PDC_napms(int);
void	PDC_reset_prog_mode(void);
void	PDC_reset_shell_mode(void);
int	PDC_resize_screen(int, int);
void	PDC_restore_screen_mode(int);
void	PDC_scr_close(void);
void	PDC_scr_exit(void);
int	PDC_scr_open(int, char **);
int	PDC_set_ctrl_break(bool);
void	PDC_set_keyboard_binary(bool);
void	PDC_sync(WINDOW *);
void	PDC_transform_line(int, int, int, const chtype *);
int	PDC_validchar(int);

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
#else
# define PDC_COLOR_PAIRS	32
# define PDC_OFFSET		8
#endif

#define chtype_attr(ch)		(atrtab[(chtype)(ch) >> PDC_ATTR_SHIFT])

#define MAX_ATRTAB		(PDC_COLOR_PAIRS * PDC_OFFSET)

#ifdef DOS
# define PDC_SYSNAME "DOS"
#else
# ifdef OS2
#  define PDC_SYSNAME "OS/2"
# else
#  ifdef WIN32
#   define PDC_SYSNAME "Win32"
#  else
#   define PDC_SYSNAME "X11"
#  endif
# endif
#endif

#endif /* __CURSES_INTERNALS__*/
