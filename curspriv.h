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
$Id: curspriv.h,v 1.5 2002/11/27 11:14:27 mark Exp $
*/
/*
*
*                          CURSPRIV.H
*
* Header file for definitions and declarations for the
* PDCurses package. These definitions should not be generally
* accessible to programmers, but are provided if the applications
* programmer decides to make the decision in favor of speed on a
* PC over portability.
*
* Revision History:
* Frotz 1.5Beta 900714  Added many levels of compiler support.
*                       Added mixed prototypes for all "internal" routines.
*                       Removed all assembly language.  Added EGA/VGA
*                       support.  Converted all #ifdef to #if in all
*                       modules except CURSES.H and CURSPRIV.H.
*                       Always include ASSERT.H.  Added support for an
*                       external malloc(), calloc() and free().
*                       Added support for FAST_VIDEO (direct-memory writes).
*                       Added various memory model support (for FAST_VIDEO).
*                       Added much of the December 1988 X/Open Curses
*                       specification.
* bl    1.3     881005  All modules lint-checked with MSC '-W3' and turbo'C'
*                       '-w -w-pro' switches.
* bl    1.2     881002  Support (by #ifdef UCMASM) for uppercase-only
*                       assembly routine names. If UCMASM if defined,
*                       all assembler names are #defined as upper case.
*                       Not needed if you do "MASM /MX. Also missing
*                       declaration of cursesscroll(). Fixes thanks to
*                       N.D. Pentcheff
* bl    1.1     880306  Add _chadd() for raw output routines.
* bl    1.0     870515  Release.
*
*/

#ifndef __CURSES_INTERNALS__
#define __CURSES_INTERNALS__

/* Always include... */
#include <assert.h>

#if defined(HAVE_STDARG_H) && defined(HAVE_PROTO)
#  include <stdarg.h>
#  define HAVE_STDARG_H_HAVE_PROTO
#else
#  include <varargs.h>
#endif

/*----------------------------------------------------------------------
*       MEMORY MODEL SUPPORT:
*
*       MODELS
*               TINY            cs,ds,ss all in 1 segment (not enough memory!)
*               SMALL           cs:1 segment,           ds:1 segment
*               MEDIUM          cs:many segments        ds:1 segment
*               COMPACT         cs:1 segment,           ds:many segments
*               LARGE           cs:many segments        ds:many segments
*               HUGE            cs:many segments        ds:segments > 64K
*/
#ifdef  __TINY__
#  define SMALL 1
#endif
#ifdef  __SMALL__
#  define SMALL 1
#endif
#ifdef  __MEDIUM__
#  define MEDIUM 1
#endif
#ifdef  __COMPACT__
#  define COMPACT 1
#endif
#ifdef  __LARGE__
#  define LARGE 1
#endif
#ifdef  __HUGE__
#  define HUGE 1
#endif


/*----------------------------------------------------------------------
*       OPERATING SYSTEM SUPPORT:
*
*               DOS             The one we all know and love:-}
*               OS/2            The new kid on the block.
*               FLEXOS          A Real-time, protected-mode OS from
*                               Digital Research, Inc.
*				(AKA, the 4680 from IBM...)
*/

/*----------------------------------------*/
#ifdef  DOS
#  define FAST_VIDEO 1          /* We can write directly to the screen. */
#  ifdef NDP
     typedef union REGS16 Regs;
#  else
     typedef union REGS Regs;
#  endif
   extern Regs regs;
#  ifdef GO32                           /* Note: works only in plain DOS... */
#    if DJGPP == 2
#      define _FAR_POINTER(s,o)		((((int)(s))<<4) + ((int)(o)))
#    else
#      define _FAR_POINTER(s,o)		(0xe0000000 + (((int)(s))<<4) + ((int)(o)))
#    endif
#    define _FP_SEGMENT(p)		(unsigned short)((((long)fp) >> 4) & 0xffff)
#    define _FP_OFFSET(p)		((unsigned short)fp & 0x000f)
#  else
#    ifdef __TURBOC__
#      define _FAR_POINTER(s,o)	MK_FP(s,o)
#    else
#      if defined(NDP)
#        define _FAR_POINTER(s,o)	((((int)(s))<<4) + ((int)(o)))
#      else
#        if defined(WATCOMC) && defined(__FLAT__)
#          define _FAR_POINTER(s,o)	((((int)(s))<<4) + ((int)(o)))
#        else
#          define _FAR_POINTER(s,o)	(((long)s << 16) | (long)o)
#        endif
#      endif
#    endif
#    define _FP_SEGMENT(p)		(unsigned short)(((long)fp) >> 4)
#    define _FP_OFFSET(p)		((unsigned short)fp & 0x000f)
#  endif

#  ifdef GO32
     unsigned char getdosmembyte (int offs);    /* see: private\_dosmem.c */
     unsigned short getdosmemword (int offs);
     void setdosmembyte (int offs, unsigned char b);
     void setdosmemword (int offs, unsigned short w);
#  else
#    if SMALL || MEDIUM || MSC
#      define getdosmembyte(offs)    (*((unsigned char far *) _FAR_POINTER(0,offs)))
#      define getdosmemword(offs)    (*((unsigned short far *) _FAR_POINTER(0,offs)))
#      define setdosmembyte(offs,x)  (*((unsigned char far *) _FAR_POINTER(0,offs)) = (x))
#      define setdosmemword(offs,x)  (*((unsigned short far *) _FAR_POINTER(0,offs)) = (x))
#    else
#      define getdosmembyte(offs)    (*((unsigned char *) _FAR_POINTER(0,offs)))
#      define getdosmemword(offs)    (*((unsigned short *) _FAR_POINTER(0,offs)))
#      define setdosmembyte(offs,x)  (*((unsigned char *) _FAR_POINTER(0,offs)) = (x))
#      define setdosmemword(offs,x)  (*((unsigned short *) _FAR_POINTER(0,offs)) = (x))
#    endif
#  endif
#endif

/*----------------------------------------*/
#ifdef  FLEXOS
#  define FAST_VIDEO 1          /* We can use scopy()   */
#  define GMODE  0 /* KLUDGE ALERT!
                   * GMODE == 0 defines character mode structures in FLEXTAB.H.
                   * GMODE == 1 defines graphics  mode structures in FLEXTAB.H.
                   */
#include <flextab.h>
extern VIRCON vir;
#endif




/*----------------------------------------------------------------------
*       MALLOC DEBUGGING SUPPORT:
*
*       Set EMALLOC and EMALLOC_MAGIC in order to use your private
*       versions of malloc(), calloc(), and free().  This can help,
*       but not solve, your malloc problems when debugging...
*
*/
#if defined(INTERNAL) || defined(CURSES_LIBRARY)
#  define EMALLOC 0             /* Enable/Disable External Malloc       */
#  define EMALLOC_MAGIC  0x0C0C /* Our magic indicator that we should   */
                                /* use our external malloc rather than  */
                                /* the runtime's malloc.                */
#else
#  define EMALLOC 0             /* Disable External Malloc	        */
#endif


/*----------------------------------------------------------------------*/
/* window properties */
#define _SUBWIN         0x01    /* window is a subwindow            */
#define _ENDLINE        0x02    /* last winline is last screen line */
#define _FULLWIN        0x04    /* window fills screen              */
#define _SCROLLWIN      0x08    /* window lwr rgt is screen lwr rgt */
#define _PAD            0x10    /* X/Open Pad.                      */
#define _SUBPAD         0x20    /* X/Open subpad.                   */




/*----------------------------------------------------------------------*/
/* Miscellaneous */
#define _INBUFSIZ       512     /* size of terminal input buffer */
#define _NO_CHANGE      -1      /* flags line edge unchanged     */




/* @@@ THESE SHOULD BE INDIVIDUAL FUNCTIONS, NOT MACROS! */
#define _BCHAR          0x03    /* Break char	    (^C)         */
#define _ECHAR          0x08    /* Erase char	    (^H)         */
#define _DWCHAR         0x17    /* Delete Word char (^W)         */
#define _DLCHAR         0x15    /* Delete Line char (^U)         */
#define _GOCHAR         0x11    /* ^Q character                  */
#define _PRINTCHAR      0x10    /* ^P character                  */
#define _STOPCHAR       0x13    /* ^S character                  */
#define  NUNGETCH       20      /* max # chars to ungetch()      */




/* Setmode stuff */
struct cttyset
{
	bool	been_set;
	SCREEN	saved;
};

extern struct cttyset c_sh_tty;         /* tty modes for shell_mode */
extern struct cttyset c_pr_tty;         /* tty modes for prog_mode  */
extern struct cttyset c_save_tty;
extern struct cttyset c_save_trm;

/* Printscan stuff */
extern char c_printscanbuf[];           /* buffer used during I/O */

/* tracing flag */
extern bool trace_on;

/* Strget stuff */
extern char*    c_strbeg;

/* doupdate stuff */
extern WINDOW*  twin;                   /* used by many routines */


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
#define _FLEXOS         0x20            /* A Flexos console */
#define _MDS_GENIUS     0x30
#define _UNIX_COLOR     0x40
#define _UNIX_MONO      0x41

/* Text-mode font size information */
#define _FONT8  8
#define _FONT14 14
#define _FONT15 15              /* GENIUS */
#define _FONT16 16


/*----------------------------------------------------------------------
*       ANSI C prototypes.  Be sure that your compiler conditional
*       compilation definitions above define HAVE_PROTO if your compiler 
*       supports prototypes.
*/
#ifdef     HAVE_PROTO
void            PDC_beep( void );
int             PDC_backchar( WINDOW*, char*, int* );
bool            PDC_breakout( void );
int             PDC_chadd( WINDOW*, chtype, bool, bool );
bool            PDC_check_bios_key( void );
int             PDC_chg_attr( WINDOW*, chtype, int, int, int, int );
int             PDC_chins( WINDOW*, chtype, bool );
int             PDC_clr_scrn( WINDOW* );
int             PDC_clr_update( WINDOW* );
int             PDC_copy_win( WINDOW *,WINDOW *,int,int,int,int,int,int,int,int,bool );
int             PDC_cursor_off( void );
int             PDC_cursor_on( void );
int             PDC_curs_set( int );
int             PDC_fix_cursor( int );
int             PDC_gattr( void );
int             PDC_get_bios_key( void );
int             PDC_get_columns( void );
bool            PDC_get_ctrl_break( void );
int             PDC_get_cur_col( void );
int             PDC_get_cur_row( void );
int             PDC_get_cursor_pos( int*, int* );
int             PDC_get_cursor_mode( void );
int             PDC_get_font( void );
int             PDC_get_rows( void );
int             PDC_get_buffer_rows( void );
int             PDC_gotoxy( int, int );
int             PDC_init_atrtab(void);
WINDOW*         PDC_makenew( int, int, int, int );
int             PDC_mouse_in_slk( int, int );
int             PDC_newline( WINDOW*, int );
int             PDC_print( int, int, int );
int             PDC_putc( chtype, chtype );
int             PDC_putchar( chtype );
int             PDC_putctty( chtype, chtype );
int             PDC_rawgetch( void );
int             PDC_reset_prog_mode( void );
int             PDC_reset_shell_mode( void );
int             PDC_resize_screen( int, int );
int             PDC_sanity_check( int );
int             PDC_scr_close( void );
int             PDC_scr_open( SCREEN*, bool );
int             PDC_scroll( int, int, int, int, int, chtype );
int             PDC_set_80x25( void );
int             PDC_set_ctrl_break( bool );
int             PDC_set_cursor_mode( int, int );
int             PDC_set_font( int );
int             PDC_set_rows( int );
int             PDC_split_plane( WINDOW*, char*, char*, int, int, int, int );
void            PDC_sync( WINDOW * );
int             PDC_sysgetch( void );
bool            PDC_transform_line( int );
void            PDC_usleep( long );
int             PDC_validchar( int );
int             PDC_vsscanf( char *, const char *, va_list);

void            PDC_slk_calc( void );


# if defined( OS2 ) && !defined( EMXVIDEO )
int             PDC_set_scrn_mode( VIOMODEINFO);
bool            PDC_scrn_modes_equal (VIOMODEINFO, VIOMODEINFO);
int             PDC_get_scrn_mode( VIOMODEINFO *);
int             PDC_query_adapter_type( VIOCONFIGINFO * );
int             PDC_get_keyboard_info( KBDINFO * );
int             PDC_set_keyboard_binary( void );
int             PDC_set_keyboard_default( void );
int             PDC_reset_shell_mode( void );
int             PDC_reset_prog_mode( void );
# else
int             PDC_set_scrn_mode( int );
bool            PDC_scrn_modes_equal (int, int);
int             PDC_get_scrn_mode( void );
int             PDC_query_adapter_type( void );
# endif

# ifdef  FLEXOS
int             PDC_flexos_8bitmode( void );
int             PDC_flexos_16bitmode( void );
char*           PDC_flexos_gname( void );
# endif

# ifdef UNIX
int             PDC_kbhit(void);
int             PDC_setup_keys(void);
# endif

# ifdef WIN32
void            PDC_doupdate(void);
# endif

# if defined (XCURSES)
int             XCurses_redraw_curscr(void);
int             XCurses_display_cursor(int,int,int,int,int);
int             XCurses_rawgetch(int);
bool            XCurses_kbhit(void);
int             XCurses_get_input_fd(void);
int             XCursesInstruct(int);
int             XCursesInstructAndWait(int);
int             XCurses_transform_line(chtype*, int , int , int );
int             XCursesInitscr(char *,int, char **);
int             XCursesEndwin(void);
void            XCursesCleanupCursesProcess(int);
int             XCursesResizeScreen( int, int );
int             XCurses_get_cols(void);
int             XCurses_get_rows(void);
int             XCurses_refresh_scrollbar(void);
void            XCurses_set_title(char *);
int             XCurses_getclipboard( char **, long * );
int             XCurses_setclipboard( char *, long );
int             XCurses_clearclipboard( void );
unsigned long   XCurses_get_key_modifiers( void );
# endif

# ifdef PDCDEBUG
void            PDC_debug( char*,... );
# endif

# ifdef  REGISTERWINDOWS
bool            PDC_inswin( WINDOW*, WINDOW* );
int             PDC_addtail( WINDOW* );
int             PDC_addwin( WINDOW*, WINDOW* );
int             PDC_rmwin( WINDOW* );
WINDS*          PDC_findwin( WINDOW* );
# endif

#else

void            PDC_beep( /* void */ );
int             PDC_backchar( /* WINDOW*, char*, int* */ );
bool            PDC_breakout( /* void */ );
int             PDC_chadd( /* WINDOW*, chtype, bool, bool */ );
bool            PDC_check_bios_key( /* void */ );
int             PDC_chg_attr( /* WINDOW*, chtype, int, int, int, int */ );
int             PDC_chins( /* WINDOW*, chtype, bool */ );
int             PDC_clr_scrn( /* WINDOW* */ );
int             PDC_clr_update( /* WINDOW* */ );
int             PDC_copy_win( /* WINDOW *,WINDOW *,int,int,int,int,int,int,int,int,bool */ );
int             PDC_cursor_off( /* void */ );
int             PDC_cursor_on( /* void */ );
int             PDC_curs_set( /* int */ );
int             PDC_fix_cursor( /* int */ );
int             PDC_gattr( /* void */ );
int             PDC_get_bios_key( /* void */ );
int             PDC_get_columns( /* void */ );
bool            PDC_get_ctrl_break( /* void */ );
int             PDC_get_cur_col( /* void */ );
int             PDC_get_cur_row( /* void */ );
int             PDC_get_cursor_pos( /* int*, int* */ );
int             PDC_get_cursor_mode( /* void */ );
int             PDC_get_font( /* void */ );
int             PDC_get_rows( /* void */ );
int             PDC_get_buffer_rows( /* void */ );
int             PDC_gotoxy( /* int, int */ );
int             PDC_init_atrtab( /*void*/ );
WINDOW*         PDC_makenew( /* int, int, int, int */ );
int             PDC_mouse_in_slk(/* int, int */ );
int             PDC_newline( /* WINDOW*, int */ );
int             PDC_print( /* int, int, int */ );
int             PDC_putc( /* chtype, chtype */ );
int             PDC_putchar( /* chtype */ );
int             PDC_putctty( /* chtype, chtype */ );
int             PDC_rawgetch( /* void */ );
int             PDC_reset_prog_mode( /* void */ );
int             PDC_reset_shell_mode( /* void */ );
int             PDC_resize_screen( /* int, int */ );
int             PDC_sanity_check( /* int */ );
int             PDC_scr_close( /* void */ );
int             PDC_scr_open( /* SCREEN*, bool */ );
int             PDC_scroll( /* int, int, int, int, int, chtype */ );
int             PDC_set_80x25( /* void */ );
int             PDC_set_ctrl_break( /* bool */ );
int             PDC_set_cursor_mode( /* int, int */ );
int             PDC_set_font( /* int */ );
int             PDC_set_rows( /* int */ );
int             PDC_split_plane( /* WINDOW*, char*, char*, int, int, int, int */ );
void            PDC_sync( /* WINDOW * */ );
int             PDC_sysgetch( /* void */ );
bool            PDC_transform_line( /* int */ );
void            PDC_usleep( /* long */ );
int             PDC_validchar( /* int */ );
int             PDC_vsscanf( /* char *, const char *, ... */);

void            PDC_slk_calc( /* void */ );


# if defined( OS2 ) && !defined( EMXVIDEO )
int             PDC_set_scrn_mode( /* VIOMODEINFO*/ );
bool            PDC_scrn_modes_equal ( /*VIOMODEINFO, VIOMODEINFO*/ );
int             PDC_get_scrn_mode( /* VIOMODEINFO * */ );
int             PDC_query_adapter_type( /* VIOCONFIGINFO * */ );
# else
int             PDC_set_scrn_mode( /* int */ );
bool            PDC_scrn_modes_equal ( /*int, int*/ );
int             PDC_get_scrn_mode( /* void */ );
int             PDC_query_adapter_type( /* void */ );
# endif

# ifdef  FLEXOS
int             PDC_flexos_8bitmode( /* void */ );
int             PDC_flexos_16bitmode( /* void */ );
char*           PDC_flexos_gname( /* void */ );
# endif

# ifdef UNIX
int             PDC_kbhit( /*void*/ );
int             PDC_setup_keys( /*void*/ );
# endif

# ifdef WIN32
void            PDC_doupdate( /*void*/ );
# endif

# if defined ( XCURSES )
int             XCurses_redraw_curscr( /*void*/ );
int             XCurses_display_cursor( /*int,int,int,int,int*/ );
int             XCurses_rawgetch( /*int*/ );
bool            XCurses_kbhit( /*void*/ );
int             XCurses_get_input_fd( /*void*/ );
int             XCursesInstruct( /*int*/ );
int             XCursesInstructAndWait( /*void*/ );
int             XCurses_transform_line( /*chtype*, int , int , int */ );
int             XCursesInitscr( /* char *, int, char ** */ );
int             XCursesEndwin( /*void*/ );
void            XCursesCleanupCursesProcess(/* int */);
int             XCursesResizeScreen( /* int, int */ );
int             XCurses_get_cols(/* void */);
int             XCurses_get_rows(/* void */);
int             XCurses_refresh_scrollbar( /* void */ );
void            XCurses_set_title(/* char * */ );
int             XCurses_getclipboard( /* char **, long * */ );
int             XCurses_setclipboard( /* char *, long */ );
int             XCurses_clearclipboard( /* void */ );
unsigned long   XCurses_get_key_modifiers( /* void */ );
# endif

# ifdef PDCDEBUG
void            PDC_debug( /* char*,... */ );
# endif

# ifdef  REGISTERWINDOWS
bool            PDC_inswin( /* WINDOW*, WINDOW* */ );
int             PDC_addtail( /* WINDOW* */ );
int             PDC_addwin( /* WINDOW*, WINDOW* */ );
int             PDC_rmwin( /* WINDOW* */ );
WINDS*          PDC_findwin( /* WINDOW* */ );
# endif
#endif

#define PDC_COLORS           8
#define PDC_COLOR_PAIRS     64

#if defined(CHTYPE_LONG)
#  define PDC_OFFSET       32
#  define MAX_ATRTAB      ((PDC_COLOR_PAIRS+1)*PDC_OFFSET)
/* internal macros for attributes */
#  define chtype_attr(ch)  (atrtab[((ch >> 19) & 0xFFFF)] << 8)
#else
#  define PDC_OFFSET        8
#  define MAX_ATRTAB      272
/* internal macros for attributes */
#  define chtype_attr(ch)  ((atrtab[((ch >> 8) & 0xFF)] << 8) & A_ATTRIBUTES)
#endif
/*
 * Internal mouse handling macros
 */
#define TRAPPED_MOUSE_X_POS            (Trapped_Mouse_status.x)
#define TRAPPED_MOUSE_Y_POS            (Trapped_Mouse_status.y)
#define TRAPPED_A_BUTTON_CHANGED       (Trapped_Mouse_status.changes & 7)
#define TRAPPED_MOUSE_MOVED            (Trapped_Mouse_status.changes & 8)
#define TRAPPED_MOUSE_POS_REPORT       (Trapped_Mouse_status.changes & 16)
#define TRAPPED_BUTTON_CHANGED(x)      (Trapped_Mouse_status.changes & (1 << ((x) - 1)))
#define TRAPPED_BUTTON_STATUS(x)       (Trapped_Mouse_status.button[(x)-1])

#define ACTUAL_MOUSE_X_POS            (Actual_Mouse_status.x)
#define ACTUAL_MOUSE_Y_POS            (Actual_Mouse_status.y)
#define ACTUAL_A_BUTTON_CHANGED       (Actual_Mouse_status.changes & 7)
#define ACTUAL_MOUSE_MOVED            (Actual_Mouse_status.changes & 8)
#define ACTUAL_MOUSE_POS_REPORT       (Actual_Mouse_status.changes & 16)
#define ACTUAL_BUTTON_CHANGED(x)      (Actual_Mouse_status.changes & (1 << ((x) - 1)))
#define ACTUAL_BUTTON_STATUS(x)       (Actual_Mouse_status.button[(x)-1])

#define TEMP_MOUSE_X_POS            (Temp_Mouse_status.x)
#define TEMP_MOUSE_Y_POS            (Temp_Mouse_status.y)
#define TEMP_A_BUTTON_CHANGED       (Temp_Mouse_status.changes & 7)
#define TEMP_MOUSE_MOVED            (Temp_Mouse_status.changes & 8)
#define TEMP_MOUSE_POS_REPORT       (Temp_Mouse_status.changes & 16)
#define TEMP_BUTTON_CHANGED(x)      (Temp_Mouse_status.changes & (1 << ((x) - 1)))
#define TEMP_BUTTON_STATUS(x)       (Temp_Mouse_status.button[(x)-1])

#if defined(XCURSES)
#define CURSES_EXIT                999999
#define CURSES_REFRESH             999998
#define CURSES_CHILD               999997
#define CURSES_CURSOR              999996
#define CURSES_CONTINUE            999995
#define CURSES_BELL                999994
#define CURSES_FLASH               999993
#define CURSES_CLEAR               999992
#define CURSES_RESIZE              999991
#define CURSES_REFRESH_SCROLLBAR   999990
#define CURSES_TITLE               999989
#define CURSES_GET_SELECTION       999988
#define CURSES_SET_SELECTION       999987
#define CURSES_DISPLAY_CURSOR      999986
#define CURSES_CLEAR_SELECTION     999985

#define XCURSCR_Y_SIZE      (XCursesLINES*XCursesCOLS*sizeof(chtype))
#define XCURSCR_FLAG_SIZE   (XCursesLINES*sizeof(int))
#define XCURSCR_START_SIZE  (XCursesLINES*sizeof(int))
#define XCURSCR_LENGTH_SIZE (XCursesLINES*sizeof(int))
#define XCURSCR_ATRTAB_SIZE (MAX_ATRTAB*sizeof(unsigned char))
#define XCURSCR_SIZE        (XCURSCR_FLAG_SIZE+XCURSCR_START_SIZE+XCURSCR_LENGTH_SIZE+XCURSCR_Y_SIZE+XCURSCR_ATRTAB_SIZE)

#define XCURSCR_Y_OFF(y)    ((y)*XCursesCOLS*(sizeof(chtype)))
#define XCURSCR_FLAG_OFF    (XCURSCR_Y_OFF(0)+XCURSCR_Y_SIZE)
#define XCURSCR_START_OFF   (XCURSCR_FLAG_OFF+XCURSCR_FLAG_SIZE)
#define XCURSCR_LENGTH_OFF  (XCURSCR_START_OFF+XCURSCR_START_SIZE)
#define XCURSCR_ATRTAB_OFF  (XCURSCR_LENGTH_OFF+XCURSCR_LENGTH_SIZE)
#endif

#endif /* __CURSES_INTERNALS__*/
