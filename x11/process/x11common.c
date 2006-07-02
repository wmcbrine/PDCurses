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

#include "pdcx11.h"

RCSID("$Id: x11common.c,v 1.16 2006/07/02 07:08:28 wmcbrine Exp $");

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

int write_socket(int sock_num, const char *buf, int len)
{
	int start = 0, rc;

	PDC_LOG(("%s:write_socket called: sock_num %d len %d\n",
		XCLOGMSG, sock_num, len));

#ifdef MOUSE_DEBUG
	if (sock_num == key_sock)
		printf("%s:write_socket(key) len: %d\n", XCLOGMSG, len);
#endif
	while (1)
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

	while (1)
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

int write_display_socket_int(int x)
{
	return write_socket(display_sock, (char *)&x, sizeof(int));
}
