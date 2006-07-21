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

#include <stdlib.h>

RCSID("$Id: pdcx11.c,v 1.78 2006/07/21 15:28:18 wmcbrine Exp $");


/*** Functions that are called by both processes ***/

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

static void dummy_function(void)
{
}

void get_line_lock(int row)
{
	/* loop until we can write to the line -- Patch by:
	   Georg Fuchs, georg.fuchs@rz.uni-regensburg.de */

	while (*(Xcurscr + XCURSCR_FLAG_OFF + row))
		dummy_function();

	*(Xcurscr + XCURSCR_FLAG_OFF + row) = 1;
}

void release_line_lock(int row)
{
	*(Xcurscr + XCURSCR_FLAG_OFF + row) = 0;
}

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


/*** Functions that are called by the "curses" process ***/

int XCursesInstruct(int flag)
{
	PDC_LOG(("%s:XCursesInstruct() - called flag %d\n", XCLOGMSG, flag));

	/* Send a request to X */

	if (write_display_socket_int(flag) < 0)
		XCursesExitCursesProcess(4, "exiting from XCursesInstruct");

	return OK;
}

int XCursesInstructAndWait(int flag)
{
	int result;

	PDC_LOG(("%s:XCursesInstructAndWait() - called\n", XCLOGMSG));

	/* tell X we want to do something */

	XCursesInstruct(flag);

	/* wait for X to say the refresh has occurred*/

	if (read_socket(display_sock, (char *)&result, sizeof(int)) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCursesInstructAndWait");

	if (result != CURSES_CONTINUE)
		XCursesExitCursesProcess(6, "exiting from "
			"XCursesInstructAndWait - synchronization error");

	return OK;
}

static int XCursesSetupCurses(void)
{
	int wait_value;

	PDC_LOG(("%s:XCursesSetupCurses called\n", XCLOGMSG));

	close(display_sockets[1]);
	close(key_sockets[1]);

	display_sock = display_sockets[0];
	key_sock = key_sockets[0];

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	read_socket(display_sock, (char *)&wait_value, sizeof(int));

	if (wait_value != CURSES_CHILD)
		return ERR;

	/* Set LINES and COLS now so that the size of the shared memory 
	   segment can be allocated */

	if ((shmidSP = shmget(shmkeySP, sizeof(SCREEN) + XCURSESSHMMIN, 
	    0700)) < 0)
	{
		perror("Cannot allocate shared memory for SCREEN");
		kill(otherpid, SIGKILL);
		return ERR;
	}

	SP = (SCREEN*)shmat(shmidSP, 0, 0);

	XCursesLINES = SP->lines;
	LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;
	XCursesCOLS = COLS = SP->cols;

	if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,
	    SP->XcurscrSize + XCURSESSHMMIN, 0700)) < 0)
	{
		perror("Cannot allocate shared memory for curscr");
		kill(otherpid, SIGKILL);
		return ERR;
	}

	PDC_LOG(("%s:shmid_Xcurscr %d shmkey_Xcurscr %d LINES %d COLS %d\n",
		XCLOGMSG, shmid_Xcurscr, shmkey_Xcurscr, LINES, COLS));

	Xcurscr = (unsigned char *)shmat(shmid_Xcurscr, 0, 0);
	atrtab = (unsigned char *)(Xcurscr + XCURSCR_ATRTAB_OFF);

	XC_LOG(("cursesprocess exiting from Xinitscr\n"));

	/* Always trap SIGWINCH if the C library supports SIGWINCH */

	XCursesSetSignal(SIGWINCH, XCursesSigwinchHandler);

	atexit(XCursesExit);

	return OK;
}

int XCursesInitscr(int argc, char *argv[])
{
	int pid, rc;

	PDC_LOG(("%s:XCursesInitscr() - called\n", XCLOGMSG));

#if defined FOREIGN
	if (setlocale(LC_ALL, "") == NULL)
	{
		fprintf(stderr, "ERROR: cannot set locale\n");
		return ERR;
	}

	if (!XSupportsLocale())
	{
		fprintf(stderr, "ERROR: X does not support locale\n");
		return ERR;
	}

	if (XSetLocaleModifiers("") == NULL)
		fprintf(stderr, "WARNING: Cannot set locale modifiers\n");
#endif
	shmkeySP = getpid();
	memset(&socket_timeout, '\0', sizeof(socket_timeout));
            
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, display_sockets) < 0)
	{
		fprintf(stderr, "ERROR: cannot create display socketpair\n");
		return ERR;
	}
            
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, key_sockets) < 0)
	{
		fprintf(stderr, "ERROR: cannot create key socketpair\n");
		return ERR;
	}

	pid = fork();

	switch(pid)
	{
	case -1:
		fprintf(stderr, "ERROR: cannot fork()\n");
		return ERR;
		break;

	case 0:	/* child */
		shmkey_Xcurscr = getpid();
#ifdef XISPARENT
		XCursesProcess = 0;
		rc = XCursesSetupCurses();
#else
		XCursesProcess = 1;
		otherpid = getppid();
		rc = XCursesSetupX(argc, argv);
#endif
		break;

      default:	/* parent */
		shmkey_Xcurscr = pid;
#ifdef XISPARENT
		XCursesProcess = 1;
		otherpid = pid;
		rc = XCursesSetupX(argc, argv);
#else
		XCursesProcess = 0;
		rc = XCursesSetupCurses();
#endif
	}

	return rc;
}

static void XCursesCleanupCursesProcess(int rc)
{
	PDC_LOG(("%s:XCursesCleanupCursesProcess() - called: %d\n",
		XCLOGMSG, rc));

	shutdown(display_sock, 2);
	close(display_sock);

	shutdown(key_sock, 2);
	close(key_sock);

	shmdt((char *)SP);
	shmdt((char *)Xcurscr);

	if (rc)
		_exit(rc);
}

void XCursesExitCursesProcess(int rc, char *msg)
{
	PDC_LOG(("%s:XCursesExitCursesProcess() - called: %d %s\n",
		XCLOGMSG, rc, msg));

	endwin();
	XCursesCleanupCursesProcess(rc);
}

void XCursesExit(void)
{
	static bool called = FALSE;

	PDC_LOG(("%s:XCursesExit() - called\n", XCLOGMSG));

	if (FALSE == called)
	{
		XCursesInstruct(CURSES_EXIT);
		XCursesCleanupCursesProcess(0);

		called = TRUE;
	}
}
