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
#define INCL_DOSMISC
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCscrn  = "$Id: pdcscrn.c,v 1.3 2002/01/12 04:04:18 mark Exp $";
#endif

#ifdef EMXVIDEO
   static unsigned char *saved_screen = NULL;
   static int saved_lines = 0;
   static int saved_cols = 0;
#else
   static PCH saved_screen = NULL;
   static USHORT saved_lines = 0;
   static USHORT saved_cols = 0;
#endif

/*man-start*********************************************************************

  PDC_scr_close() - Internal low-level binding to close the physical screen

  PDCurses Description:
   This function provides a low-level binding for the Flexos
   platform which must close the screen before writing to it.
   This is a nop for the DOS platform.

   This function is provided in order to reset the FlexOS 16 bit
   character set for input rather than the limited input
   character set associated with the VT52.

  PDCurses Return Value:
   This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
   The DOS platform will never fail.  The Flexos platform may fail
   depending on the ability to close the current virtual console in
   8 (as opposed to 16) bit mode.

  Portability:
   PDCurses int   PDC_scr_close( void );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_scr_close(void)
/***********************************************************************/
{
   char *ptr;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_scr_close() - called\n");
#endif
#ifdef EMXVIDEO
   ptr = getenv("PDC_RESTORE_SCREEN");
#else
   if ( DosScanEnv( "PDC_RESTORE_SCREEN", (PSZ *)&ptr ) )
      ptr = NULL;
#endif
   if (ptr != NULL)
   {
      if (saved_screen == NULL)
         return( OK );
#ifdef EMXVIDEO
      v_putline(saved_screen,0,0,saved_lines*saved_cols);
#else
      VioWrtCellStr(saved_screen,saved_lines*saved_cols*2,0,0,(HVIO)NULL);
#endif
      free(saved_screen);
      saved_screen = NULL;
   }
   return( OK );
}

/*man-start*********************************************************************

  PDC_scrn_modes_equal()   - Decide if two screen modes are equal

  PDCurses Description:
   Mainly required for OS/2. It decides if two screen modes
        (VIOMODEINFO structure) are equal. Under DOS it just compares
        two integers

  PDCurses Return Value:
   This function returns TRUE if equal else FALSe.

  PDCurses Errors:
   No errors are defined for this function.

  Portability:
   PDCurses int PDC_scrn_modes_equal( int mode1, int mode2 );
   OS2 PDCurses   int PDC_scrn_modes_equal( VIOMODEINFO mode1, VIOMODEINFO mode2 );

**man-end**********************************************************************/

/***********************************************************************/
#if !defined( EMXVIDEO )
bool PDC_scrn_modes_equal(VIOMODEINFO mode1, VIOMODEINFO mode2)
#else
bool PDC_scrn_modes_equal(int mode1, int mode2)
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_scrn_modes_equal() - called\n");
#endif

#if !defined( EMXVIDEO )
   return ((mode1.cb == mode2.cb) && (mode1.fbType == mode2.fbType)
      && (mode1.color == mode2.color) && (mode1.col == mode2.col)
      && (mode1.row == mode2.row) && (mode1.hres == mode2.vres)
      && (mode1.vres == mode2.vres) );
#else
   return (mode1 == mode2);
#endif
}

/*man-start*********************************************************************

  PDC_scr_open()  - Internal low-level binding to open the physical screen

  PDCurses Description:
   This function provides a low-level binding for the Flexos
   platform which must open the screen before writing to it.

   This function is provided in order to access the FlexOS 16 bit
   character set for input rather than the limited input
   character set associated with the VT52.

  PDCurses Return Value:
   This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
   The DOS platform will never fail.  The Flexos platform may fail
   depending on the ability to open the current virtual console in
   8 (as opposed to 16) bit mode.

  Portability:
   PDCurses int   PDC_scr_open( SCREEN* internal, bool echo );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_scr_open(SCREEN *internal, bool echo)
/***********************************************************************/
{
   char *ptr;
#if !defined( EMXVIDEO )
   USHORT totchars=0;
#endif
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_scr_open() - called. internal: %x, echo: %d\n", internal, echo);
#endif
   internal->orig_attr   = 0;
   internal->orig_emulation = 0;
   PDC_get_cursor_pos(&internal->cursrow, &internal->curscol);
   internal->direct_video  = TRUE;     /* Assume that we can         */
   internal->autocr  = TRUE;     /* lf -> crlf by default      */
   internal->raw_out = FALSE; /* tty I/O modes        */
   internal->raw_inp = FALSE; /* tty I/O modes        */
   internal->cbreak  = TRUE;
   internal->save_key_modifiers  = FALSE;
   internal->return_key_modifiers   = FALSE;
   internal->echo    = echo;
/* under System V Curses, typeahead checking is enabled by default */
   internal->refrbrk = TRUE;  /* allow premature end of refresh*/
   internal->video_page = 0;     /* Current Video Page         */
   internal->visible_cursor= TRUE;     /* Assume that it is visible  */
   internal->cursor  = PDC_get_cursor_mode();
#if defined(EMXVIDEO)
   internal->tahead  = -1;
#endif

#if !defined(EMXVIDEO)
   (void)PDC_query_adapter_type( &internal->adapter );
   (void)PDC_get_scrn_mode( &internal->scrnmode );
   (void)PDC_get_keyboard_info( &internal->kbdinfo );
# ifdef PDCDEBUG1
   if (trace_on) PDC_debug("PDC_scr_open() - after PDC_get_keyboard_info(). cb: %x, fsMask: %x, chTurnAround: %x, fsInterim: %x, fsState: %x\n",
      SP->kbdinfo.cb,
      SP->kbdinfo.fsMask,
      SP->kbdinfo.chTurnAround,
      SP->kbdinfo.fsInterim,
      SP->kbdinfo.fsState);
# endif
   /*
    * Now set the keyboard into binary mode
    */
   (void)PDC_set_keyboard_binary();
#else
   internal->adapter = PDC_query_adapter_type();
   if ( internal->adapter == _UNIX_MONO )
      internal->mono = TRUE;
#endif

   internal->orig_font = internal->font   = PDC_get_font();
   internal->lines      = PDC_get_rows();
   internal->cols    = PDC_get_columns();
   internal->audible = TRUE;
   internal->visibility = 1;
   internal->orig_cursor = internal->cursor;
   internal->orgcbr = PDC_get_ctrl_break();
   internal->blank = ' ';
   internal->resized = FALSE;
   internal->shell = FALSE;
   internal->_trap_mbe = 0L;
   internal->_map_mbe_to_key = 0L;
   internal->linesrippedoff = 0;
   internal->linesrippedoffontop = 0;
   internal->delaytenths = 0;
   internal->sizeable = TRUE;
   /*
    * This code for preserving the current screen...
    */
#ifdef EMXVIDEO
   ptr = getenv("PDC_RESTORE_SCREEN");
#else
   if ( DosScanEnv( "PDC_RESTORE_SCREEN", (PSZ *)&ptr ) )
      ptr = NULL;
#endif
   if (ptr != NULL)
   {
      saved_lines = internal->lines;
      saved_cols = internal->cols;
#ifdef EMXVIDEO
      if ((saved_screen = (unsigned char*)malloc(2*saved_lines*saved_cols*sizeof(unsigned char))) == NULL)
         return(ERR);
      v_getline(saved_screen,0,0,saved_lines*saved_cols);
#else
      if ((saved_screen = (PCH)malloc(2*saved_lines*saved_cols*sizeof(unsigned char))) == NULL)
         return(ERR);
      totchars = saved_lines*saved_cols*2;
      VioReadCellStr((PCH)saved_screen,&totchars,0,0,(HVIO)NULL);
#endif
   }

#ifdef EMXVIDEO
   ptr = getenv("PDC_PRESERVE_SCREEN");
#else
   if ( DosScanEnv( "PDC_PRESERVE_SCREEN", (PSZ *)&ptr ) )
      ptr = NULL;
#endif
   if (ptr != NULL)
      internal->_preserve = TRUE;
   else
      internal->_preserve = FALSE;

   return( OK );
}

/*man-start*********************************************************************

  PDC_resize_screen()   - Internal low-level function to resize screen

  PDCurses Description:
   This function provides a means for the application program to
   resize the overall dimensions of the screen.  Under DOS and OS/2
   the application can tell PDCurses what size to make the screen;
   under X11, resizing is done by the user and this function simply
   adjusts its internal structures to fit the new size.

  PDCurses Return Value:
   This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:

  Portability:
   PDCurses int   PDC_resize_screen( int, int );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_resize_screen(int nlines, int ncols)
/***********************************************************************/
{
#ifndef EMXVIDEO
   VIOMODEINFO modeInfo={0};
   USHORT result=0;
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_resize_screen() - called. Lines: %d Cols: %d\n",nlines,ncols);
#endif

#ifdef EMXVIDEO
   return (ERR);
#else
   modeInfo.cb = sizeof(modeInfo);
   /* set most parameters of modeInfo */
   VioGetMode(&modeInfo, 0);
   modeInfo.fbType = 1;
   modeInfo.row = nlines;
   modeInfo.col = ncols;
   result = VioSetMode(&modeInfo, 0);
   LINES = PDC_get_rows();
   COLS = PDC_get_columns();
   return ((result == 0) ? OK : ERR);
#endif
}

#if !defined(EMXVIDEO)
/***********************************************************************/
int PDC_reset_shell_mode( void )
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_reset_shell_mode() - called.\n");
#endif
   PDC_set_keyboard_default();
   return OK;
}

/***********************************************************************/
int PDC_reset_prog_mode( void )
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_reset_prog_mode() - called.\n");
#endif
   PDC_set_keyboard_binary();
   return OK;
}
#endif
