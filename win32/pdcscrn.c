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
#define  INCLUDE_WINDOWS_H
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCscrn  = "$Id: pdcscrn.c,v 1.15 2006/01/04 00:19:50 wmcbrine Exp $";
#endif

#define PDC_RESTORE_NONE     0
#define PDC_RESTORE_BUFFER   1
#define PDC_RESTORE_WINDOW   2

HANDLE hConOut = INVALID_HANDLE_VALUE;
HANDLE hConIn = INVALID_HANDLE_VALUE;
#if defined(PDC_THREAD_BUILD)
HANDLE hPipeRead = INVALID_HANDLE_VALUE;
HANDLE hPipeWrite = INVALID_HANDLE_VALUE;
HANDLE hSemKeyCount = INVALID_HANDLE_VALUE;
extern LONG InputThread( LPVOID lpThreadData );
#endif

CONSOLE_SCREEN_BUFFER_INFO scr;
CONSOLE_SCREEN_BUFFER_INFO orig_scr;

static CHAR_INFO *ciSaveBuffer=NULL;
static CHAR_INFO *save_ci=NULL;
static DWORD dwConsoleMode=0;

/*man-start*********************************************************************

  PDC_scr_close() - Internal low-level binding to close the physical screen

  PDCurses Description:
   This is a nop for the DOS platform.

  PDCurses Return Value:
   This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
   The DOS platform will never fail.

  Portability:
   PDCurses int   PDC_scr_close( void );

**man-end**********************************************************************/

int   PDC_scr_close(void)
{
 COORD origin;
 SMALL_RECT rect;

   PDC_LOG(("PDC_scr_close() - called\n"));

   /*
    * All of this code should probably go into DllMain() at DLL_PROCESS_DETACH
    */
   SetConsoleScreenBufferSize(hConOut, orig_scr.dwSize);
   SetConsoleWindowInfo(hConOut,TRUE,&orig_scr.srWindow);
   SetConsoleScreenBufferSize(hConOut, orig_scr.dwSize);
   SetConsoleWindowInfo(hConOut,TRUE,&orig_scr.srWindow);

   if (SP->_restore == PDC_RESTORE_WINDOW)
   {
      rect.Top = orig_scr.srWindow.Top;
      rect.Left = orig_scr.srWindow.Left;
      rect.Bottom = orig_scr.srWindow.Bottom;
      rect.Right = orig_scr.srWindow.Right;
      origin.X = origin.Y = 0;
      if (!WriteConsoleOutput(hConOut,ciSaveBuffer,orig_scr.dwSize,origin,&rect))
         return(ERR);
   }
   else if (SP->_restore == PDC_RESTORE_BUFFER)
   {
      rect.Top = rect.Left = 0;
      rect.Bottom = orig_scr.dwSize.Y  - 1;
      rect.Right = orig_scr.dwSize.X - 1;
      origin.X = origin.Y = 0;
      if (!WriteConsoleOutput(hConOut,ciSaveBuffer,orig_scr.dwSize,origin,&rect))
         return(ERR);
   }

   SetConsoleActiveScreenBuffer(hConOut);
   SetConsoleMode(hConIn, dwConsoleMode);
#if defined(PDC_THREAD_BUILD)
   if ( hPipeRead != INVALID_HANDLE_VALUE )
      CloseHandle( hPipeRead );
   if ( hPipeWrite != INVALID_HANDLE_VALUE )
      CloseHandle( hPipeWrite );
#endif

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

bool  PDC_scrn_modes_equal(int mode1, int mode2)
{
   PDC_LOG(("PDC_scrn_modes_equal() - called\n"));

   return (mode1 == mode2);
}

/*man-start*********************************************************************

  PDC_scr_open()  - Internal low-level binding to open the physical screen

  PDCurses Description:
   This is a NOP for the DOS platform.

  PDCurses Return Value:
   This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:
   The DOS platform will never fail.

  Portability:
   PDCurses int   PDC_scr_open( SCREEN* internal, bool echo );

**man-end**********************************************************************/

int   PDC_scr_open(SCREEN *internal, bool echo)
{
   COORD bufsize,origin;
   SMALL_RECT rect;
   char *str;
   CONSOLE_SCREEN_BUFFER_INFO csbi;
#if defined(PDC_THREAD_BUILD)
   HANDLE hThread;
   DWORD dwThreadID;
#endif

   PDC_LOG(("PDC_scr_open() - called\n"));

   hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
   hConIn = GetStdHandle(STD_INPUT_HANDLE);

   GetConsoleScreenBufferInfo(hConOut, &csbi);
   GetConsoleScreenBufferInfo(hConOut, &orig_scr);
   GetConsoleMode(hConIn, &dwConsoleMode);

   if ((str = getenv("LINES")) != NULL)
      internal->lines = atoi(str);
   else
      internal->lines   = PDC_get_rows();

   if ((str = getenv("COLS")) != NULL)
      internal->cols = atoi(str);
   else
      internal->cols = PDC_get_columns();

   if (internal->lines < 2
   ||  internal->lines > csbi.dwMaximumWindowSize.Y)
   {
      fprintf(stderr,"LINES value must be >= 2 and <= %d: got %d\n",
            csbi.dwMaximumWindowSize.Y,
            internal->lines);
      return(ERR);
   }

   if (internal->cols < 2
   ||  internal->cols > csbi.dwMaximumWindowSize.X)
   {
      fprintf(stderr,"COLS value must be >= 2 and <= %d: got %d\n",
            csbi.dwMaximumWindowSize.X,
            internal->cols);
      return(ERR);
   }
   if ( getenv( "PDC_ORIGINAL_COLORS" ) != NULL )
   {
      chtype orig_back, orig_fore;
      orig_fore = csbi.wAttributes & 0x0f;
      orig_back = (csbi.wAttributes & 0xf0) >> 4;
      internal->orig_attr = (orig_back << 16) | orig_fore;
   }
   else
   {
      internal->orig_attr = 0;
   }

   internal->_restore = PDC_RESTORE_NONE;
   if (getenv("PDC_RESTORE_SCREEN") != NULL)
   {
      /*
       * Attempt to save the complete console buffer...
       */
      if ((ciSaveBuffer = (CHAR_INFO*)malloc(orig_scr.dwSize.X*orig_scr.dwSize.Y*sizeof(CHAR_INFO))) == NULL)
      {
         PDC_LOG(("PDC_scr_open() - exiting at line %d\n",__LINE__));

         return(ERR);
      }
      bufsize.X = orig_scr.dwSize.X;
      bufsize.Y = orig_scr.dwSize.Y;

      origin.X = origin.Y = 0;

      rect.Top = rect.Left = 0;
      rect.Bottom = orig_scr.dwSize.Y  - 1;
      rect.Right = orig_scr.dwSize.X - 1;
      if (!ReadConsoleOutput(hConOut,ciSaveBuffer,bufsize,origin,&rect))
      {
         /*
          * We can't save the complete buffer, so try and save just the displayed window...
          */
         free(ciSaveBuffer);
         ciSaveBuffer = NULL;

         bufsize.X = orig_scr.srWindow.Right - orig_scr.srWindow.Left + 1;
         bufsize.Y = orig_scr.srWindow.Bottom - orig_scr.srWindow.Top + 1;
         if ((ciSaveBuffer = (CHAR_INFO*)malloc(bufsize.X*bufsize.Y*sizeof(CHAR_INFO))) == NULL)
         {
            CHAR LastError[256];
            ULONG last_error = GetLastError();
            FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, last_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), LastError, 256, NULL ) ;

            PDC_LOG(("PDC_scr_open() - exiting at line %d: %s\n",__LINE__,LastError));

            return(ERR);
         }

         origin.X = origin.Y = 0;

         rect.Top = orig_scr.srWindow.Top;
         rect.Left = orig_scr.srWindow.Left;
         rect.Bottom = orig_scr.srWindow.Bottom;
         rect.Right = orig_scr.srWindow.Right;
         if (!ReadConsoleOutput(hConOut,ciSaveBuffer,bufsize,origin,&rect))
         {
            CHAR LastError[256];
            ULONG last_error = GetLastError();
            FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, last_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), LastError, 256, NULL ) ;
            free(ciSaveBuffer);
            ciSaveBuffer = NULL;

            PDC_LOG(("PDC_scr_open() - exiting at line %d: %s\n",__LINE__,LastError));

            return(ERR);
         }
         internal->_restore = PDC_RESTORE_WINDOW;
      }
      else
         internal->_restore = PDC_RESTORE_BUFFER;
   }

   if (getenv("PDC_PRESERVE_SCREEN") != NULL)
   {
      bufsize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
      bufsize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
      if ((save_ci = (CHAR_INFO*)malloc(bufsize.X*bufsize.Y*sizeof(CHAR_INFO))) == NULL)
      {
         CHAR LastError[256];
         ULONG last_error = GetLastError();
         FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, last_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), LastError, 256, NULL ) ;

         PDC_LOG(("PDC_scr_open() - exiting at line %d: %s\n",__LINE__,LastError));

         return(ERR);
      }

      origin.X = origin.Y = 0;

      rect.Top = csbi.srWindow.Top;
      rect.Left = csbi.srWindow.Left;
      rect.Bottom = csbi.srWindow.Bottom;
      rect.Right = csbi.srWindow.Right;
      if (!ReadConsoleOutput(hConOut,save_ci,bufsize,origin,&rect))
      {
         CHAR LastError[256];
         ULONG last_error = GetLastError();
         FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, last_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), LastError, 256, NULL ) ;
         free(save_ci);
         save_ci = NULL;

         PDC_LOG(("PDC_scr_open() - exiting at line %d: %s\n",__LINE__,LastError));

         return(ERR);
      }
      internal->_preserve = TRUE;
   }
   else
      internal->_preserve = FALSE;

   bufsize.X = orig_scr.srWindow.Right - orig_scr.srWindow.Left + 1;
   bufsize.Y = orig_scr.srWindow.Bottom - orig_scr.srWindow.Top + 1;
   rect.Top = rect.Left = 0;
   rect.Bottom = bufsize.Y - 1;
   rect.Right = bufsize.X - 1;

   SetConsoleScreenBufferSize(hConOut, bufsize);
   SetConsoleWindowInfo(hConOut,TRUE,&rect);
   SetConsoleScreenBufferSize(hConOut, bufsize);
   SetConsoleActiveScreenBuffer(hConOut);

   PDC_reset_prog_mode();

   PDC_get_cursor_pos(&internal->cursrow, &internal->curscol);
   internal->direct_video  = TRUE; /* Assume that we can      */
   internal->autocr  = TRUE;     /* lf -> crlf by default     */
   internal->raw_out = FALSE; /* tty I/O modes       */
   internal->raw_inp = FALSE; /* tty I/O modes       */
   internal->cbreak  = TRUE;
   internal->save_key_modifiers  = FALSE;
   internal->return_key_modifiers   = FALSE;
   internal->echo = echo;
   internal->refrbrk = FALSE;  /* no premature end of refresh*/
   internal->video_seg  = 0xb000;   /* Base screen segment addr   */
   internal->video_ofs  = 0x0;      /* Base screen segment ofs    */
   internal->video_page = 0;     /* Current Video Page         */
   internal->visible_cursor= TRUE;  /* Assume that it is visible  */
   internal->cursor  = PDC_get_cursor_mode();

   internal->adapter = PDC_query_adapter_type();
   internal->scrnmode   = PDC_get_scrn_mode();

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

   internal->os_version = GetVersion();
#if defined(PDC_THREAD_BUILD)
   /*
    * Create the anonymous pipe and thread for handling input
    */
   if ( !CreatePipe( &hPipeRead,     // reading handle
                     &hPipeWrite,    // writing handle
                     NULL,           // handles not inherited
                     0 ) )           // default buffer size
   {
      // error during pipe creation
      fprintf( stderr, "Cannot create input pipe\n" );
      return( ERR );
   }
   hThread = CreateThread( NULL,              // security attributes
                           0,                 // initial stack size
                           (LPTHREAD_START_ROUTINE) InputThread,
                           NULL,              // argument
                           CREATE_SUSPENDED,  // creation flag
                           &dwThreadID );     // new thread ID
   if ( ! hThread )
   {
      fprintf( stderr, "Cannot create input thread\n" );
      return( ERR );
   }
   ResumeThread( hThread );
#endif

   return( OK );
}

static BOOL FitConsoleWindow(HANDLE hConOut, CONST SMALL_RECT *rect)
/* Calls SetConsoleWindowInfo with the given parameters but fits them if a
 * scoll bar shrinks the maximum possible value. The rectangle must at least
 * fit in a half-sized window.
 */
{
 SMALL_RECT run;
 SHORT mx, my;

   if (SetConsoleWindowInfo(hConOut, TRUE, rect))
      return(TRUE);

   run = *rect;
   run.Right /= 2;
   run.Bottom /= 2;
   mx = run.Right;
   my = run.Bottom;

   if (!SetConsoleWindowInfo(hConOut, TRUE, &run))
      return(FALSE);
   for (run.Right = rect->Right;run.Right >= mx;run.Right--)
   {
      if (SetConsoleWindowInfo(hConOut, TRUE, &run))
         break;
   }
   if (run.Right < mx)
      return(FALSE);
   for (run.Bottom = rect->Bottom;run.Bottom >= my;run.Bottom--)
   {
      if (SetConsoleWindowInfo(hConOut, TRUE, &run))
         return(TRUE);
   }
   return(FALSE);
}

/*man-start*********************************************************************

  PDC_resize_screen()   - Internal low-level function to resize screen

  PDCurses Description:
   This function provides a means for the application program to
   resize the overall dimensions of the screen.  Under DOS and OS/2
   the application can tell PDCurses what size to make the screen;
   under X11, resizing is done by the user and this function simply
   adjusts its internal structures to fit the new size.
   This function doesn't set LINES, COLS, SP->lines or SP->cols. This
   must be done by resize_term.
   If both arguments are 0 the function returns sucessfully. This
   allows the calling routine to reset the SP->resized flag.
   The functions fails if one of the arguments is less then 2.

  PDCurses Return Value:
   This function returns OK on success, otherwise an ERR is returned.

  PDCurses Errors:

  Portability:
   PDCurses int   PDC_resize_screen( int, int );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_resize_screen(int nlines, int ncols)
#else
int   PDC_resize_screen(nlines, ncols)
int nlines,ncols;
#endif
/***********************************************************************/
{
#ifdef MHES
   COORD size, max;
   SMALL_RECT rect,displayarea = {0,0,0,0};
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   int external_resized = SP->resized;

   PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",nlines,ncols));

   SP->resized = FALSE; /* prevent endless loops in case of errors */

   if (nlines == 0
   && ncols == 0)
      return(OK); /* undocumented feature: let assign LINES and COLS
                          to current values by the calling
                          resize_term-function */

   if (nlines < 2 || ncols < 2)
      return(ERR);

   if (!GetConsoleScreenBufferInfo(hConOut,&csbi)) /*needed for recovery*/
      return(ERR);
   max = GetLargestConsoleWindowSize(hConOut);

   size.X = ncols;
   size.Y = nlines;

   /* Fit window into allowed values */
   rect.Left = rect.Top = 0;
   rect.Right = ncols - 1;
   if (rect.Right >= max.X)
      rect.Right = max.X;
   rect.Bottom = rect.Top + nlines - 1;
   if (rect.Bottom >= max.Y)
      rect.Bottom = max.Y;
   FitConsoleWindow(hConOut, &rect); /* helps to allow the BufferSize */
   displayarea.Right = rect.Right;
   displayarea.Bottom = rect.Bottom;
   if (!SetConsoleScreenBufferSize(hConOut, size) ||
       !FitConsoleWindow(hConOut, &rect) ||
       !SetConsoleWindowInfo(hConOut, TRUE, &displayarea))
   {
      SetConsoleScreenBufferSize(hConOut, csbi.dwSize);
      SetConsoleWindowInfo(hConOut, TRUE, &csbi.srWindow);
      return( (external_resized) ? OK : ERR );
   }

   SetConsoleActiveScreenBuffer(hConOut);
   return ( OK );
#elif defined(FGC)
   int rc=OK;
   COORD size;
   SMALL_RECT rect;

   PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",nlines,ncols));

   if (nlines < 2)
      return(ERR);
   SP->lines = LINES = nlines;

   if (ncols < 2)
      return(ERR);
   SP->cols = COLS = ncols;

   size.X = SP->cols;
   size.Y = SP->lines;
   rect.Top = rect.Left = 0;
   rect.Bottom = SP->lines - 1;
   rect.Right = SP->cols - 1;
   SetConsoleScreenBufferSize(hConOut, size);
   SetConsoleWindowInfo(hConOut,TRUE,&rect);
   SetConsoleScreenBufferSize(hConOut, size);
   SetConsoleWindowInfo(hConOut,TRUE,&rect);
   SetConsoleActiveScreenBuffer(hConOut);
   SP->resized = FALSE;
   return ( rc );
#else
   int rc=OK;
   SMALL_RECT rect;
   COORD size, max;

   if (nlines < 2 || ncols < 2)
      return(ERR);
   max = GetLargestConsoleWindowSize(hConOut);

   rect.Left = rect.Top = 0;
   rect.Right = ncols - 1;
   if (rect.Right >= max.X)
      rect.Right = max.X;
   rect.Bottom = nlines - 1;
   if (rect.Bottom >= max.Y)
      rect.Bottom = max.Y;
   size.X = rect.Right + 1;
   size.Y = rect.Bottom + 1;
   FitConsoleWindow(hConOut, &rect);
   SetConsoleScreenBufferSize(hConOut, size);
   FitConsoleWindow(hConOut, &rect);
   SetConsoleScreenBufferSize(hConOut, size);


   SetConsoleActiveScreenBuffer(hConOut);
   SP->resized = FALSE;
   return ( rc );
#endif
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_reset_prog_mode(void)
#else
int   PDC_reset_prog_mode()
#endif
/***********************************************************************/
{
   SetConsoleMode(hConIn, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
   return(OK);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_reset_shell_mode(void)
#else
int   PDC_reset_shell_mode()
#endif
/***********************************************************************/
{
   SetConsoleMode(hConIn, dwConsoleMode);
   return(OK);
}

#if defined(PDC_DLL_BUILD)
BOOL WINAPI DllMain( HINSTANCE hDLL, DWORD dwReason, LPVOID pReserved)
{
   switch( dwReason)
   {
      case DLL_PROCESS_ATTACH:
/*         fprintf(stderr,"DLL_PROCESS_ATTACH\n"); */
         break;
      case DLL_PROCESS_DETACH:
/*         fprintf(stderr,"DLL_PROCESS_DETACH\n"); */
         break;
      case DLL_THREAD_ATTACH:
/*         fprintf(stderr,"DLL_THREAD_ATTACH\n"); */
         break;
      case DLL_THREAD_DETACH:
/*         fprintf(stderr,"DLL_THREAD_DETACH\n"); */
         break;
   }
   return(TRUE);
}
#endif
