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
#include <defs.h>
#include <term.h>
#endif

/* undefine any macros for functions defined in this module */
#undef   def_prog_mode
#undef   def_shell_mode
#undef   reset_prog_mode
#undef   reset_shell_mode
#undef   resetty
#undef   savetty
#undef   ripoffline
#undef   curs_set
#undef   napms
#undef   draino

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef move
#  undef wmove
#endif

#ifdef PDCDEBUG
char *rcsid_kernel  = "$Id: kernel.c,v 1.3 2002/11/27 11:24:31 mark Exp $";
#endif

RIPPEDOFFLINE linesripped[5];
char linesrippedoff=0;

/*man-start*********************************************************************

  Name:                                                        kernel

  Synopsis:
   int def_prog_mode(void);
   int def_shell_mode(void);
   int reset_prog_mode(void);
   int reset_shell_mode(void);
   int resetty(void);
   int savetty(void);
   void getsyx(int y, int x);
   void setsyx(int y, int x);
   int ripoffline(int line, int (*init)(WINDOW *,int));
   int curs_set(int visibility);
   int napms(int ms);
   int draino(int ms);

  X/Open Description:
   The def_prog_mode() and def_shell_mode() functions save the 
   current terminal modes as the "program" (in curses) or
   "shell" (not in curses) state for use by the reset_prog_mode()
   and reset_shell_mode() functions.  This is done automatically by
   initscr().

   The reset_prog_mode() and reset_shell_mode() functions restore 
   the terminal to "program" (in curses) or "shell" (not in curses)
   state.  These are done automatically by endwin()
   and doupdate() after an endwin(), so they would normally not
   be called before these functions.

   The savetty() and resetty() routines save and restore the state of 
   the terminal modes. The savetty() function saves the current state 
   in a buffer and resetty() restores the state to what it was at the 
   last call to savetty().

   The getsyx() routine obtains the coordinates of the virtual screen
   cursor. If leaveok() is currently TRUE, then -1, -1 is returned. If
   lines have been removed from the top of the screen with ripoffline()
   then getsyx() includes those lines, so y and x should only be used
   by setyx().
   The setyx() routine sets the cursor position of the virtual screen
   to the y,x coordinates. If y,x are -1,-1, leaveok() is set TRUE.
   The getsyx() and setsyx() routines are designed to be used by a
   library routine that manipulates curses windows, but does not want
   to change the position of the cursor.

   Note that getsyx() and setsyx() are defined as macros only. System VR4
   defines these as having a return type of int, but that is misleading as
   there is no documented sematics for the return value.

   The curs_set() function enables the appearance of the text cursor
   to be altered. A value of 0 for visibility makes the cursor
   disappear; a value of 1 makes the cursor appear "normal" (usually
   an underline) and 2 makes the cursor "highly visible"; a block.

   The ripoffline() function allows the user to reduce the size of
   stdscr by 1 line.  If the value of line is positive, the line is 
   removed from the top of the screen; negative from the bottom. Up to
   5 lines can be ripped off stdscr by calling ripoffline()
   consecutively.
   The function argument, init, is called from within initscr() or
   newterm(), so ripoffline() must be called before either of these
   functions.  The init function is passed a pointer to a 1 line
   WINDOW and the width of the window. Calling ripoffline() with a
   NULL initialise function pointer is not advisable!

   The napms() and draino() functions, suspends the program for the
   specified number of milliseconds.

  PDCurses Description:
   FYI: It is very unclear whether savetty() and resetty() functions
   are a duplication of the reset_prog_mode() and reset_shell_mode() 
   functions or whether this is a backing store type of operation.  
   At this time, they are implemented similar to the reset_*_mode() 
   routines.

   The curs_set() routine is used to set the visibility of the cursor.
   The cursor can be made invisible, normal or highly visible by setting
   the parameter to 0, 1 or 2 respectively. If an invalid value is passed
   the function will set the cursor to "normal".

  X/Open Return Value:
   All functions return OK on success and ERR on error except curs_set()
   which returns the previous visibility.

  X/Open Errors:
   No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      def_prog_mode                         Y        Y       Y
      def_shell_mode                        Y        Y       Y
      reset_prog_mode                       Y        Y       Y
      reset_shell_mode                      Y        Y       Y
      resetty                               Y        Y       Y
      savetty                               Y        Y       Y
      getsyx                                -        -      3.0
      setsyx                                -        -      3.0
      ripoffline                            -        -      3.0
      curs_set                              -        -      3.0
      napms                                 Y        Y       Y

**man-end**********************************************************************/

#ifndef UNIX
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   def_prog_mode(void)
#else
int   PDC_CDECL   def_prog_mode()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("def_prog_mode() - called\n");
#endif

#ifdef   FLEXOS
   _flexos_16bitmode();
#endif
   c_pr_tty.been_set = TRUE;

   memcpy(&c_pr_tty.saved, SP, sizeof(SCREEN));

   return( OK );
}
#endif

#ifndef UNIX
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   def_shell_mode(void)
#else
int   PDC_CDECL   def_shell_mode()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("def_shell_mode() - called\n");
#endif

#ifdef   FLEXOS
   _flexos_8bitmode();
#endif
   c_sh_tty.been_set = TRUE;

   memcpy(&c_sh_tty.saved, SP, sizeof(SCREEN));

   return( OK );
}
#endif

#ifndef UNIX
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL reset_prog_mode(void)
#else
int PDC_CDECL reset_prog_mode()
#endif
/***********************************************************************/
{
#if defined(OS2) && !defined(EMXVIDEO)
   VIOMODEINFO modeInfo;
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("reset_prog_mode() - called\n");
#endif
   if (c_pr_tty.been_set == TRUE)
   {
      memcpy(SP, &c_pr_tty.saved, sizeof(SCREEN));

      mvcur(0, 0, c_pr_tty.saved.cursrow, c_pr_tty.saved.curscol);
      if (PDC_get_ctrl_break() != c_pr_tty.saved.orgcbr)
         PDC_set_ctrl_break(c_pr_tty.saved.orgcbr);
      if (c_pr_tty.saved.raw_out)
         raw();
      if (c_pr_tty.saved.visible_cursor)
         PDC_cursor_on();
      SP->font = PDC_get_font();
      PDC_set_font(c_pr_tty.saved.font);
#if !defined (XCURSES)
# if !defined(EMXVIDEO)
#  if defined(OS2)
      (void)PDC_get_scrn_mode( &modeInfo );
      if ( !PDC_scrn_modes_equal( modeInfo, c_pr_tty.saved.scrnmode ) )
         PDC_set_scrn_mode( c_pr_tty.saved.scrnmode );
#  else
      if ( !PDC_scrn_modes_equal( PDC_get_scrn_mode(), c_pr_tty.saved.scrnmode ) )
         PDC_set_scrn_mode( c_pr_tty.saved.scrnmode );
#  endif
# endif
#endif
      PDC_set_rows(c_pr_tty.saved.lines);
   }
#ifdef FLEXOS
   _flexos_16bitmode();
#endif

#if defined(WIN32) || (defined(OS2) && !defined(EMXVIDEO))
   PDC_reset_prog_mode();
#endif
   return( OK );
}
#endif

#ifndef UNIX
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL reset_shell_mode(void)
#else
int PDC_CDECL reset_shell_mode()
#endif
/***********************************************************************/
{
#if defined(OS2) && !defined(EMXVIDEO)
   VIOMODEINFO modeInfo;
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("reset_shell_mode() - called\n");
#endif

#ifndef WIN32
   if (c_sh_tty.been_set == TRUE)
   {
      memcpy(SP, &c_sh_tty.saved, sizeof(SCREEN));

      mvcur(0, 0, c_sh_tty.saved.cursrow, c_sh_tty.saved.curscol);
      if (PDC_get_ctrl_break() != c_sh_tty.saved.orgcbr)
         PDC_set_ctrl_break(c_sh_tty.saved.orgcbr);
      if (c_sh_tty.saved.raw_out)
         raw();
      if (c_sh_tty.saved.visible_cursor)
         PDC_cursor_on();
      SP->font = PDC_get_font();
      PDC_set_font(c_sh_tty.saved.font);
#if !defined (XCURSES)
# if !defined(EMXVIDEO)
#  if defined(OS2)
      (void)PDC_get_scrn_mode( &modeInfo );
      if ( !PDC_scrn_modes_equal( modeInfo, c_sh_tty.saved.scrnmode ) )
         PDC_set_scrn_mode( c_sh_tty.saved.scrnmode );
#  else
      if ( !PDC_scrn_modes_equal( PDC_get_scrn_mode(), c_sh_tty.saved.scrnmode ) )
         PDC_set_scrn_mode( c_sh_tty.saved.scrnmode );
#  endif
# endif
#endif
#if defined(OS2) || defined(WIN32)
      PDC_resize_screen(c_sh_tty.saved.lines,c_sh_tty.saved.cols);
#else
      PDC_set_rows(c_sh_tty.saved.lines);
#endif
   }
#endif

#ifdef   FLEXOS
   _flexos_8bitmode();
#endif

#if defined(WIN32) || (defined(OS2) && !defined(EMXVIDEO))
   PDC_reset_shell_mode();
#endif

   return( OK );
}
#endif

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL resetty(void)
#else
int PDC_CDECL resetty()
#endif
/***********************************************************************/
{
#if defined(OS2) && !defined(EMXVIDEO)
   VIOMODEINFO modeInfo;
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("resetty() - called\n");
#endif

#ifndef UNIX
   if (c_save_tty.been_set == TRUE)
   {
      memcpy(SP, &c_save_tty.saved, sizeof(SCREEN));
      mvcur(0, 0, c_save_tty.saved.cursrow, c_save_tty.saved.curscol);
      if (PDC_get_ctrl_break() != c_save_tty.saved.orgcbr)
         PDC_set_ctrl_break(c_save_tty.saved.orgcbr);
      if (c_save_tty.saved.raw_out)
         raw();
      if (c_save_tty.saved.visible_cursor)
         PDC_cursor_on();
      SP->font = PDC_get_font();
      PDC_set_font(c_save_tty.saved.font);
# if !defined (XCURSES)
#  if !defined(EMXVIDEO)
#   if defined(OS2)
      (void)PDC_get_scrn_mode( &modeInfo );
      if ( !PDC_scrn_modes_equal( modeInfo, c_save_tty.saved.scrnmode ) )
         PDC_set_scrn_mode( c_save_tty.saved.scrnmode );
#   else
      if ( !PDC_scrn_modes_equal( PDC_get_scrn_mode(), c_save_tty.saved.scrnmode ) )
         PDC_set_scrn_mode( c_save_tty.saved.scrnmode );
#   endif
#  endif
# endif
# if defined(OS2)
      PDC_resize_screen(c_save_tty.saved.lines,c_save_tty.saved.cols);
# else
      PDC_set_rows(c_save_tty.saved.lines);
# endif
   }
#endif
   return( c_save_tty.been_set ? OK : ERR );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   savetty(void)
#else
int   PDC_CDECL   savetty()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("savetty() - called\n");
#endif

   c_save_tty.been_set = TRUE;
   memcpy(&c_save_tty.saved, SP, sizeof(SCREEN));
   return( OK );
}
#if 0
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   getsyx(int *y, int *x)
#else
int   PDC_CDECL   getsyx(y,x)
int *y,*x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("getsyx() - called\n");
#endif

   if (curscr->_leaveit)
      *y = *x = (-1);
   else
   {
      *y = curscr->_cury - SP->linesrippedoffontop;
      *x = curscr->_curx;
   }
   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   setsyx(int y, int x)
#else
int   PDC_CDECL   setsyx(y,x)
int y,x;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("setsyx() - called\n");
#endif

   if (y < 0 && x < 0)
      curscr->_leaveit = TRUE;
   else
   {
      curscr->_cury = y + SP->linesrippedoffontop;
      curscr->_curx = x;
      curscr->_leaveit = FALSE;
   }
   return( OK );
}
#endif
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   curs_set(int visibility)
#else
int   PDC_CDECL   curs_set(visibility)
int visibility;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("curs_set() - called: visibility=%d\n",visibility);
#endif

   return(PDC_curs_set(visibility));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   ripoffline(int line, int (*init)(WINDOW *,int))
#else
int   PDC_CDECL   ripoffline(line, init)
int line;
int (*init)();
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("ripoffline() - called: line=%d\n",line);
#endif

   if (linesrippedoff < 5
   &&  line != 0)
   {
   linesripped[(int)linesrippedoff].line = line;
   linesripped[(int)linesrippedoff++].init = init;
   }
   return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   napms(int ms)
#else
int   PDC_CDECL   napms(ms)
int ms;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("napms() - called: ms=%d\n",ms);
#endif
   return(delay_output(ms));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   draino(int ms)
#else
int   PDC_CDECL   draino(ms)
int ms;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("draino() - called: ms=%d\n",ms);
#endif
   return(delay_output(ms));
}
