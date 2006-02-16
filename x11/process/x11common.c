/**************************************************************************
* This file is NOT public domain software.  It is Copyright, Mark Hessling
* 1994-2000.
**************************************************************************/

#include "pdcx11.h"

#ifdef PDCDEBUG
const char rcsid_x11common =
        "$Id: x11common.c,v 1.10 2006/02/16 22:59:50 wmcbrine Exp $";
#endif

/* Variables specific to process port */

unsigned char *Xcurscr;

int XCursesProcess = 1;
int shmidSP;
int shmid_Xcurscr;
int shmkeySP;
int shmkey_Xcurscr;
int otherpid;
int XCursesLINES = 24;
int XCursesCOLS = 80;
int display_sock;
int key_sock;
int display_sockets[2];
int key_sockets[2];
int exit_sock;

fd_set readfds;
fd_set writefds;

struct timeval socket_timeout;

int write_socket(int sock_num, char *buf, int len)
{
	int start = 0, rc;

	PDC_LOG(("%s:write_socket called: sock_num %d len %d\n",
		XCLOGMSG, sock_num, len));

#ifdef MOUSE_DEBUG
	if (sock_num == key_sock)
		printf("%s:write_socket(key) len: %d\n", XCLOGMSG, len);
#endif
	while(1)
	{
		rc = write(sock_num, buf + start, len);

		if (rc < 0 || rc == len)
			return rc;

		len -= rc;
		start = rc;
	}
}

int read_socket(int sock_num, char *buf, int len)
{
	int start = 0, length = len, rc;

	PDC_LOG(("%s:read_socket called: sock_num %d len %d\n",
		XCLOGMSG, sock_num, len));

	while(1)
	{
		rc = read(sock_num, buf + start, length);

#ifdef MOUSE_DEBUG
		if (sock_num == key_sock)
		    printf("%s:read_socket(key) rc %d errno %d resized: %d\n",
			XCLOGMSG, rc, errno, SP->resized);
#endif
		if (rc < 0 && sock_num == key_sock && errno == EINTR
		    && SP->resized != FALSE)
		{
			MOUSE_LOG(("%s:continuing\n", XCLOGMSG));

			rc = 0;

			if (SP->resized > 1)
				SP->resized = TRUE;
			else
				SP->resized = FALSE;

			memcpy(buf, (char *)&rc, sizeof(int));
         
			return 0;
		}

		if (rc <= 0 || rc == length)
			return rc;

		length -= rc;
		start = rc;
	}
}
