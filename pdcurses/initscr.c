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
#define  CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#ifdef UNIX
#define NOTLIB
#include <defs.h>
#include <term.h>
#endif

/* undefine any macros for functions defined in this module */
#undef   initscr
#undef   endwin
#undef   isendwin
#undef   newterm
#undef   set_term
#undef   delscreen
#undef   resize_term
#undef   is_termresized

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef move
#  undef wmove
#  undef resize_term
#  undef touchwin
#  undef wnoutrefresh
#endif

#ifdef UNIX
#define NOTLIB
#include <defs.h>
#include <term.h>
/* following is to stop compilation problems with #define of lines */
#undef lines
#endif

#ifdef PDCDEBUG
char *rcsid_initscr  = "$Id: initscr.c,v 1.4 2002/12/16 06:58:09 mark Exp $";
#else
char* _curses_notice = "PDCurses 2.2 - Public Domain 1994";
#endif

int   _default_lines = 25; /* default terminal height */

SCREEN *SP = (SCREEN*)NULL;      /* curses variables     */
WINDOW*  curscr=NULL;         /* the current screen image   */
WINDOW*  stdscr=NULL;         /* the default screen window  */
int   LINES=0;       /* current terminal height */
int   COLS=0;        /* current terminal width  */
MOUSE_STATUS Mouse_status;
int    use_emalloc = FALSE;

#if defined DOS
Regs regs;
#endif

/*
 * Global definitions for charget routines
 */
int   c_pindex = 0;     /* putter index */
int   c_gindex = 1;     /* getter index */
int   c_ungind = 0;     /* wungetch() push index */
int   c_ungch[NUNGETCH];   /* array of ungotten chars */
WINDOW*  _getch_win_=NULL;

/*
 * Global definitions for setmode routines
 */
struct cttyset c_sh_tty = {0};   /* tty modes for def_shell_mode */
struct cttyset c_pr_tty = {0};   /* tty modes for def_prog_mode  */
struct cttyset c_save_tty = {0};
struct cttyset c_save_trm = {0};

/*
 * Global definitions for printscan routines
 */
char c_printscanbuf[513];  /* buffer used during I/O */

/*
 * Global definitions for strget routines
 */
char *c_strbeg;

#if   EMALLOC
void* emalloc( size_t );
void* ecalloc( size_t, size_t );
void  efree( void* );
   
extern   void* emalloc();  /* user's emalloc(size)    */
extern   void* ecalloc();  /* user's ecalloc(num,size)   */
extern   void  efree(); /* user's efree(ptr)    */
#endif

#if !defined(UNIX) && !defined(XCURSES)
extern   void* malloc();   /* runtime's malloc(size)  */
extern   void* calloc();   /* runtime's calloc(num,size) */
extern   void  free();     /* runtime's free(ptr)     */
#endif

void* (*mallc)();    /* ptr to some malloc(size)   */
void* (*callc)();    /* ptr to some ecalloc(num,size)*/
void  (*fre)();      /* ptr to some free(ptr)   */
void* (*reallc)();      /* ptr to some realloc(ptr,size) */

#ifdef XCURSES
chtype *acs_map;
#endif

extern RIPPEDOFFLINE linesripped[5];
extern char linesrippedoff;

#ifdef HAVE_PROTO
extern void (*PDC_initial_slk)(void);
#else
extern void (*PDC_initial_slk)();
#endif

/*man-start*********************************************************************

  Name:                                                       initscr

  Synopsis:
   WINDOW *initscr(void);
   WINDOW *Xinitscr(int argc, char *argv[]);
   int endwin(void);
   int isendwin(void);
  *** SCREEN *newterm(char *type, FILE *outfd, FILE *infd);
   SCREEN *set_term(SCREEN *new);
  *** void delscreen(SCREEN *sp);

   int resize_term(int nlines, int ncols);
   bool is_termresized(void);

  X/Open Description:
   The first curses routine called should be initscr().  This will
   determine the terminal type and initialize all curses data
   structures.  The initscr() function also arranges that the
   first call to refresh() will clear the screen.  If errors
   occur, initscr() will write an appropriate error message to
   standard error and exit.  If the program wants an indication
   of error conditions, newterm() should be used instead of
   initscr().

   A program should always call endwin() before exiting or
   escaping from curses mode temporarily.  This routine will
   restore tty modes, move the cursor to the lower left corner
   of the screen and reset the terminal into the proper non-visual
   mode.  To resume curses after a temporary escape, refresh() or
   doupdate() should be called.

   The isendwin() function returns TRUE if endwin() has been called
   without any subsequent calls to wrefresh(), and FALSE otherwise.

   A program which outputs to more than one terminal should use
   newterm() for each terminal instead of initscr().  The newterm()
   function should be called once for each terminal.  It returns a 
   value of type SCREEN* which should be saved as a reference to that
   terminal. The arguments are the type of of terminal to be used
   in place of TERM (environment variable), a file pointer for output
   to the terminal and another file pointer for input from the terminal.
   The program must also call endwin() for each terminal no longer being
   used.

   The set_term() function is used to switch between different terminals.
   The screen reference 'new' becomes the new current terminal.
   The previous terminal is returned by the routine.  This is the
   only routine which manipulates SCREEN pointers; all other routines
   affect only the current terminal.

  PDCurses Description:
   The resize_term() function is used to have PDCurses change its
   internal structures to the new, specified size.

   The is_termresized() function returns TRUE if the Curses screen has
   been resized by external means, and requires a call to resize_term().

   Due to the fact that newterm() does not yet exist in PDCurses,
   there is no way to recover from an error in initscr().

  X/Open Return Value:
   All functions return NULL on error, except endwin(), which
   returns ERR on error.

  X/Open Errors:
   No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      initscr                               Y        Y       Y
      endwin                                Y        Y       Y
      isendwin                              -        -      3.0
      newterm                               -        -       Y
      set_term                              -        -       Y
      delscreen                             -        -      4.0
      resize_term                           -        -       -
      is_termresized                        -        -       -

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
WINDOW*  PDC_CDECL   Xinitscr(int argc, char *argv[])
#else
WINDOW*  PDC_CDECL   Xinitscr(argc,argv)
int argc;
char *argv[];
#endif
/***********************************************************************/
{
register int i=0;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("Xinitscr() - called\n");
#endif

   if (SP != (SCREEN *)NULL
   &&  SP->alive)
      return( NULL);

#ifdef EMXVIDEO
   v_init();
#endif

   if  (use_emalloc == EMALLOC_MAGIC)
   {
#if   EMALLOC
      use_emalloc = TRUE;
      mallc = emalloc;
      callc = ecalloc;
      fre   = efree;
      reallc = erealloc;
#endif
   }
   else
   {
      mallc = malloc;
      callc = calloc;
      fre   = free;
      reallc = realloc;
   }

#ifdef UNIX
   setupterm((char *)0,1,(int *)0);
   if (enter_ca_mode != NULL)
      putp(enter_ca_mode);
#endif

#if defined (XCURSES)
   if (XCursesInitscr(NULL,argc,argv) == ERR)
      exit(7);
#endif

#if defined (XCURSES)
   if (SP == (SCREEN*)NULL) /* SP already attached in XCursesInitscr() */
#else
   if ((SP = (SCREEN*)callc(1,sizeof(SCREEN))) == (SCREEN*)NULL)
#endif
   {
      fprintf( stderr, "initscr(): Unable to create SP\n" );
      exit(8);
   }

   PDC_scr_open(SP, 0);

#ifdef   FLEXOS
   _flexos_16bitmode();
#endif

   LINES = SP->lines;
   COLS = SP->cols;

   if (LINES < 2 || COLS < 2)
   {
      fprintf( stderr, "initscr(): LINES=%d COLS=%d: too small.\n",LINES,COLS );
      exit( 4 );
   }

   if ((curscr = newwin(LINES, COLS, 0, 0)) == (WINDOW *) NULL)
   {
      fprintf( stderr, "initscr(): Unable to create curscr.\n" );
      exit( 2 );
   }

#ifdef XCURSES
   if ((acs_map = (chtype *)(*mallc)(128*sizeof(chtype))) == (chtype *)NULL)
   {
      fprintf( stderr, "initscr(): Unable to create acs_map.\n" );
      exit( 5 );
   }
   for (i=0;i<128;i++)
   {
      acs_map[i] = '#' | A_ALTCHARSET;
   }
   ACS_BSSB = (chtype)13|A_ALTCHARSET;
   ACS_SSBB = (chtype)14|A_ALTCHARSET;
   ACS_BBSS = (chtype)12|A_ALTCHARSET;
   ACS_SBBS = (chtype)11|A_ALTCHARSET;
   ACS_SBSS = (chtype)22|A_ALTCHARSET;
   ACS_SSSB = (chtype)21|A_ALTCHARSET;
   ACS_SSBS = (chtype)23|A_ALTCHARSET;
   ACS_BSSS = (chtype)24|A_ALTCHARSET;
   ACS_BSBS = (chtype)18|A_ALTCHARSET;
   ACS_SBSB = (chtype)25|A_ALTCHARSET;
   ACS_SSSS = (chtype)15|A_ALTCHARSET;
   ACS_S1       = (chtype)16|A_ALTCHARSET;
   ACS_S9       = (chtype)20|A_ALTCHARSET;
   ACS_DIAMOND  = (chtype)1|A_ALTCHARSET;
   ACS_CKBOARD  = (chtype)2|A_ALTCHARSET;
   ACS_DEGREE   = (chtype)7|A_ALTCHARSET;
   ACS_PLMINUS  = (chtype)8|A_ALTCHARSET;
   ACS_BULLET   = (chtype)'*'|A_ALTCHARSET;
   ACS_LARROW   = (chtype)'<'|A_ALTCHARSET;
   ACS_RARROW   = (chtype)'>'|A_ALTCHARSET;
   ACS_DARROW   = (chtype)'v'|A_ALTCHARSET;
   ACS_UARROW   = (chtype)'^'|A_ALTCHARSET;
   ACS_BOARD    = (chtype)'#'|A_ALTCHARSET;
   ACS_LANTERN  = (chtype)'#'|A_ALTCHARSET;
   ACS_BLOCK    = (chtype)0|A_ALTCHARSET;
   /* extra, non-standard symbols */
   ACS_S3      = (chtype)17|A_ALTCHARSET;
   ACS_S7      = (chtype)19|A_ALTCHARSET;
   ACS_LEQUAL  = (chtype)26|A_ALTCHARSET;
   ACS_GEQUAL  = (chtype)27|A_ALTCHARSET;
   ACS_PI      = (chtype)28|A_ALTCHARSET;
   ACS_NEQUAL  = (chtype)29|A_ALTCHARSET;
   ACS_STERLING= (chtype)30|A_ALTCHARSET;
#endif

   if (PDC_initial_slk)
   {
      (*PDC_initial_slk)();
      LINES -= SP->slklines;
   }

/*
 * We have to sort out ripped off lines here and reduce the height
 * of stdscr by the number of lines ripped off
 */
   for (i=0;i<linesrippedoff;i++)
   {
      if (linesripped[i].line < 0)
         (*linesripped[i].init)(newwin(1,COLS,LINES-1,0),COLS);
      else
         (*linesripped[i].init)(newwin(1,COLS,SP->linesrippedoffontop++,0),COLS);
      SP->linesrippedoff++;
      LINES--;
   }
   linesrippedoff = 0;

   if ((stdscr = newwin(LINES, COLS, SP->linesrippedoffontop, 0)) == (WINDOW *) NULL)
   {
      fprintf( stderr, "initscr(): Unable to create stdscr.\n" );
      exit( 1 );
   }

   wclrtobot(stdscr);

/*
 * If preserving the existing screen don't allow a screen clear...
 */
if (SP->_preserve)
   {
   untouchwin(curscr);
   untouchwin(stdscr);
   stdscr->_clear = FALSE;
   }

   curscr->_clear = FALSE;

#ifdef   REGISTERWINDOWS
   SP->refreshall = FALSE;
   _inswin(stdscr, (WINDOW *)NULL);
#endif

#if defined(CHTYPE_LONG)
   PDC_init_atrtab(); /* set up default (BLACK on WHITE colours */
#endif

#ifdef EMXVIDEO
   SP->tahead = -1;
#endif

   MOUSE_X_POS = MOUSE_Y_POS = (-1);
   BUTTON_STATUS(1) = BUTTON_RELEASED;
   BUTTON_STATUS(2) = BUTTON_RELEASED;
   BUTTON_STATUS(3) = BUTTON_RELEASED;
   Mouse_status.changes = 0;

   SP->alive = TRUE;

#ifdef UNIX
   PDC_setup_keys();
#else
   def_shell_mode(); /* don't do this for UNIX as scropen has already changed things */
#endif

   return( stdscr );
}
/***********************************************************************/
#ifdef HAVE_PROTO
WINDOW*  PDC_CDECL   initscr(void)
#else
WINDOW*  PDC_CDECL   initscr()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("initscr() - called\n");
#endif
   return (Xinitscr( 0, NULL ) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL endwin(void)
#else
int PDC_CDECL endwin()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("endwin() - called\n");
#endif

#ifdef ORIGINAL_PDCURSES_BEHAVIOUR
   PDC_scr_close();
/* resetty();*/
   if (SP->orig_font != SP->font)  /* screen has not been resized */
   {
      PDC_set_font(SP->orig_font);
      resize_term(PDC_get_rows(),PDC_get_columns());
   }

   SP->visible_cursor = FALSE;   /* Force the visible cursor */
   SP->cursor = SP->orig_cursor;
   PDC_cursor_on();
   /*
    * Position cursor so that the screen will not scroll until they hit
    * a carriage return. Do this BEFORE delwin(curscr) as PDC_gotoxy() uses
    * curscr.
    */
   PDC_gotoxy(PDC_get_rows() - 2, 0);
   delwin(stdscr);
   delwin(curscr);
   stdscr = (WINDOW *)NULL;
   curscr = (WINDOW *)NULL;
   SP->alive = FALSE;

# if !defined (XCURSES)
   if (SP)
   {
      fre(SP);
      SP = (SCREEN *)NULL;
   }
# endif


# ifdef  FLEXOS
   _flexos_8bitmode();
# endif
/* PDC_fix_cursor(SP->orig_emulation);*/

# ifdef UNIX
   if (exit_ca_mode != NULL)
      putp(exit_ca_mode);
# endif

# if defined(DOS) || defined(OS2)
   reset_shell_mode();
# endif

# if DONOTKILLXFROMHERE
#  if defined (XCURSES)
   XCursesInstruct(CURSES_EXIT);
   XCursesCleanupCursesProcess(0);
#  endif
# endif

#else

/*
 * New endwin() behaviour; to allow temporary exit from curses
 * using endwin().
 */

   PDC_scr_close();

# ifdef  FLEXOS
   _flexos_8bitmode();
# endif

# if defined(DOS) || defined(OS2)
   reset_shell_mode();
# endif

   curs_set(1);
   /*
    * Position cursor to the bottom left of the screen.
    */
#if defined(WIN32)
   PDC_gotoxy(PDC_get_buffer_rows() - 2, 0);
#else
   PDC_gotoxy(PDC_get_rows() - 2, 0);
#endif

   SP->alive = FALSE;

#endif

   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   isendwin(void)
#else
int   PDC_CDECL   isendwin()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("isendwin() - called\n");
#endif

   return((SP->alive) ? FALSE : TRUE);
}

/***********************************************************************/
#ifdef HAVE_PROTO
SCREEN*PDC_CDECL     newterm( char *type, FILE *outfd, FILE *infd )
#else
SCREEN*PDC_CDECL     newterm(type,outfd,infd)
char *type;
FILE *outfd;
FILE *infd;
#endif
/***********************************************************************/
{
#ifdef   TC
#  pragma argsused
#endif

#if 0
extern   void* mallc(); /* malloc(size)      */
extern   void* callc(); /* calloc(num,size)  */
extern   void  fre();      /* free(ptr)      */

extern   void* malloc();
extern   void* calloc();
extern   void  free();

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("newterm() - called\n");
#endif

   if  (SP->alive)
      return( ERR );

   if  (use_emalloc == EMALLOC_MAGIC)
   {
#if   EMALLOC
      memset(SP, 0, sizeof(SCREEN));
      use_emalloc = TRUE;
      mallc = emalloc;
      callc = ecalloc;
      fre   = efree;
      reallc = erealloc;
#endif
   }
   else
   {
      memset(SP, 0, sizeof(SCREEN));
      mallc = malloc;
      callc = calloc;
      fre   = free;
      reallc = realloc;
   }
   PDC_scr_open(SP, 0);
   SP->orig_cursor = SP->cursor;
   SP->orig_font = PDC_get_font();
   SP->orgcbr = PDC_get_ctrl_break();
   SP->blank = ' ';
#ifdef   FLEXOS
   _flexos_16bitmode();
#endif
   savetty();
   LINES = PDC_get_rows();
   COLS = PDC_get_columns();

   if ((curscr = newwin(LINES, COLS, 0, 0)) == (WINDOW *) ERR)
   {
      return( ERR );
   }
   if ((stdscr = newwin(LINES, COLS, 0, 0)) == (WINDOW *) ERR)
   {
      return( ERR );
   }
   curscr->_clear = FALSE;
#ifdef   REGISTERWINDOWS
   SP->refreshall = FALSE;
   _inswin(stdscr, (WINDOW *)NULL);
#endif
   SP->alive = TRUE;
   return( SP );
#endif
   return(NULL);
}
/***********************************************************************/
#ifdef HAVE_PROTO
SCREEN*  PDC_CDECL   set_term( SCREEN *new )
#else
SCREEN*  PDC_CDECL   set_term(new)
SCREEN *new;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("set_term() - called\n");
#endif

#ifdef   TC
#  pragma argsused
#endif
   return( SP );  /* We only have one screen supported right now */
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   resize_term(int nlines, int ncols)
#else
int   PDC_CDECL   resize_term(nlines,ncols)
int nlines,ncols;
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("resize_term() - called: nlines %d\n",nlines);
#endif

   if (stdscr == (WINDOW *)NULL)
      return(ERR);

   if (PDC_resize_screen(nlines,ncols) == ERR)
      return ( ERR );

   SP->lines = PDC_get_rows();
   LINES = SP->lines - SP->linesrippedoff - SP->slklines;
   SP->cols  = COLS  = PDC_get_columns();

   /*
    * We need to change the saved prog_mode details...
    */
   if ( c_pr_tty.been_set )
   {
      c_pr_tty.saved.lines = SP->lines;
      c_pr_tty.saved.cols = SP->cols;
   }

   if ((curscr = resize_window(curscr, SP->lines, SP->cols)) == NULL)
      return (ERR);

   if ((stdscr = resize_window(stdscr, LINES, COLS)) == NULL)
      return (ERR);

   if (SP->slk_winptr)
   {
      if ((SP->slk_winptr = resize_window(SP->slk_winptr,SP->slklines, COLS)) == NULL)
         return (ERR);
      wmove(SP->slk_winptr, 0, 0);
      wclrtobot(SP->slk_winptr);
      (*PDC_initial_slk)();
      slk_noutrefresh();
   }

   touchwin(stdscr);
   wnoutrefresh(stdscr);

   return (OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
bool  PDC_CDECL   is_termresized(void)
#else
bool  PDC_CDECL   is_termresized()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("is_termresized() - called\n");
#endif
   return( SP->resized );
}
