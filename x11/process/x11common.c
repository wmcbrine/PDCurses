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

#include "pdcx11.h"
/* 
 * Variables specific to process port
 */
unsigned char *Xcurscr;
int XCursesProcess=1;
int shmidSP;
int shmid_Xcurscr;
int shmkeySP;
int shmkey_Xcurscr;
int otherpid;
int XCursesLINES=24;
int XCursesCOLS=80;
int display_sock;
int key_sock;
int display_sockets[2];
int key_sockets[2];
int exit_sock;

fd_set readfds;
fd_set writefds;
struct timeval socket_timeout;

/***********************************************************************/
#ifdef HAVE_PROTO
int write_socket(int sock_num,char *buf,int len)
#else
int write_socket(sock_num,buf,len)
int sock_num;
char *buf;
int len;
#endif
/***********************************************************************/
{
   int start=0,length=len,rc;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:write_socket called: sock_num %d len %d\n",(XCursesProcess)?"     X":"CURSES",sock_num,len);
#endif
#ifdef MOUSE_DEBUG1
if (sock_num == key_sock)
    printf("%s:write_socket(key) len: %d\n",(XCursesProcess)?"     X":"CURSES",len);
#endif
   while(1)
   {
      rc = write(sock_num,buf+start,length);
      if (rc < 0
      ||  rc == length)
         return(rc);
      length -= rc;
      start = rc;
   }
}
/***********************************************************************/
#ifdef HAVE_PROTO
int read_socket(int sock_num,char *buf,int len)
#else
int read_socket(sock_num,buf,len)
int sock_num;
char *buf;
int len;
#endif
/***********************************************************************/
{
   int start=0,length=len,rc;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("%s:read_socket called: sock_num %d len %d\n",(XCursesProcess)?"     X":"CURSES",sock_num,len);
#endif
   while(1)
   {
      rc = read(sock_num,buf+start,length);
#ifdef MOUSE_DEBUG1
     if (sock_num == key_sock)
       printf("%s:read_socket(key) rc %d errno %d resized: %d\n",(XCursesProcess)?"     X":"CURSES",
                                                        rc,errno,SP->resized);
#endif
      if (rc < 0
      && sock_num == key_sock
      && errno == EINTR
#ifdef BEFORE_CHANGE_BY_G_FUCHS
/*
 * Patch by:
 * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 02-Feb-1999
 */
      && SP->resized == TRUE)
      {
#ifdef MOUSE_DEBUG1
       printf("%s:continuing\n",(XCursesProcess)?"     X":"CURSES");
#endif
         rc = 0;
#else
      && SP->resized != FALSE)
      {
#ifdef MOUSE_DEBUG1
       printf("%s:continuing\n",(XCursesProcess)?"     X":"CURSES");
#endif
         rc = 0;
         if (SP->resized > 1)
            SP->resized = TRUE;
         else
            SP->resized = FALSE;
#endif
         memcpy(buf,(char *)&rc,sizeof(int));
         return(0); /* must be >= 0 to avoid error */
      }

#ifdef BEFORE_CHANGE_BY_G_FUCHS
/*
 * Patch by:
 * Georg Fuchs, georg.fuchs@rz.uni-regensburg.de 02-Feb-1999
 */
      if (rc < 0
#else
      if (rc <= 0
#endif
      ||  rc == length)
         return(rc);
      length -= rc;
      start = rc;
   }
}
