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
*
* This file is NOT public domain software.  It is Copyright, Mark Hessling
* 1994-2000.
***************************************************************************
*/

/*
 * This file contains functions that are called by the "curses" process;
 * ie the parent process.
 */

#include "pdcx11.h"

#ifdef PROTO
static void XCursesExitCursesProcess(int, char *);
#else
static void XCursesExitCursesProcess();
#endif

/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesResizeScreen(int nlines, int ncols)
#else
int XCursesResizeScreen(nlines, ncols)
int nlines,ncols;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesResizeScreen() - called: Lines: %d Cols: %d\n",(XCursesProcess)?"     X":"CURSES",nlines,ncols);
#endif
   shmdt((char *)Xcurscr);
   XCursesInstructAndWait(CURSES_RESIZE);
   if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,SP->XcurscrSize+XCURSESSHMMIN,0700)) < 0)
   {
      perror("Cannot allocate shared memory for curscr");
      kill(otherpid,SIGKILL);
      return(ERR);
   }
   XCursesLINES = SP->lines;
   XCursesCOLS = SP->cols;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:shmid_Xcurscr %d shmkey_Xcurscr %d SP->lines %d SP->cols %d\n",(XCursesProcess)?"     X":"CURSES",shmid_Xcurscr,shmkey_Xcurscr,SP->lines,SP->cols);
#endif
   Xcurscr = (unsigned char*)shmat(shmid_Xcurscr,0,0);
   atrtab = (unsigned char *)(Xcurscr+XCURSCR_ATRTAB_OFF);
   SP->resized=FALSE;
   return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_display_cursor(int oldrow, int oldcol, int newrow,int newcol,int visibility)
#else
int XCurses_display_cursor(oldrow,oldcol,newrow,newcol,visibility)
int oldrow,oldcol,newrow,newcol,visibility;
#endif
/***********************************************************************/
{
   char buf[30];
   int idx,pos;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_display_cursor() - called: NEW row %d col %d, vis %d\n",
                        (XCursesProcess)?"X":"CURSES",newrow,newcol,visibility);
#endif

   if ( visibility == -1 )
   {
      /*
       * Only send the CURSES_DISPLAY_CURSOR message, no data
       */
      idx = CURSES_DISPLAY_CURSOR;
      memcpy(buf,(char *)&idx,sizeof(int));
      idx = sizeof(int);
   }
   else
   {
      idx = CURSES_CURSOR;
      memcpy(buf,(char *)&idx,sizeof(int));
      idx = sizeof(int);
      pos = oldrow + (oldcol << 8);
      memcpy(buf+idx,(char *)&pos,sizeof(int));
      idx += sizeof(int);
      pos = newrow + (newcol << 8);
      memcpy(buf+idx,(char *)&pos,sizeof(int));
      idx += sizeof(int);
   }

   if (write_socket(display_sock,buf,idx) < 0)
      XCursesExitCursesProcess(1,"exitting from XCurses_display_cursor");

   return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
void XCurses_set_title(char *title)
#else
void XCurses_set_title(title)
char *title;
#endif
/***********************************************************************/
{
   char buf[30];
   int idx,len;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_set_title() - called: TITLE: %s\n",
                        (XCursesProcess)?"X":"CURSES",title);
#endif

   idx = CURSES_TITLE;
   memcpy(buf,(char *)&idx,sizeof(int));
   idx = sizeof(int);
   len = strlen(title)+1; /* write nul character */
   memcpy(buf+idx,(char *)&len,sizeof(int));
   idx += sizeof(int);

   if (write_socket(display_sock,buf,idx) < 0)
      XCursesExitCursesProcess(1,"exiting from XCurses_set_title");

   if (write_socket(display_sock,title,len) < 0)
      XCursesExitCursesProcess(1,"exiting from XCurses_set_title");

   return;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_refresh_scrollbar(void)
#else
int XCurses_refresh_scrollbar()
#endif
/***********************************************************************/
{
   char buf[30];
   int idx;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_refresh_scrollbar() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif

   idx = CURSES_REFRESH_SCROLLBAR;
   memcpy(buf,(char *)&idx,sizeof(int));
   idx = sizeof(int);

   if (write_socket(display_sock,buf,idx) < 0)
      XCursesExitCursesProcess(1,"exiting from XCurses_refresh_scrollbar");

   return(OK);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_rawgetch( int delaytenths)
#else
int XCurses_rawgetch( delaytenths )
int delaytenths;
#endif
/***********************************************************************/
{
   unsigned long newkey=0;
   int key=0;
   char buf[100]; /* big enough for MOUSE_STATUS struct */

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_rawgetch() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif

   while(1)
   {
      if ( delaytenths && !XCurses_kbhit() )
      {
         key = -1;
      }
      else
      {
         if (read_socket(key_sock,buf,sizeof(unsigned long)) < 0)
            XCursesExitCursesProcess(2,"exiting from XCurses_rawchar"); /* what else ?? */
         memcpy((char *)&newkey,buf,sizeof(unsigned long));
         pdc_key_modifier = newkey >> 24; /*(sizeof(unsigned long) - sizeof(unsigned char)); */
         key = (int)(newkey & 0x00FFFFFF);
      }
#if 0
 printf("%s:reading %d mod %ld\n",(XCursesProcess)?"     X":"CURSES",key,pdc_key_modifier);
#endif
      if (key == KEY_MOUSE)
      {
         if (read_socket(key_sock,buf,sizeof(MOUSE_STATUS)) < 0)
            XCursesExitCursesProcess(2,"exitting from XCurses_rawchar"); /* what else ?? */
         memcpy((char *)&Trapped_Mouse_status,buf,sizeof(MOUSE_STATUS));
         /*
          * Check if the mouse has been clicked on a slk area. If the return
          * value is > 0 (indicating the
          * label number, return with the KEY_F(key) value.
          */
/*fprintf(stderr,"%d %d %x\n",TRAPPED_MOUSE_Y_POS,TRAPPED_MOUSE_X_POS,SP->slk_winptr);*/
         if ((newkey = PDC_mouse_in_slk(TRAPPED_MOUSE_Y_POS,TRAPPED_MOUSE_X_POS)))
         {
            if (TRAPPED_BUTTON_STATUS(1) & BUTTON_PRESSED)
            {
               key = KEY_F(newkey);
               break;
            }
         }
         break;

#ifdef MOUSE_DEBUG
 printf("rawgetch-x: %d y: %d Mouse status: %x\n",
                                    MOUSE_X_POS,
                                    MOUSE_Y_POS,
                                    Mouse_status.changes);
 printf("rawgetch-Button1: %x Button2: %x Button3: %x\n",
                                    BUTTON_STATUS(1),
                                    BUTTON_STATUS(2),
                                    BUTTON_STATUS(3));
#endif
      }
      else
         break;
   }

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_rawgetch() - key %d returned\n",(XCursesProcess)?"     X":"CURSES",key);
#endif
   return(key);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_get_input_fd(void)
#else
int XCurses_get_input_fd()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCurses_get_input_fd() - called\n",(XCursesProcess)?"     X":"CURSES");
	if (trace_on) PDC_debug("%s:XCurses_get_input_fd() - returning %i\n",(XCursesProcess)?"     X":"CURSES",key_sock);
#endif
 return key_sock;
}

/***********************************************************************/
#ifdef HAVE_PROTO
bool XCurses_kbhit(void)
#else
bool XCurses_kbhit()
#endif
/***********************************************************************/
{
   int s;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_kbhit() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   /*
    * Is something ready to be read on the socket ? Must be a key.
    */
   FD_ZERO( &readfds );
   FD_SET( key_sock, &readfds );
   if ( ( s = select ( FD_SETSIZE, (FD_SET_CAST)&readfds, NULL, NULL, &socket_timeout ) ) < 0 )
      XCursesExitCursesProcess(3,"child - exiting from XCurses_kbhit select failed");
            
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_kbhit() - returning %s\n",(XCursesProcess)?"     X":"CURSES",(s == 0) ? "FALSE" : "TRUE");
#endif
   if ( s == 0 )
      return(FALSE);
   return(TRUE);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesInstruct(int flag)
#else
int XCursesInstruct(flag)
int flag;
#endif
/***********************************************************************/
{
   char buf[10];

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesInstruct() - called flag %d\n",(XCursesProcess)?"     X":"CURSES",flag);
#endif
   /*
    * Send a request to X...
    */
   memcpy(buf,(char *)&flag,sizeof(int));
   if (write_socket(display_sock,buf,sizeof(int)) < 0)
      XCursesExitCursesProcess(4,"exitting from XCursesInstruct");
   return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesInstructAndWait(int flag)
#else
int XCursesInstructAndWait(flag)
int flag;
#endif
/***********************************************************************/
{
   int result;
   char buf[10];

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesInstructAndWait() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   /*
    * Tell X we want to do something...
    */
   XCursesInstruct(flag);
   /*
    * ... wait for X to say the refresh has occurred.
    */
   if (read_socket(display_sock,buf,sizeof(int)) < 0)
      XCursesExitCursesProcess(5,"exiting from XCursesInstructAndWait");
   memcpy((char *)&result,buf,sizeof(int));
   if (result != CURSES_CONTINUE)
      XCursesExitCursesProcess(6,"exitting from XCursesInstructAndWait - synchronization error");
   return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_transform_line(chtype *ch, int row, int start_col, int num_cols)
#else
int XCurses_transform_line(ch, row, start_col, num_cols)
chtype *ch;
int row,start_col,num_cols;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_transform_line() called: row %d start_col %d num_cols %d flag %d\n",
                        (XCursesProcess)?"X":"CURSES",row,start_col,num_cols,*(Xcurscr+XCURSCR_FLAG_OFF+row));
#endif

   while(*(Xcurscr+XCURSCR_FLAG_OFF+row))
   /*
    * Patch by:
    * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 02-Feb-1999
    */
   dummy_function(); /* loop until we can write to the line */
   /*
    * End of patch by:
    * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 02-Feb-1999
    */

   *(Xcurscr+XCURSCR_FLAG_OFF+row) = 1;
   memcpy(Xcurscr+XCURSCR_Y_OFF(row)+(start_col*sizeof(chtype)),ch,num_cols*sizeof(chtype));
#if 0
   *((int*)(Xcurscr+XCURSCR_START_OFF+row)) = start_col;
   *((int*)(Xcurscr+XCURSCR_LENGTH_OFF+row)) = num_cols;
   if ( (int)*(Xcurscr+XCURSCR_START_OFF+row) != start_col )
      printf("%d difference: start_col %d mem: %d\n",__LINE__,start_col,(int)*(Xcurscr+XCURSCR_START_OFF+row) );
   if ( (int)*(Xcurscr+XCURSCR_LENGTH_OFF+row) != num_cols )
      printf("%d difference: num_cols %d mem: %d\n",__LINE__,num_cols,(int)*(Xcurscr+XCURSCR_LENGTH_OFF+row) );
#else
   *(Xcurscr+XCURSCR_START_OFF+row) = start_col;
   *(Xcurscr+XCURSCR_LENGTH_OFF+row) = num_cols;
#endif
   *(Xcurscr+XCURSCR_FLAG_OFF+row) = 0;
   return(0);
}
/***********************************************************************/
#ifdef HAVE_PROTO
static int XCursesSetupCurses(void)
#else
static int XCursesSetupCurses()
#endif
/***********************************************************************/
{
   char wait_buf[5];
   int wait_value;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesSetupCurses called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   close ( display_sockets[1] );
   close ( key_sockets[1] );
   display_sock = display_sockets[0];
   key_sock = key_sockets[0];
   FD_ZERO ( &readfds );
   FD_ZERO ( &writefds );
   read_socket(display_sock,wait_buf,sizeof(int));
   memcpy((char *)&wait_value,wait_buf,sizeof(int));
   if (wait_value != CURSES_CHILD)
      return(ERR);
   /*
    * Set LINES and COLS now so that the size of the
    * shared memory segment can be allocated
    */
   if ((shmidSP = shmget(shmkeySP,sizeof(SCREEN)+XCURSESSHMMIN,0700)) < 0)
   {
      perror("Cannot allocate shared memory for SCREEN");
      kill(otherpid,SIGKILL);
      return(ERR);
   }
   SP = (SCREEN*)shmat(shmidSP,0,0);
   XCursesLINES = SP->lines;
   LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;
   XCursesCOLS = COLS = SP->cols;
   if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,SP->XcurscrSize+XCURSESSHMMIN,0700)) < 0)
   {
      perror("Cannot allocate shared memory for curscr");
      kill(otherpid,SIGKILL);
      return(ERR);
   }
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:shmid_Xcurscr %d shmkey_Xcurscr %d LINES %d COLS %d\n",(XCursesProcess)?"     X":"CURSES",shmid_Xcurscr,shmkey_Xcurscr,LINES,COLS);
#endif
   Xcurscr = (unsigned char *)shmat(shmid_Xcurscr,0,0);
   atrtab = (unsigned char *)(Xcurscr+XCURSCR_ATRTAB_OFF);
   PDC_init_atrtab();
#ifdef PDCDEBUG
   say ("cursesprocess exiting from Xinitscr\n");
#endif
   /*
    * Always trap SIGWINCH if the C library supports SIGWINCH...
    */
   XCursesSetSignal(SIGWINCH, XCursesSigwinchHandler );
   return(OK);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesInitscr(char *display_name, int argc, char *argv[])
#else
int XCursesInitscr(display_name,argc,argv)
char *display_name;
int argc;
char *argv[];
#endif
/***********************************************************************/
{
   int pid,rc;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesInitscr() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif

#if defined FOREIGN
   if (setlocale(LC_ALL, "") == NULL)
   {
      fprintf(stderr, "ERROR: cannot set locale\n");
      return(ERR);
   }
   if (!XSupportsLocale())
   {
      fprintf(stderr, "ERROR: X does not support locale\n");
      return(ERR);
   }
   if (XSetLocaleModifiers("") == NULL)
      fprintf(stderr, "WARNING: Cannot set locale modifiers\n");
#endif

   shmkeySP = getpid();
   memset ( &socket_timeout, '\0', sizeof ( socket_timeout ) );
            
   if ( socketpair ( AF_UNIX, SOCK_STREAM, 0, display_sockets ) < 0 )
   {
      fprintf(stderr, "ERROR: cannot create display socketpair\n");
      return(ERR);
   }
            
   if ( socketpair ( AF_UNIX, SOCK_STREAM, 0, key_sockets ) < 0 )
   {
      fprintf(stderr, "ERROR: cannot create key socketpair\n");
      return(ERR);
   }

   pid = fork();
   switch(pid)
   {
      case (-1):
         fprintf(stderr,"ERROR: cannot fork()\n");
         return(ERR);
         break;
      case 0: /* child */
         shmkey_Xcurscr = getpid();
#ifdef XISPARENT
         XCursesProcess=0;
         rc = XCursesSetupCurses();
#else
         XCursesProcess=1;
         otherpid = getppid();
         rc = XCursesSetupX(display_name,argc,argv);
#endif
         break;
      default: /* parent */
         shmkey_Xcurscr = pid;
#ifdef XISPARENT
         XCursesProcess=1;
         otherpid = pid;
         rc = XCursesSetupX(display_name,argc,argv);
#else
         XCursesProcess=0;
         rc = XCursesSetupCurses();
#endif
         break;
   }
   return(rc);
}

/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_getclipboard( char **contents, long *length )
#else
int XCurses_getclipboard( contents, length )
char **contents;
long *length;
#endif
/***********************************************************************/
{
   int result=0;
   int len;
   char buf[12];

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_getclipboard() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   XCursesInstructAndWait(CURSES_GET_SELECTION);

   if (read_socket(display_sock,buf,sizeof(int)) < 0)
      XCursesExitCursesProcess(5,"exiting from XCurses_getclipboard");
   memcpy((char *)&result,buf,sizeof(int));
   if (result == PDC_CLIP_SUCCESS)
   {
      if (read_socket(display_sock,buf,sizeof(int)) < 0)
         XCursesExitCursesProcess(5,"exiting from XCurses_getclipboard");
      memcpy((char *)&len,buf,sizeof(int));
      if (len != 0)
      {
         *contents = (char *)malloc(len+1);
         if (!*contents)
            XCursesExitCursesProcess(6,"exitting from XCurses_getclipboard - synchronization error");
         if (read_socket(display_sock,*contents,len) < 0)
            XCursesExitCursesProcess(5,"exiting from XCurses_getclipboard");
         *length = len;
      }
   }
   return result;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_setclipboard( char *contents, long length )
#else
int XCurses_setclipboard( contents, length )
char *contents;
long length;
#endif
/***********************************************************************/
{
   int rc=0;
   char buf[12]; /* big enough for 2 integers */
   long len=length;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_setclipboard() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   XCursesInstruct(CURSES_SET_SELECTION);
   memcpy(buf,(char *)&len,sizeof(long));
   if (write_socket(display_sock,buf,sizeof(long)) < 0)
      XCursesExitCursesProcess(5,"exiting from XCurses_setclipboard");
   if (write_socket(display_sock,contents,length) < 0)
      XCursesExitCursesProcess(5,"exiting from XCurses_setclipboard");
   /*
    * Wait for X to do its stuff. Now expect return code...
    */
   if (read_socket(display_sock,buf,sizeof(int)) < 0)
      XCursesExitCursesProcess(5,"exiting from XCurses_setclipboard");
   memcpy((char *)&rc,buf,sizeof(int));
   return rc;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int XCurses_clearclipboard( void )
#else
int XCurses_clearclipboard( )
#endif
/***********************************************************************/
{
   int rc=0;
   char buf[12]; /* big enough for 2 integers */
   long len;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCurses_clearclipboard() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   XCursesInstruct(CURSES_CLEAR_SELECTION);
   memcpy(buf,(char *)&len,sizeof(long));
   if (write_socket(display_sock,buf,sizeof(long)) < 0)
      XCursesExitCursesProcess(5,"exiting from XCurses_setclipboard");
   /*
    * Wait for X to do its stuff. Now expect return code...
    */
   if (read_socket(display_sock,buf,sizeof(int)) < 0)
      XCursesExitCursesProcess(5,"exiting from XCurses_clearclipboard");
   memcpy((char *)&rc,buf,sizeof(int));
   return rc;
}

/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesCleanupCursesProcess(int rc)
#else
void XCursesCleanupCursesProcess(rc)
int rc;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesCleanupCursesProcess() - called: %d\n",(XCursesProcess)?"     X":"CURSES",rc);
#endif
   shutdown(display_sock,2);
   close(display_sock);
   shutdown(key_sock,2);
   close(key_sock);
   shmdt((char *)SP);
   shmdt((char *)Xcurscr);
   if (rc)
      _exit(rc);
   else
      return;
}
/***********************************************************************/
#ifdef HAVE_PROTO
static void XCursesExitCursesProcess(int rc,char *msg)
#else
static void XCursesExitCursesProcess(rc,msg)
int rc;
char *msg;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesExitCursesProcess() - called: %d %s\n",(XCursesProcess)?"     X":"CURSES",rc,msg);
#endif
   endwin();
   XCursesCleanupCursesProcess(rc);
   return;
}
/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesExit(void)
#else
void XCursesExit()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesExit() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif
   XCursesInstruct(CURSES_EXIT);
   XCursesCleanupCursesProcess(0);
   return;
}
