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
* 1994,1995.
***************************************************************************
*/

#include "pdcx11.h"

   static int visible_cursor=0;
   int windowEntered = 1;

/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesExitXCursesProcess(int rc,int sig,char *msg)
#else
void XCursesExitXCursesProcess(rc,sig,msg)
int rc,sig;
char *msg;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesExitXCursesProcess() - called: rc:%d sig:%d %s\n",(XCursesProcess)?"     X":"CURSES",rc,sig,msg);
#endif
 shmdt((char *)SP);
 shmdt((char *)Xcurscr);
 shmctl(shmidSP,IPC_RMID,0);
 shmctl(shmid_Xcurscr,IPC_RMID,0);
 XCursesEndwin();
 shutdown(display_sock,2);
 close(display_sock);
 shutdown(exit_sock,2);
 close(exit_sock);
 shutdown(key_sock,2);
 close(key_sock);
 if (sig)
    kill(otherpid,sig); /* to kill parent process */
 _exit(rc);
}
/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesDisplayScreen(bool highlight)
#else
void XCursesDisplayScreen(highlight)
bool highlight;
#endif
/***********************************************************************/
/*---------------------------------------------------------------------*/
/* This function re-draws the entire screen.                           */
/*---------------------------------------------------------------------*/
{
 int row=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesDisplayScreen() - called:\n",(XCursesProcess)?"     X":"CURSES");
#endif
 for (row=0;row<XCursesLINES;row++)
   {
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
    XCursesDisplayText((chtype *)(Xcurscr+XCURSCR_Y_OFF(row)),row,0,COLS,highlight);
    *(Xcurscr+XCURSCR_FLAG_OFF+row) = 0;
   }
 XCursesDisplayCursor(SP->cursrow,SP->curscol,
                      SP->cursrow,SP->curscol);
 /*
  * Draw the border if required
  */
 if (XCURSESBORDERWIDTH)
    XDrawRectangle(XCURSESDISPLAY,XCURSESWIN,border_gc,
                  (XCURSESBORDERWIDTH/2),(XCURSESBORDERWIDTH/2),
                  (XCursesWindowWidth-XCURSESBORDERWIDTH),
                  (XCursesWindowHeight-XCURSESBORDERWIDTH));
 return;
}

/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesRefreshScreen(void)
#else
int XCursesRefreshScreen()
#endif
/***********************************************************************/
/*---------------------------------------------------------------------*/
/* This function draws those portions of the screen that have changed. */
/*---------------------------------------------------------------------*/
{
 int row=0,start_col=0,num_cols=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesRefreshScreen() - called:\n",(XCursesProcess)?"     X":"CURSES");
#endif
 for (row=0;row<XCursesLINES;row++)
   {
#if 0
    num_cols = (int)*((int*)(Xcurscr+XCURSCR_LENGTH_OFF+row));
printf("Num cols: %d\n",num_cols);
#else
    num_cols = (int)*(Xcurscr+XCURSCR_LENGTH_OFF+row);
#endif
    if (num_cols != 0)
      {
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
#if 0
       start_col = (int)*((int*)(Xcurscr+XCURSCR_START_OFF+row));
printf("start_col: %d\n",start_col);
#else
       start_col = (int)*(Xcurscr+XCURSCR_START_OFF+row);
#endif
       XCursesDisplayText((chtype *)(Xcurscr+XCURSCR_Y_OFF(row)+(start_col*sizeof(chtype))),row,start_col,num_cols,FALSE);
#if 0
       *((int*)(Xcurscr+XCURSCR_LENGTH_OFF+row)) = 0;
 if ( (int)*(Xcurscr+XCURSCR_LENGTH_OFF+row) != 0 )
    printf("%d difference: num_cols %d mem: %d\n",__LINE__,0,(int)*(Xcurscr+XCURSCR_LENGTH_OFF+row) );
#else
       *(Xcurscr+XCURSCR_LENGTH_OFF+row) = 0;
#endif
       *(Xcurscr+XCURSCR_FLAG_OFF+row) = 0;
      }
   }
 if (mouse_selection)
    SelectionOff();
 XCursesDisplayCursor(SP->cursrow,SP->curscol,
                      SP->cursrow,SP->curscol);
 return(0);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesSendKeyToCurses(unsigned long key)
#else
int XCursesSendKeyToCurses(key)
unsigned long key;
#endif
/***********************************************************************/
{
 char buf[10];

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesSendKeyToCurses() - called: sending %d\n",(XCursesProcess)?"     X":"CURSES",key);
#endif
 memcpy(buf,(char *)&key,sizeof(unsigned long));
#ifdef MOUSE_DEBUG1
 printf("%s:writing %d\n",(XCursesProcess)?"     X":"CURSES",key);
#endif
 if (write_socket(key_sock,buf,sizeof(int)) < 0)
    XCursesExitXCursesProcess(1,SIGKILL,"exiting from XCursesSendKeyToCurses");
 return(0);
}
/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesProcessRequestsFromCurses(XtPointer client_data,int *fid,XtInputId *id)
#else
void XCursesProcessRequestsFromCurses(client_data,fid,id)
XtPointer client_data;
int *fid;
XtInputId *id;
#endif
/***********************************************************************/
{
 int s,idx;
 int old_row,new_row;
 int old_x,new_x;
 int pos,num_cols;
 long length;
 char buf[12]; /* big enough for 2 integers */
 char title[1024]; /* big enough for window title */
 unsigned char save_atrtab[MAX_ATRTAB];

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesProcessRequestsFromCurses()\n",(XCursesProcess)?"     X":"CURSES");
#endif

 if (!ReceivedMapNotify)
    return;
 FD_ZERO ( &readfds );
 FD_SET ( display_sock, &readfds );

 if ( ( s = select ( FD_SETSIZE, (FD_SET_CAST)&readfds, NULL, NULL, &socket_timeout ) ) < 0 )
    XCursesExitXCursesProcess(2,SIGKILL,"exiting from XCursesProcessRequestsFromCurses - select failed");
            
 if ( s == 0 ) /* no requests pending - should never happen!*/
    return;
            
 if ( FD_ISSET ( display_sock, &readfds ) )
   {
/* read first integer to determine total message has been received */
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesProcessRequestsFromCurses() - before read_socket()\n",(XCursesProcess)?"     X":"CURSES");
#endif
    if (read_socket(display_sock,buf,sizeof(int)) < 0)
       XCursesExitXCursesProcess(3,SIGKILL,"exiting from XCursesProcessRequestsFromCurses - first read");
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesProcessRequestsFromCurses() - after read_socket()\n",(XCursesProcess)?"     X":"CURSES");
#endif
    memcpy((char *)&num_cols,buf,sizeof(int));
    after_first_curses_request = True;

    switch(num_cols)
      {
       case 0: break;
       case CURSES_EXIT: /* request from curses to stop */
            say("CURSES_EXIT received from child\n");
            XCursesExitXCursesProcess(0,0,"XCursesProcess requested to exit by child");
            break;
       case CURSES_BELL: /* request from curses to beep */
            say("CURSES_BELL received from child\n");
            XBell(XCURSESDISPLAY,50);
            break;
       case CURSES_CLEAR: /* request from curses to clear window */
            say("CURSES_CLEAR received from child\n");
            XClearWindow(XCURSESDISPLAY,XCURSESWIN);
            break;
       case CURSES_FLASH: /* request from curses to beep */
            say("CURSES_FLASH received from child\n");
#if 0
            XFillRectangle(XCURSESDISPLAY,XCURSESWIN,normal_highlight_gc,10,10,XCursesWindowWidth-10,XCursesWindowHeight-10);
            delay_output(50);
            XFillRectangle(XCURSESDISPLAY,XCURSESWIN,normal_highlight_gc,10,10,XCursesWindowWidth-10,XCursesWindowHeight-10);
            XCursesDisplayCursor(SP->cursrow,SP->curscol,
                                 SP->cursrow,SP->curscol);
#endif
            old_x = CURSES_CONTINUE;
            memcpy(buf,(char *)&old_x,sizeof(int));
            if (write_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
            break;
       case CURSES_REFRESH: /* request from curses to confirm completion of display */
            say("CURSES_REFRESH received from child\n");
            visible_cursor = 1;
            XCursesRefreshScreen();
            XCursesDisplayCursor(SP->cursrow,SP->curscol,
                                 SP->cursrow,SP->curscol);
            old_x = CURSES_CONTINUE;
            memcpy(buf,(char *)&old_x,sizeof(int));
            if (write_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
            break;
       case CURSES_REFRESH_SCROLLBAR: /* request from curses draw scrollbar */
            XCursesRefreshScrollbar();
#if 0
            old_x = CURSES_CONTINUE;
            memcpy(buf,(char *)&old_x,sizeof(int));
            if (write_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
#endif
            break;
       case CURSES_CURSOR: /* display cursor */
            say("CURSES_CURSOR received from child\n");
            if (read_socket(display_sock,buf,sizeof(int)*2) < 0)
               XCursesExitXCursesProcess(5,SIGKILL,"exiting from CURSES_CURSOR XCursesProcessRequestsFromCurses");
            memcpy((char *)&pos,buf,sizeof(int));
            old_row = pos & 0xFF;
            old_x = pos >> 8;
            idx = sizeof(int);
            memcpy((char *)&pos,buf+idx,sizeof(int));
            new_row = pos & 0xFF;
            new_x = pos >> 8;
            visible_cursor = 1;
            XCursesDisplayCursor(old_row,old_x,new_row,new_x);
            break;
       case CURSES_DISPLAY_CURSOR: /* display cursor */
            say("CURSES_DISPLAY_CURSOR received from child. Vis now: %d\n",visible_cursor);
            /*
             * If the window is not active, ignore this command. The
             * cursor will stay solid.
             */
            if ( windowEntered )
            {
               if ( visible_cursor )
               {
                  /*
                   * Cursor currently ON, turn it off
                   */
                  int save_visibility = SP->visibility;
                  SP->visibility = 0;
                  XCursesDisplayCursor(SP->cursrow,SP->curscol,
                                       SP->cursrow,SP->curscol);
                  SP->visibility = save_visibility;
                  visible_cursor = 0;
               }
               else
               {
                  /*
                   * Cursor currently OFF, turn it on
                   */
                  XCursesDisplayCursor(SP->cursrow,SP->curscol,
                                       SP->cursrow,SP->curscol);
                  visible_cursor = 1;
               }
            }
            break;
       case CURSES_TITLE: /* display window title */
            say("CURSES_TITLE received from child\n");
            if (read_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(5,SIGKILL,"exiting from CURSES_TITLE XCursesProcessRequestsFromCurses");
            memcpy((char *)&pos,buf,sizeof(int));
            if (read_socket(display_sock,title,pos) < 0)
               XCursesExitXCursesProcess(5,SIGKILL,"exiting from CURSES_TITLE XCursesProcessRequestsFromCurses");
            XtVaSetValues(topLevel, XtNtitle, title, NULL);
            break;
       case CURSES_RESIZE: /* resize window */
            after_first_curses_request = False;
            say("CURSES_RESIZE received from child\n");
            SP->lines = XCursesLINES = ((resizeXCursesWindowHeight-(2*XCURSESBORDERWIDTH)) / XCursesFontHeight);
            LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;
            SP->cols =  COLS  = XCursesCOLS = ((resizeXCursesWindowWidth-(2*XCURSESBORDERWIDTH)) / XCursesFontWidth);
            XCursesWindowWidth = resizeXCursesWindowWidth;
            XCursesWindowHeight = resizeXCursesWindowHeight;
            visible_cursor = 1;
            /*
             * Draw the border if required
             */
            if (XCURSESBORDERWIDTH)
              XDrawRectangle(XCURSESDISPLAY,XCURSESWIN,border_gc,
                            (XCURSESBORDERWIDTH/2),(XCURSESBORDERWIDTH/2),
                            (XCursesWindowWidth-XCURSESBORDERWIDTH),
                            (XCursesWindowHeight-XCURSESBORDERWIDTH));
/*
 * detach and drop the current shared memory segment and create and attach
 * to a new segment.
 */
            memcpy(save_atrtab,atrtab,sizeof(save_atrtab));
            SP->XcurscrSize = XCURSCR_SIZE;
            shmdt((char *)Xcurscr);
            shmctl(shmid_Xcurscr,IPC_RMID,0);
            if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,SP->XcurscrSize+XCURSESSHMMIN,0700|IPC_CREAT)) < 0)
              {
               perror("Cannot allocate shared memory for curscr");
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
              }
            Xcurscr = (unsigned char*)shmat(shmid_Xcurscr,0,0);
            memset(Xcurscr, 0, SP->XcurscrSize); 
            atrtab = (unsigned char *)(Xcurscr+XCURSCR_ATRTAB_OFF);
            memcpy(atrtab,save_atrtab,sizeof(save_atrtab));

            old_x = CURSES_CONTINUE;
            memcpy(buf,(char *)&old_x,sizeof(int));
            if (write_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
            break;
       case CURSES_GET_SELECTION: /* request selection contents */
            say("CURSES_GET_SELECTION received from child\n");
            old_x = CURSES_CONTINUE;
            memcpy(buf,(char *)&old_x,sizeof(int));
            if (write_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
            XtGetSelectionValue(topLevel,XA_PRIMARY,XA_STRING,XCursesRequestorCallbackForGetSelection,(XtPointer)NULL,0);
            break;
       case CURSES_SET_SELECTION: /* set the selection contents */
            say("CURSES_SET_SELECTION received from child\n");
            if (read_socket(display_sock,buf,sizeof(long)) < 0)
               XCursesExitXCursesProcess(5,SIGKILL,"exiting from CURSES_TITLE XCursesProcessRequestsFromCurses");
            memcpy((char *)&length,buf,sizeof(long));
            if (length > tmpsel_length)
            {
               if (tmpsel_length == 0)
                  tmpsel = (char *)malloc(length+1);
               else
                  tmpsel = (char *)realloc(tmpsel,length+1);
            }
            if (!tmpsel)
            {
               old_x = PDC_CLIP_MEMORY_ERROR;
               memcpy(buf,(char *)&old_x,sizeof(int));
               if (write_socket(display_sock,buf,sizeof(int)) < 0)
                  XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
               break;
            }
            if (read_socket(display_sock,tmpsel,length) < 0)
               XCursesExitXCursesProcess(5,SIGKILL,"exiting from CURSES_TITLE XCursesProcessRequestsFromCurses");
            tmpsel_length = length;
            *(tmpsel+length) = '\0';
            if (XtOwnSelection(topLevel,
                               XA_PRIMARY,
                               CurrentTime,
                               XCursesConvertProc,
                               XCursesLoseOwnership,
                               NULL) == False)
            {
               old_x = PDC_CLIP_ACCESS_ERROR;
               free(tmpsel);
               tmpsel = NULL;
               tmpsel_length = 0;
            }
            else
               old_x = PDC_CLIP_SUCCESS;
            SelectionOff();
            memcpy(buf,(char *)&old_x,sizeof(int));
            if (write_socket(display_sock,buf,sizeof(int)) < 0)
               XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesProcessRequestsFromCurses");
            break;
       default:
#ifdef PDCDEBUG
            if (trace_on) PDC_debug("%s:Unknown request %d\n",(XCursesProcess)?"     X":"CURSES",num_cols);
#endif
            break;
      }
   }
 return;
}
/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesButton(Widget w,XEvent *event, String *params, Cardinal *nparams)
#else
void XCursesButton(w,event,params,nparams)
Widget w;
XEvent *event;
String *params;
Cardinal *nparams;
#endif
/***********************************************************************/
{
   int button_no=0;
   static int last_button_no;
   static Time last_button_press_time=0;
   MOUSE_STATUS save_mouse_status;
   bool send_key=TRUE;
   char buf[100]; /* for MOUSE_STATUS struct */
   static bool remove_release;
   static bool handle_real_release;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesButton called\n",(XCursesProcess)?"     X":"CURSES");
#endif

   save_mouse_status=Mouse_status;
   button_no = event->xbutton.button;
   /*---------------------------------------------------------------------*/
   /* It appears that under X11R6 (at least on Linux), that an event_type */
   /* of ButtonMotion does not include the mouse button in the event. The */
   /* following code is designed to cater for this situation.             */
   /*---------------------------------------------------------------------*/
   if (button_no == 0)
      button_no = last_button_no;
   last_button_no = button_no;

   Mouse_status.changes = 0;
   switch(event->type)
   {
      /*
       * ButtonPress
       */
      case ButtonPress:
         remove_release = False;
         handle_real_release = False;
#ifdef MOUSE_DEBUG
         printf("\nButtonPress\n");
#endif
         if ((event->xbutton.time - last_button_press_time) < XCURSESDOUBLECLICKPERIOD)
         {
            MOUSE_X_POS = save_mouse_status.x;
            MOUSE_Y_POS = save_mouse_status.y;
            BUTTON_STATUS(button_no) = BUTTON_DOUBLE_CLICKED;
            SelectionOff();
            if (!(SP->_trap_mbe & BUTTON1_DOUBLE_CLICKED)
            &&  button_no == 1)
               send_key = FALSE;
            if (!(SP->_trap_mbe & BUTTON2_DOUBLE_CLICKED)
            &&  button_no == 2)
               send_key = FALSE;
            if (!(SP->_trap_mbe & BUTTON3_DOUBLE_CLICKED)
            &&  button_no == 3)
               send_key = FALSE;
            if (send_key)
               remove_release = True;
         }
         else
         {
#if 0
            MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
            MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
            BUTTON_STATUS(button_no) = BUTTON_PRESSED;
            if (!(SP->_trap_mbe & BUTTON1_PRESSED)
            &&  button_no == 1)
               send_key = FALSE;
            if (!(SP->_trap_mbe & BUTTON2_PRESSED)
            &&  button_no == 2)
               send_key = FALSE;
            if (!(SP->_trap_mbe & BUTTON3_PRESSED)
            &&  button_no == 3)
               send_key = FALSE;
            if (button_no == 1
            &&  !(event->xbutton.state & ShiftMask)
            &&  !(event->xbutton.state & ControlMask)
            &&  !(event->xbutton.state & Mod1Mask))
            {
               SelectionOff();
               SelectionOn(MOUSE_X_POS,MOUSE_Y_POS);
            }
#else
            PDC_usleep( XCURSESCLICKPERIOD * 1000 ); /* microseconds to milliseconds */
            event->type = ButtonRelease;
            XSendEvent(event->xbutton.display, event->xbutton.window, True,
                       0, event);
            last_button_press_time = event->xbutton.time;
#ifdef PDCDEBUG
printf("Pressed at: %ld\n",last_button_press_time);
#endif
            return;
#endif
         }
         last_button_press_time = event->xbutton.time;
#ifdef PDCDEBUG
printf("Pressed at: %ld\n",last_button_press_time);
#endif
         break;
      /*
       * MotionNotify
       */
      case MotionNotify:
#ifdef MOUSE_DEBUG
         printf("\nMotionNotify: y: %d x: %d Width: %d Height: %d\n",event->xbutton.y,
                event->xbutton.x,
                XCursesFontWidth,
                XCursesFontHeight);
#endif
         if (button_no == 1
         &&  !(event->xbutton.state & ShiftMask)
         &&  !(event->xbutton.state & ControlMask)
         &&  !(event->xbutton.state & Mod1Mask))
         {
            MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
            MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
            SelectionExtend(MOUSE_X_POS,MOUSE_Y_POS);
            send_key = FALSE;
         }
         else
            SelectionOff();
         /*
          * Throw away mouse movements if they are in the same character position
          * as the last mouse event, or if we are currently in the middle of
          * a double click event.
          */
         MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
         MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
         if ((MOUSE_X_POS == save_mouse_status.x
           &&  MOUSE_Y_POS == save_mouse_status.y)
         ||  save_mouse_status.button[button_no-1] == BUTTON_DOUBLE_CLICKED)
         {
            send_key = FALSE;
            break;
         }
         MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
         MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
         Mouse_status.changes |= 8;
         if (!(SP->_trap_mbe & BUTTON1_MOVED)
         &&  button_no == 1)
            send_key = FALSE;
         if (!(SP->_trap_mbe & BUTTON2_MOVED)
         &&  button_no == 2)
            send_key = FALSE;
         if (!(SP->_trap_mbe & BUTTON3_MOVED)
         &&  button_no == 3)
            send_key = FALSE;
#if 0
         if (button_no == 1
         &&  !(event->xbutton.state & ShiftMask)
         &&  !(event->xbutton.state & ControlMask)
         &&  !(event->xbutton.state & Mod1Mask))
         {
            SelectionExtend(MOUSE_X_POS,MOUSE_Y_POS);
            send_key = FALSE;
         }
         else
            SelectionOff();
#endif
         break;
      /*
       * ButtonRelease
       */
      case ButtonRelease:
#if 0
         MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
         MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
         BUTTON_STATUS(button_no) = BUTTON_RELEASED;
         if (!(SP->_trap_mbe & BUTTON1_RELEASED)
         &&  button_no == 1)
            send_key = FALSE;
         if (!(SP->_trap_mbe & BUTTON2_RELEASED)
         &&  button_no == 2)
            send_key = FALSE;
         if (!(SP->_trap_mbe & BUTTON3_RELEASED)
         &&  button_no == 3)
            send_key = FALSE;
         if (button_no == 1
         &&  !(event->xbutton.state & ShiftMask)
         &&  !(event->xbutton.state & ControlMask)
         &&  !(event->xbutton.state & Mod1Mask)
         &&  mouse_selection)
         {
            send_key = FALSE;
            if (XtOwnSelection(topLevel,
                               XA_PRIMARY,
                               event->xbutton.time,
                               XCursesConvertProc,
                               XCursesLoseOwnership,
                               NULL) == False)
            {
               SelectionOff();
            }
            SelectionSet();
         }
         else
            SelectionOff();
         break;
#else
         if (remove_release)
         {
#ifdef PDCDEBUG
   printf("Release at: %ld - removed\n",event->xbutton.time);
#endif
            return;
         }
         else
         {
            if (!handle_real_release)
            {
               if ((event->xbutton.time - last_button_press_time) < 100
               && (event->xbutton.time != last_button_press_time))
               {
                  /*
                   * The "real" release was shorter than usleep() time
                   * therefore generate a click event
                   */
#ifdef PDCDEBUG
   printf("Release at: %ld - click\n",event->xbutton.time);
#endif
                  MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
                  MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
                  BUTTON_STATUS(button_no) = BUTTON_CLICKED;
                  if (!(SP->_trap_mbe & BUTTON1_RELEASED)
                  &&  button_no == 1)
                     send_key = FALSE;
                  if (!(SP->_trap_mbe & BUTTON2_RELEASED)
                  &&  button_no == 2)
                     send_key = FALSE;
                  if (!(SP->_trap_mbe & BUTTON3_RELEASED)
                  &&  button_no == 3)
                     send_key = FALSE;
                  if (button_no == 1
                  &&  !(event->xbutton.state & ShiftMask)
                  &&  !(event->xbutton.state & ControlMask)
                  &&  !(event->xbutton.state & Mod1Mask)
                  &&  mouse_selection)
                  {
                     send_key = FALSE;
                     if (XtOwnSelection(topLevel,
                                  XA_PRIMARY,
                                  event->xbutton.time,
                                  XCursesConvertProc,
                                  XCursesLoseOwnership,
                                  NULL) == False)
                     {
                        SelectionOff();
                     }
                  }
                  else
                     SelectionOff();
                  /*
                   * Ensure the "pseudo" release event is ignored
                   */
                  remove_release = True;
                  handle_real_release = False;
                  break;
               }
               else
               {
                  /*
                   * Button release longer than usleep() time
                   * therefore generate a press and wait for the
                   * real release to occur later.
                   */
#ifdef PDCDEBUG
   printf("Generated Release at: %ld - press & release\n",event->xbutton.time);
#endif
                  MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
                  MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
                  BUTTON_STATUS(button_no) = BUTTON_PRESSED;
                  if (!(SP->_trap_mbe & BUTTON1_PRESSED)
                  &&  button_no == 1)
                     send_key = FALSE;
                  if (!(SP->_trap_mbe & BUTTON2_PRESSED)
                  &&  button_no == 2)
                     send_key = FALSE;
                  if (!(SP->_trap_mbe & BUTTON3_PRESSED)
                  &&  button_no == 3)
                     send_key = FALSE;
                  if (button_no == 1
                  &&  !(event->xbutton.state & ShiftMask)
                  &&  !(event->xbutton.state & ControlMask)
                  &&  !(event->xbutton.state & Mod1Mask))
                  {
                     SelectionOff();
                     SelectionOn(MOUSE_X_POS,MOUSE_Y_POS);
                  }
                  handle_real_release = True;
                  break;
               }
            }
            else
            {
#ifdef PDCDEBUG
   printf("Release at: %ld - released\n",event->xbutton.time);
#endif
            }
         }
#ifdef PDCDEBUG
printf("\nButtonRelease\n");
#endif
         MOUSE_X_POS = (event->xbutton.x-XCURSESBORDERWIDTH) / XCursesFontWidth;
         MOUSE_Y_POS = (event->xbutton.y-XCURSESBORDERWIDTH) / XCursesFontHeight;
         BUTTON_STATUS(button_no) = BUTTON_RELEASED;
         if (!(SP->_trap_mbe & BUTTON1_RELEASED)
         &&  button_no == 1)
            send_key = FALSE;
         if (!(SP->_trap_mbe & BUTTON2_RELEASED)
         &&  button_no == 2)
            send_key = FALSE;
         if (!(SP->_trap_mbe & BUTTON3_RELEASED)
         &&  button_no == 3)
            send_key = FALSE;
         if (button_no == 1
         &&  !(event->xbutton.state & ShiftMask)
         &&  !(event->xbutton.state & ControlMask)
         &&  !(event->xbutton.state & Mod1Mask)
         &&  mouse_selection)
         {
            send_key = FALSE;
            if (XtOwnSelection(topLevel,
                               XA_PRIMARY,
                               event->xbutton.time,
                               XCursesConvertProc,
                               XCursesLoseOwnership,
                               NULL) == False)
            {
               SelectionOff();
            }
            SelectionSet();
         }
         else
            SelectionOff();
         break;
#endif
   }
   /*
    *---------------------------------------------------------------------
    * Set up the mouse status fields in preparation for sending...
    *---------------------------------------------------------------------
    */
   Mouse_status.changes |= (1 << (button_no-1));
   if (event->xbutton.state & ShiftMask)
      BUTTON_STATUS(button_no) |= BUTTON_SHIFT;
   if (event->xbutton.state & ControlMask)
      BUTTON_STATUS(button_no) |= BUTTON_CONTROL;
   if (event->xbutton.state & Mod1Mask)
      BUTTON_STATUS(button_no) |= BUTTON_ALT;
   /*
    *---------------------------------------------------------------------
    * If we are ignoring the event, or the mouse position is outside the
    * bounds of the screen (because of the border), return here...
    *---------------------------------------------------------------------
    */
#ifdef PDCDEBUG
   printf("Button: %d x: %d y: %d Button status: %x Mouse status: %x\n",button_no,
                                    MOUSE_X_POS,
                                    MOUSE_Y_POS,
                                    BUTTON_STATUS(button_no),
                                    Mouse_status.changes);
   printf("Send: %d Button1: %x Button2: %x Button3: %x %d %d\n",
                                    send_key,
                                    BUTTON_STATUS(1),
                                    BUTTON_STATUS(2),
                                    BUTTON_STATUS(3),
                                    XCursesLINES,XCursesCOLS);
#endif

   if (!send_key
   ||  MOUSE_X_POS < 0
   ||  MOUSE_X_POS >= XCursesCOLS
   ||  MOUSE_Y_POS < 0
   ||  MOUSE_Y_POS >= XCursesLINES)
      return;

#ifdef MOUSE_DEBUG
   printf("Button: %d x: %d y: %d Button status: %x Mouse status: %x\n",button_no,
                                    MOUSE_X_POS,
                                    MOUSE_Y_POS,
                                    BUTTON_STATUS(button_no),
                                    Mouse_status.changes);
   printf("Send: %d Button1: %x Button2: %x Button3: %x\n",send_key,
                                    BUTTON_STATUS(1),
                                    BUTTON_STATUS(2),
                                    BUTTON_STATUS(3));
#endif
/*---------------------------------------------------------------------*/
/* Send the KEY_MOUSE to curses program...                             */
/*---------------------------------------------------------------------*/
   XCursesSendKeyToCurses((unsigned long)KEY_MOUSE);
   memcpy(buf,(char *)&Mouse_status,sizeof(MOUSE_STATUS));
#ifdef MOUSE_DEBUG1
   printf("%s:writing mouse stuff\n",(XCursesProcess)?"     X":"CURSES");
#endif
   (void)write_socket(key_sock,buf,sizeof(MOUSE_STATUS));
   return;
}

/***********************************************************************/
#ifdef HAVE_PROTO
static void Scroll_up_down(Widget w, XtPointer client_data, XtPointer call_data)
#else
static void Scroll_up_down(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#endif
/***********************************************************************/
{
 int pixels = (int) call_data;
 int total_y=(SP->sb_total_y*XCursesFontHeight);
 int viewport_y=(SP->sb_viewport_y*XCursesFontHeight);
 int cur_y=(SP->sb_cur_y*XCursesFontHeight);

 /* When pixels is negative, right button pressed, move data down,
  * thumb moves up.  Otherwise, left button pressed, pixels 
  * positive, move data up, thumb down. 
  */
 cur_y += pixels;

 /* limit panning to size of overall */
 if (cur_y < 0)
    cur_y = 0;
 else 
   {
    if (cur_y > (total_y - viewport_y))
       cur_y = (total_y - viewport_y);
   }
 SP->sb_cur_y = cur_y / XCursesFontHeight;
 XawScrollbarSetThumb(w,(float)((float)cur_y/(float)total_y),(float)((float)viewport_y/(float)total_y));
 /*
  * Send a key: if pixels negative, send KEY_SCROLL_DOWN
  */
 XCursesSendKeyToCurses((unsigned long)KEY_SF);
#if 0
fprintf(stderr,"Scroll_up_down: pixels %d cur_y %d\n",pixels,SP->sb_cur_y);
#endif
}

/***********************************************************************/
#ifdef HAVE_PROTO
static void Scroll_left_right(Widget w, XtPointer client_data, XtPointer call_data)
#else
static void Scroll_left_right(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#endif
/***********************************************************************/
{
 int pixels = (int) call_data;
 int total_x=(SP->sb_total_x*XCursesFontWidth);
 int viewport_x=(SP->sb_viewport_x*XCursesFontWidth);
 int cur_x=(SP->sb_cur_x*XCursesFontWidth);

 /* When pixels is negative, right button pressed, move data down,
  * thumb moves up.  Otherwise, left button pressed, pixels 
  * positive, move data up, thumb down. 
  */
 cur_x += pixels;

 /* limit panning to size of overall */
 if (cur_x < 0)
    cur_x = 0;
 else 
   {
    if (cur_x > (total_x - viewport_x))
       cur_x = (total_x - viewport_x);
   }
 SP->sb_cur_x = cur_x / XCursesFontWidth;
 XawScrollbarSetThumb(w,(float)((float)cur_x/(float)total_x),(float)((float)viewport_x/(float)total_x));
 /*
  * Send a key: if pixels negative, send KEY_SCROLL_DOWN
  */
 XCursesSendKeyToCurses((unsigned long)KEY_SR);
#if 0
fprintf(stderr,"Scroll_left_right: pixels %d cur_x %d\n",pixels,SP->sb_cur_x);
#endif
}
/***********************************************************************/
#ifdef HAVE_PROTO
static void Thumb_up_down(Widget w, XtPointer client_data, XtPointer call_data)
#else
static void Thumb_up_down(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#endif
/***********************************************************************/
{
 float percent = *(float *) call_data;
 float total_y=(float)(SP->sb_total_y);
 float viewport_y=(float)(SP->sb_viewport_y);
 int cur_y=SP->sb_cur_y;

#if 0
fprintf(stderr,"Thumb_up_down: percent %f\n",percent);
#endif
 /*
  * If the size of the viewport is > overall area
  * simply return, as no scrolling is permitted.
  */
 if (SP->sb_viewport_y >= SP->sb_total_y)
    return;
 if ((SP->sb_cur_y = (int)((float)total_y * percent)) >= (total_y - viewport_y))
    SP->sb_cur_y = total_y - viewport_y;

/* SP->sb_cur_y = cur_y;*/
 XawScrollbarSetThumb(w,(float)(cur_y/total_y),(float)(viewport_y/total_y));
 /*
  * Send a key: if pixels negative, send KEY_SCROLL_DOWN
  */
#if 0
fprintf(stderr,"Thumb_up_down: percent %f cur_y %d\n",percent,SP->sb_cur_y);
#endif
 XCursesSendKeyToCurses((unsigned long)KEY_SF);
}
/***********************************************************************/
#ifdef HAVE_PROTO
static void Thumb_left_right(Widget w, XtPointer client_data, XtPointer call_data)
#else
static void Thumb_left_right(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#endif
/***********************************************************************/
{
 float percent = *(float *) call_data;
 float total_x=(float)(SP->sb_total_x);
 float viewport_x=(float)(SP->sb_viewport_x);
 int cur_x=SP->sb_cur_x;

#if 0
fprintf(stderr,"Thumb_up_down: percent %f\n",percent);
#endif
 /*
  * If the size of the viewport is > overall area
  * simply return, as no scrolling is permitted.
  */
 if (SP->sb_viewport_x >= SP->sb_total_x)
    return;
 if ((SP->sb_cur_x = (int)((float)total_x * percent)) >= (total_x - viewport_x))
    SP->sb_cur_x = total_x - viewport_x;

/* SP->sb_cur_x = cur_x;*/
 XawScrollbarSetThumb(w,(float)(cur_x/total_x),(float)(viewport_x/total_x));
 /*
  * Send a key: if pixels negative, send KEY_SCROLL_DOWN
  */
#if 0
fprintf(stderr,"Thumb_up_down: percent %f cur_x %d\n",percent,SP->sb_cur_x);
#endif
 XCursesSendKeyToCurses((unsigned long)KEY_SR);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int XCursesSetupX(char *display_name,int argc, char *argv[])
#else
int XCursesSetupX(display_name,argc,argv)
char *display_name;
int arc;
char *argv[];
#endif
/***********************************************************************/
{
   extern bool sb_started;

   XColor pointerforecolor,pointerbackcolor;
   XrmValue rmfrom,rmto;
   char wait_buf[5];
   int wait_value=0,i=0;
   int minwidth=0,minheight=0;
   int myargc;
   char *myargv[2];
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesSetupX called\n",(XCursesProcess)?"     X":"CURSES");
#endif

   /*
    * The following kludge is to force XtVaAppInitialize() to recognize the
    * name of the program.  Without it, a default value of "main" is used.
    */
   if (argv)
   {
      argv[0] = XCursesProgramName;
#ifdef PDCDEBUG1
      for (i=0;i<argc;i++)
         printf("Arg: %d -> %s\n",i,argv[i]);
#endif
   }
   else
   {
      /*
       * initscr() called rather than initscrX()
       */
      myargv[0] = XCursesProgramName;
      myargv[1] = NULL;
      myargc=1;
   }

   /*---------------------------------------------------------------------*/
   /* Keep open the 'write' end of the socket so the XCurses process can  */
   /* send a CURSES_EXIT to itself from within the signal handler...      */
   /*---------------------------------------------------------------------*/
#if 0
   close ( display_sockets[0] );
#else
   exit_sock = display_sockets[0];
#endif
   display_sock = display_sockets[1];
   close ( key_sockets[0] );
   key_sock = key_sockets[1];
   /*---------------------------------------------------------------------*/
   /* Trap all signals when XCurses is the child process...               */
   /*---------------------------------------------------------------------*/
   for (i=0;i<PDC_MAX_SIGNALS;i++)
      XCursesSetSignal(i,XCursesSignalHandler);
   /*---------------------------------------------------------------------*/
   /* Start defining X Toolkit things...                                  */
   /*---------------------------------------------------------------------*/
#if XtSpecificationRelease > 4
   XtSetLanguageProc(NULL, (XtLanguageProc)NULL,NULL);
#endif
   /*---------------------------------------------------------------------*/
   /* If a DISPLAY value has been supplied, set the env variable DISPLAY. */
   /*---------------------------------------------------------------------*/
   if (display_name)
   {
      strcpy(global_display_name,"DISPLAY=");
      strcat(global_display_name,display_name);
      putenv(global_display_name);
   }
   /*---------------------------------------------------------------------*/
   /* Exit if no DISPLAY variable set...                                  */
   /*---------------------------------------------------------------------*/
   if (getenv("DISPLAY") == NULL)
   {
      fprintf(stderr,"Error: no DISPLAY variable set\n");
      kill(otherpid,SIGKILL);
      return(ERR);
   }
   /*---------------------------------------------------------------------*/
   /* Initialise the top level widget...                                  */
   /*---------------------------------------------------------------------*/
   if (argv)
   {
      topLevel = XtVaAppInitialize( &app_context,
                                    XCursesClassName,
                                    options,
                                    XtNumber(options),
                                    &argc,
                                    argv,
                                    NULL,
/*
                              XtNinput, (XtArgVal)True,
                              XtNtitle,XCursesProgramName,
                              XtNname,"xcurses",
*/
                              NULL);
   }
   else
   {
      topLevel = XtVaAppInitialize( &app_context,
                                    XCursesClassName,
                                    options,
                                    XtNumber(options),
                                    &myargc,
                                    myargv,
                                    NULL,
/*
                              XtNinput, (XtArgVal)True,
                              XtNtitle,XCursesProgramName,
                              XtNname,"xcurses",
*/
                              NULL);
   }
#ifdef PDCDEBUG1
      for (i=0;i<argc;i++)
         printf("Arg: %d -> %s\n",i,argv[i]);
#endif

   XtVaGetApplicationResources(topLevel,&app_data,app_resources,XtNumber(app_resources),NULL);
   /* check application resource values here... */

#if 0
printf("Width %d Height %d\n",XCURSESGEOMETRY.width,XCURSESGEOMETRY.height);
#endif

   XCursesFontWidth = XCURSESNORMALFONTINFO->max_bounds.rbearing - XCURSESNORMALFONTINFO->min_bounds.lbearing;
   XCursesFontHeight = XCURSESNORMALFONTINFO->max_bounds.ascent + XCURSESNORMALFONTINFO->max_bounds.descent;
   /*---------------------------------------------------------------------*/
   /* Check that the bold font and normal fonts are the same size...      */
   /*---------------------------------------------------------------------*/
   if (XCursesFontWidth != XCURSESBOLDFONTINFO->max_bounds.rbearing - XCURSESBOLDFONTINFO->min_bounds.lbearing
   ||  XCursesFontHeight != XCURSESBOLDFONTINFO->max_bounds.ascent + XCURSESBOLDFONTINFO->max_bounds.descent)
   {
      fprintf(stderr,"Error: normal font and bold font are different sizes\n");
      fprintf(stderr,"\tNormal\tBold\n");
      fprintf(stderr,"Width :\t%d\t%d\n",XCursesFontWidth,XCURSESBOLDFONTINFO->max_bounds.rbearing - XCURSESBOLDFONTINFO->min_bounds.lbearing);
      fprintf(stderr,"Height:\t%d\t%d\n",XCursesFontHeight,XCURSESBOLDFONTINFO->max_bounds.ascent + XCURSESBOLDFONTINFO->max_bounds.descent);
      kill(otherpid,SIGKILL);
      return(ERR);
   }
   /*---------------------------------------------------------------------*/
   /* Calculate size of display window...                                 */
   /*---------------------------------------------------------------------*/

   XCursesCOLS = XCURSESCOLS;
   XCursesLINES = XCURSESLINES;
   XCursesWindowWidth = (XCursesFontWidth * XCursesCOLS) + (2 * XCURSESBORDERWIDTH);
   XCursesWindowHeight = (XCursesFontHeight * XCursesLINES) + (2 * XCURSESBORDERWIDTH);
   minwidth = (XCursesFontWidth*2)+(XCURSESBORDERWIDTH*2);
   minheight = (XCursesFontHeight*2)+(XCURSESBORDERWIDTH*2);
   /*---------------------------------------------------------------------*/
   /* Set up the icon for the application. The default is an internal one */
   /* for XCurses. Then set various application level resources...        */
   /*---------------------------------------------------------------------*/
    XCursesGetIcon();
   
    XtVaSetValues(topLevel,
                           XtNwidthInc,XCursesFontWidth,
                           XtNheightInc,XCursesFontHeight,
                           XtNminWidth,minwidth,
                           XtNminHeight,minheight,
                           XtNiconPixmap,icon_pixmap,
                           NULL);
   /*---------------------------------------------------------------------*/
   /* Create a BOX widget in which to draw..                              */
   /*---------------------------------------------------------------------*/
#if 0
   fprintf(stderr,"Width: %d Height: %d sb_started: %d\n",XCursesWindowWidth+XCURSESSCROLLBARWIDTH,
                                   XCursesWindowHeight+XCURSESSCROLLBARWIDTH,sb_started);
#endif
    if (XCURSESSCROLLBARWIDTH != 0
    && sb_started)
      {
       scrollBox = XtVaCreateManagedWidget(XCursesProgramName,scrollBoxWidgetClass,topLevel,
                                      XtNwidth,XCursesWindowWidth+XCURSESSCROLLBARWIDTH+8+2,
                                      XtNheight,XCursesWindowHeight+XCURSESSCROLLBARWIDTH+8+2,
                                      NULL);
       drawing = XtVaCreateManagedWidget(XCursesProgramName,boxWidgetClass,
                                      scrollBox,
                                      XtNwidth,XCursesWindowWidth,
                                      XtNheight,XCursesWindowHeight,
                           XtNwidthInc,XCursesFontWidth,
                           XtNheightInc,XCursesFontHeight,
                                      NULL);
       scrollVert = XtVaCreateManagedWidget("scrollVert", scrollbarWidgetClass,
                                         scrollBox,
                                         XtNorientation, XtorientVertical,
                                         XtNheight, XCursesWindowHeight,
                                         XtNwidth, XCURSESSCROLLBARWIDTH,
                                         NULL);
       XtAddCallback(scrollVert, XtNscrollProc, Scroll_up_down, drawing);
       XtAddCallback(scrollVert, XtNjumpProc, Thumb_up_down, drawing);
       scrollHoriz = XtVaCreateManagedWidget("scrollHoriz", scrollbarWidgetClass, 
                                         scrollBox,
                                          XtNorientation, XtorientHorizontal,
                                          XtNwidth, XCursesWindowWidth,
                                          XtNheight, XCURSESSCROLLBARWIDTH,
                                          NULL);
       XtAddCallback(scrollHoriz, XtNscrollProc, Scroll_left_right, drawing);
       XtAddCallback(scrollHoriz, XtNjumpProc, Thumb_left_right, drawing);
      }
    else
      {
       drawing = XtVaCreateManagedWidget(XCursesProgramName,boxWidgetClass,topLevel,
                                      XtNwidth,XCursesWindowWidth,
                                      XtNheight,XCursesWindowHeight,
                                      NULL);
      }

#if 0
   fprintf(stderr,"Width: %d Height: %d\n",XCursesWindowWidth+XCURSESSCROLLBARWIDTH,
                                   XCursesWindowHeight+XCURSESSCROLLBARWIDTH);
#endif
/*
 XtVaSetValues(drawing,
                                   XtNwidth,XCursesWindowWidth,
                                   XtNheight,XCursesWindowHeight,
                              XtNwidthInc,XCursesFontWidth,
                              XtNheightInc,XCursesFontHeight,
                              XtNminWidth,XCursesFontWidth*2,
                              XtNminHeight,XCursesFontHeight*2,
                        XtNborderWidth,20,
               NULL);
*/
   /*---------------------------------------------------------------------*/
   /* Process any default translations...                                 */
   /*---------------------------------------------------------------------*/
    XCursesTranslations = XtParseTranslationTable(defaultTranslations);
    XtAugmentTranslations(drawing,XCursesTranslations);
    XtAppAddActions(app_context,XCursesActions,XtNumber(XCursesActions));
   /*---------------------------------------------------------------------*/
   /* Process the supplied colors...                                      */
   /*---------------------------------------------------------------------*/
   if (get_colors() == ERR)
   {
      kill(otherpid,SIGKILL);
      return(ERR);
   }
   /*---------------------------------------------------------------------*/
   /* Now have LINES and COLS. Set these in the shared SP so the    */
   /* curses program can find them...                                     */
   /*---------------------------------------------------------------------*/
   LINES = XCursesLINES;
   COLS = XCursesCOLS;
   if ((shmidSP = shmget(shmkeySP,sizeof(SCREEN)+XCURSESSHMMIN,0700|IPC_CREAT)) < 0)
   {
      perror("Cannot allocate shared memory for SCREEN");
      kill(otherpid,SIGKILL);
      return(ERR);
   }
   SP = (SCREEN*)shmat(shmidSP,0,0);
   memset(SP, 0, sizeof(SCREEN));
   SP->XcurscrSize = XCURSCR_SIZE;
   SP->lines = XCursesLINES;
   SP->cols = XCursesCOLS;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:SHM size for curscr %d\n",(XCursesProcess)?"     X":"CURSES",SP->XcurscrSize);
#endif
   if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,SP->XcurscrSize+XCURSESSHMMIN,0700|IPC_CREAT)) < 0)
   {
      perror("Cannot allocate shared memory for curscr");
      kill(otherpid,SIGKILL);
      shmdt((char *)SP);
      shmctl(shmidSP,IPC_RMID,0);
      return(ERR);
   }
   Xcurscr = (unsigned char *)shmat(shmid_Xcurscr,0,0);
   memset(Xcurscr, 0, SP->XcurscrSize); 
   atrtab = (unsigned char *)(Xcurscr+XCURSCR_ATRTAB_OFF);
   PDC_init_atrtab();
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:shmid_Xcurscr %d shmkey_Xcurscr %d LINES %d COLS %d\n",(XCursesProcess)?"     X":"CURSES",shmid_Xcurscr,shmkey_Xcurscr,LINES,COLS);
#endif
   /*---------------------------------------------------------------------*/
   /* Add Event handlers to the drawing widget...                         */
   /*---------------------------------------------------------------------*/
   XtAddEventHandler(drawing,ExposureMask,False,XCursesExpose,NULL);
   /* XtAddEventHandler(drawing,KeyPressMask|KeyReleaseMask,False,XCursesKeyPressKeyRelease,NULL); */
   /* XtAddEventHandler(drawing,ButtonPressMask|ButtonReleaseMask,False,XCursesButtonPressButtonRelease,NULL);*/
   XtAddEventHandler(drawing,StructureNotifyMask,False,XCursesStructureNotify,NULL);
   XtAddEventHandler(drawing,EnterWindowMask|LeaveWindowMask,False,XCursesEnterLeaveWindow,NULL);
#if 0
   XtAddEventHandler(drawing,0,True,XCursesNonmaskable,NULL);
#endif
   XtAddEventHandler(topLevel,0,True,XCursesNonmaskable,NULL);
   /*---------------------------------------------------------------------*/
   /* Add input handler form display_sock (requests from curses program)  */
   /*---------------------------------------------------------------------*/
   XtAppAddInput(app_context,display_sock,(XtPointer)XtInputReadMask,XCursesProcessRequestsFromCurses,NULL);
   /*---------------------------------------------------------------------*/
   /* Leave telling the curses process that it can start to here so that  */
   /* when the curses process makes a request, the Xcurses process can    */
   /* service the request...                                              */
   /*---------------------------------------------------------------------*/
   wait_value = CURSES_CHILD;
   memcpy(wait_buf,(char *)&wait_value,sizeof(int));
   (void)write_socket(display_sock,wait_buf,sizeof(int));
   /*---------------------------------------------------------------------*/
   /* Realize the widget...                                               */
   /*---------------------------------------------------------------------*/
   XtRealizeWidget(topLevel);
   /*---------------------------------------------------------------------*/
   /* Handle trapping of the WM_DELETE_WINDOW property...                 */
   /*---------------------------------------------------------------------*/
   wm_atom[0] = XInternAtom(XtDisplay(topLevel),"WM_DELETE_WINDOW",False);
   wm_atom[1] = XInternAtom(XtDisplay(topLevel),"WM_SAVE_YOURSELF",False);
   /* (void)XSetWMProtocols(XtDisplay(topLevel),RootWindowOfScreen(XtScreen(topLevel)),&wm_delete_window_atom,1);*/
   /* (void)XSetWMProtocols(XtDisplay(topLevel),XCURSESWIN,&wm_delete_window_atom,1);*/
   (void)XSetWMProtocols(XtDisplay(topLevel),XtWindow(topLevel),wm_atom,2);
   /*---------------------------------------------------------------------*/
   /* Create the Graphics Context for drawing. This MUST be done AFTER the*/
   /* associated widget has been realized...                              */
   /*---------------------------------------------------------------------*/
#ifdef PDCDEBUG
   say("before get_GC\n");
#endif
   get_GC(XCURSESDISPLAY,XCURSESWIN,&normal_gc,XCURSESNORMALFONTINFO,COLOR_WHITE,COLOR_BLACK,FALSE);
   get_GC(XCURSESDISPLAY,XCURSESWIN,&bold_gc,XCURSESBOLDFONTINFO,COLOR_WHITE,COLOR_BLACK,FALSE);
   get_GC(XCURSESDISPLAY,XCURSESWIN,&normal_highlight_gc,XCURSESNORMALFONTINFO,COLOR_WHITE,COLOR_BLACK,TRUE);
   get_GC(XCURSESDISPLAY,XCURSESWIN,&bold_highlight_gc,XCURSESBOLDFONTINFO,COLOR_WHITE,COLOR_BLACK,TRUE);
   get_GC(XCURSESDISPLAY,XCURSESWIN,&block_cursor_gc,XCURSESNORMALFONTINFO,COLOR_BLACK,COLOR_CURSOR,FALSE);
   get_GC(XCURSESDISPLAY,XCURSESWIN,&rect_cursor_gc,XCURSESNORMALFONTINFO,COLOR_CURSOR,COLOR_BLACK,FALSE);
   get_GC(XCURSESDISPLAY,XCURSESWIN,&border_gc,XCURSESNORMALFONTINFO,COLOR_BORDER,COLOR_BLACK,FALSE);
   /*
   XSetLineAttributes(XCURSESDISPLAY,normal_gc,2,LineSolid,CapButt,JoinMiter);
   XSetLineAttributes(XCURSESDISPLAY,bold_gc,2,LineSolid,CapButt,JoinMiter);
   */
   XSetLineAttributes(XCURSESDISPLAY,rect_cursor_gc,2,LineSolid,CapButt,JoinMiter);
   XSetLineAttributes(XCURSESDISPLAY,border_gc,XCURSESBORDERWIDTH,LineSolid,CapButt,JoinMiter);
   /*---------------------------------------------------------------------*/
   /* Set the cursor for the application...                               */
   /*---------------------------------------------------------------------*/
   XDefineCursor(XCURSESDISPLAY,XCURSESWIN,XCURSESPOINTER);
   rmfrom.size = sizeof(Pixel);
   rmto.size = sizeof(XColor);
#if XtSpecificationRelease > 4
   rmto.addr = (XPointer)&pointerforecolor;
   rmfrom.addr = (XPointer)&(XCURSESPOINTERFORECOLOR);
   XtConvertAndStore(drawing,XtRPixel,&rmfrom,XtRColor,&rmto);
#else
   rmto.addr = (char *)&pointerforecolor;
   rmfrom.addr = (char *)&(XCURSESPOINTERFORECOLOR);
   XtConvert(drawing,XtRPixel,&rmfrom,XtRColor,&rmto);
#endif
   rmfrom.size = sizeof(Pixel);
   rmto.size = sizeof(XColor);
#if XtSpecificationRelease > 4
   rmfrom.addr = (XPointer)&(XCURSESPOINTERBACKCOLOR);
   rmto.addr = (XPointer)&pointerbackcolor;
   XtConvertAndStore(drawing,XtRPixel,&rmfrom,XtRColor,&rmto);
#else
   rmfrom.addr = (char *)&(XCURSESPOINTERBACKCOLOR);
   rmto.addr = (char *)&pointerbackcolor;
   XtConvert(drawing,XtRPixel,&rmfrom,XtRColor,&rmto);
#endif
   XRecolorCursor(XCURSESDISPLAY,XCURSESPOINTER,&pointerforecolor,&pointerbackcolor);
   /*---------------------------------------------------------------------*/
   /* Convert the supplied compose key to a Keysym...                     */
   /*---------------------------------------------------------------------*/
   compose_key = XStringToKeysym(XCURSESCOMPOSEKEY);
   if (compose_key
   &&  IsModifierKey(compose_key))
   {
      register int i=0,j=0;
      KeyCode *kcp;
      XModifierKeymap *map;
      KeyCode compose_keycode=XKeysymToKeycode(XCURSESDISPLAY,compose_key);
      map = XGetModifierMapping(XCURSESDISPLAY);
      kcp = map->modifiermap;
      for (i=0;i<8;i++)
      {
         for (j=0;j<map->max_keypermod;j++,kcp++)
         {
            if (!*kcp)
               continue;
            if (compose_keycode == *kcp)
            {
               compose_mask = state_mask[i];
               break;
            }
         }
         if (compose_mask)
            break;
      }
      XFreeModifiermap(map);
   }
   /* fprintf(stderr,"COMPOSE KEY: %d %s STATE: %d\n",compose_key,XCURSESCOMPOSEKEY,compose_mask);*/
   
#ifdef FOREIGN
   sleep(20);
   if ((Xim = XOpenIM(XCURSESDISPLAY,NULL,NULL,NULL)) == NULL)
   {
      perror("Cannot open Input Method");
      kill(otherpid,SIGKILL);
      shmdt((char *)SP);
      shmdt((char *)Xcurscr);
      shmctl(shmidSP,IPC_RMID,0);
      shmctl(shmid_Xcurscr,IPC_RMID,0);
      return(ERR);
   }
   XGetIMValues(Xim,XNQueryInputStyle, &im_supported_styles, NULL);
   my_style = XIMPreeditNone|XIMStatusNone;
   /* call XtCvtStringToFontSet ??? */
   if ((Xic = XCreateIC(Xim, XNInputStyle, my_style,
   /*
   
   */
                         NULL)) == NULL)
   {
      perror("ERROR: Cannot create input context");
      kill(otherpid,SIGKILL);
      shmdt((char *)SP);
      shmdt((char *)Xcurscr);
      shmctl(shmidSP,IPC_RMID,0);
      shmctl(shmid_Xcurscr,IPC_RMID,0);
      return(ERR);
   }
   XFree(im_supported_styles);
   XGetICValues(Xic, XNFilterEvents, &im_event_mask, NULL);
   XtAddEventHandler(drawing,im_event_mask,False,NULL,NULL);
   XSetICFocus(Xic);
#endif
   /*---------------------------------------------------------------------*/
   /* Wait for events...                                                  */
   /*---------------------------------------------------------------------*/
   XtAppMainLoop(app_context);
   return(OK); /* won't get here */
}
/***********************************************************************/
#ifdef HAVE_PROTO
static RETSIGTYPE XCursesSignalHandler(int signo)
#else
static RETSIGTYPE XCursesSignalHandler(signo)
int signo;
#endif
/***********************************************************************/
{
   char buf[10];
   int flag = CURSES_EXIT;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:XCursesSignalHandler() - called: %d\n",(XCursesProcess)?"     X":"CURSES",signo);
#endif
   /*
    * Patch by:
    * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 04-Dec-1998
    */
   XCursesSetSignal(signo,XCursesSignalHandler);
#ifdef SIGTSTP
   if (signo == SIGTSTP) { pause(); return; }
#endif
#ifdef SIGCONT
   if (signo == SIGCONT) return;
#endif
#ifdef SIGCLD
   if (signo == SIGCLD) return;
#endif
#ifdef SIGTTIN
   if (signo == SIGTTIN) return;
#endif
#ifdef SIGWINCH
   if (signo == SIGWINCH) return;
#endif
   /*
    * End of patch by:
    * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 04-Dec-1998
    */
   XCursesSetSignal(signo,SIG_IGN);
   /*---------------------------------------------------------------------*/
   /* Send a CURSES_EXIT to myself...                                     */
   /*---------------------------------------------------------------------*/
   memcpy(buf,(char *)&flag,sizeof(int));
   if (write_socket(exit_sock,buf,sizeof(int)) < 0)
      XCursesExitXCursesProcess(7,signo,"exitting from XCursesSignalHandler");
}

/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesRequestorCallbackForGetSelection(Widget w,XtPointer data, Atom *selection,
                                     Atom *type, XtPointer value, 
                                     unsigned long *length, int *format)
#else
void XCursesRequestorCallbackForGetSelection(w,data,selection,type,value,length,format)
Widget w;
XtPointer data;
Atom *selection;
Atom *type;
XtPointer value;
unsigned long *length;
int *format;
#endif
/***********************************************************************/
{
 int rc;
 char buf[12]; /* big enough for 2 integers */
 char *string=(char *)value;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesRequestorCallbackForSelection() - called\n",(XCursesProcess)?"     X":"CURSES");
#endif

 if ((value == NULL) && (*length == 0))
 {
    rc = PDC_CLIP_EMPTY;
    memcpy(buf,(char *)&rc,sizeof(int));
    if (write_socket(display_sock,buf,sizeof(int)) < 0)
       XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesRequestorCallbackForGetSelection");
    return;
 }
 /*
  * Here all is OK, send PDC_CLIP_SUCCESS, then length, then contents
  */
 rc = PDC_CLIP_SUCCESS;
 memcpy(buf,(char *)&rc,sizeof(int));
 if (write_socket(display_sock,buf,sizeof(int)) < 0)
    XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesRequestorCallbackForGetSelection");

 rc = *(length);
 memcpy(buf,(char *)&rc,sizeof(int));
 if (write_socket(display_sock,buf,sizeof(int)) < 0)
    XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesRequestorCallbackForGetSelection");

 if (write_socket(display_sock,string,*length) < 0)
    XCursesExitXCursesProcess(4,SIGKILL,"exiting from XCursesRequestorCallbackForGetSelection");

 return;
}
/***********************************************************************/
#ifdef HAVE_PROTO
void XCursesStructureNotify(Widget w,XtPointer client_data,XEvent *event,Boolean *continue_to_dispatch)
#else
void XCursesStructureNotify(w,client_data,event,continue_to_dispatch)
Widget w;
XtPointer client_data;
XEvent *event;
Boolean *continue_to_dispatch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("%s:XCursesStructureNotify called\n",(XCursesProcess)?"     X":"CURSES");
#endif
 switch(event->type)
   {
    case ConfigureNotify:
#ifdef PDCDEBUG
         say("ConfigureNotify received\n");
#endif
    /* window has been resized, change width and
     * height to send to place_text and place_graphics
     * in next Expose */
    /* also will  need to kill (SIGWINCH) curses process if screen
     * size changes
     */
         resizeXCursesWindowWidth = (event->xconfigure.width);
         resizeXCursesWindowHeight = (event->xconfigure.height);
         after_first_curses_request = False;
#ifdef SIGWINCH
# ifdef BEFORE_CHANGE_BY_G_FUCHS
/*
 * Patch by:
 * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 02-Feb-1999
 */
# else
         SP->resized = 1;
# endif
         kill(otherpid,SIGWINCH);
#endif
         XCursesSendKeyToCurses((unsigned long)KEY_RESIZE);
         break;
    case MapNotify:
#ifdef PDCDEBUG
         say("MapNotify received\n");
#endif
         ReceivedMapNotify=1;
         /*
          * Draw the window border
          */
         if (XCURSESBORDERWIDTH)
            XDrawRectangle(XCURSESDISPLAY,XCURSESWIN,border_gc,
                          (XCURSESBORDERWIDTH/2),(XCURSESBORDERWIDTH/2),
                          (XCursesWindowWidth-XCURSESBORDERWIDTH),
                          (XCursesWindowHeight-XCURSESBORDERWIDTH));
         break;
    default:
#ifdef PDCDEBUG
         if (trace_on) PDC_debug("%s:XCursesStructureNotify - unknown event %d\n",(XCursesProcess)?"     X":"CURSES",event->type);
#endif
         break;
   }
 return;
}
