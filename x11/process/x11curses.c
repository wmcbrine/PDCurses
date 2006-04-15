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

/* This file contains functions that are called by the "curses" process;
   i.e., the parent process. */

#include "pdcx11.h"

#include <stdlib.h>

RCSID("$Id: x11curses.c,v 1.31 2006/04/15 16:55:28 wmcbrine Exp $");

extern AppData app_data;

static void XCursesExitCursesProcess(int, char *);

int XCursesResizeScreen(int nlines, int ncols)
{
	PDC_LOG(("%s:XCursesResizeScreen() - called: Lines: %d Cols: %d\n",
		XCLOGMSG, nlines, ncols));

	shmdt((char *)Xcurscr);
	XCursesInstructAndWait(CURSES_RESIZE);

	if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,
		SP->XcurscrSize + XCURSESSHMMIN, 0700)) < 0)
	{
		perror("Cannot allocate shared memory for curscr");
		kill(otherpid, SIGKILL);
		return ERR;
	}

	XCursesLINES = SP->lines;
	XCursesCOLS = SP->cols;

	PDC_LOG(("%s:shmid_Xcurscr %d shmkey_Xcurscr %d SP->lines %d "
		"SP->cols %d\n", XCLOGMSG, shmid_Xcurscr, 
		shmkey_Xcurscr, SP->lines, SP->cols));

	Xcurscr = (unsigned char*)shmat(shmid_Xcurscr, 0, 0);
	atrtab = (unsigned char *)(Xcurscr + XCURSCR_ATRTAB_OFF);

	SP->resized = FALSE;

	return OK;
}

int XCurses_display_cursor(int oldrow, int oldcol, int newrow, int newcol,
			   int visibility)
{
	char buf[30];
	int idx, pos;

	PDC_LOG(("%s:XCurses_display_cursor() - called: "
		"NEW row %d col %d, vis %d\n",
		XCLOGMSG, newrow, newcol, visibility));

	if (visibility == -1)
	{
		/* Only send the CURSES_DISPLAY_CURSOR message, no data */

		idx = CURSES_DISPLAY_CURSOR;
		memcpy(buf, (char *)&idx, sizeof(int));
		idx = sizeof(int);
	}
	else
	{
		idx = CURSES_CURSOR;
		memcpy(buf, (char *)&idx, sizeof(int));

		idx = sizeof(int);
		pos = oldrow + (oldcol << 8);
		memcpy(buf + idx, (char *)&pos, sizeof(int));

		idx += sizeof(int);
		pos = newrow + (newcol << 8);
		memcpy(buf + idx, (char *)&pos, sizeof(int));

		idx += sizeof(int);
	}

	if (write_socket(display_sock, buf, idx) < 0)
		XCursesExitCursesProcess(1,
			"exitting from XCurses_display_cursor");

	return OK;
}

void XCurses_set_title(const char *title)
{
	char buf[30];
	int idx, len;

	PDC_LOG(("%s:XCurses_set_title() - called: TITLE: %s\n",
		XCLOGMSG, title));

	idx = CURSES_TITLE;
	memcpy(buf, (char *)&idx, sizeof(int));

	idx = sizeof(int);
	len = strlen(title) + 1;		/* write nul character */
	memcpy(buf + idx, (char *)&len, sizeof(int));

	idx += sizeof(int);

	if (write_socket(display_sock, buf, idx) < 0)
		XCursesExitCursesProcess(1, "exiting from XCurses_set_title");

	if (write_socket(display_sock, title, len) < 0)
		XCursesExitCursesProcess(1, "exiting from XCurses_set_title");
}

int XCurses_refresh_scrollbar(void)
{
	char buf[30];
	int idx;

	PDC_LOG(("%s:XCurses_refresh_scrollbar() - called\n", XCLOGMSG));

	idx = CURSES_REFRESH_SCROLLBAR;
	memcpy(buf, (char *)&idx, sizeof(int));
	idx = sizeof(int);

	if (write_socket(display_sock, buf, idx) < 0)
		XCursesExitCursesProcess(1,
			"exiting from XCurses_refresh_scrollbar");

	return OK;
}

int XCurses_rawgetch(void)
{
	unsigned long newkey = 0;
	int key = 0;
	char buf[100];	/* big enough for MOUSE_STATUS struct */

	PDC_LOG(("%s:XCurses_rawgetch() - called\n", XCLOGMSG));

	while (1)
	{
	    if (read_socket(key_sock, buf, sizeof(unsigned long)) < 0)
		XCursesExitCursesProcess(2, 
		    "exiting from XCurses_rawchar");

	    memcpy((char *)&newkey, buf, sizeof(unsigned long));
	    pdc_key_modifier = (newkey >> 24) & 0xFF;
	    key = (int)(newkey & 0x00FFFFFF);

	    if (key == KEY_MOUSE)
	    {
		if (read_socket(key_sock, buf, sizeof(MOUSE_STATUS)) < 0)
		    XCursesExitCursesProcess(2,
			"exiting from XCurses_rawchar");

		memcpy((char *)&Trapped_Mouse_status, buf,
		    sizeof(MOUSE_STATUS));

		/* Check if the mouse has been clicked on a slk area. If 
		   the return value is > 0 (indicating the label number), 
		   return with the KEY_F(key) value. */

		if ((newkey = PDC_mouse_in_slk(TRAPPED_MOUSE_Y_POS,
		    TRAPPED_MOUSE_X_POS)))
		{
		    if (TRAPPED_BUTTON_STATUS(1) & BUTTON_PRESSED)
		    {
			key = KEY_F(newkey);
			break;
		    }
		}

		break;

		MOUSE_LOG(("rawgetch-x: %d y: %d Mouse status: %x\n",
		    MOUSE_X_POS, MOUSE_Y_POS, Mouse_status.changes));
		MOUSE_LOG(("rawgetch-Button1: %x Button2: %x Button3: %x\n",
		    BUTTON_STATUS(1), BUTTON_STATUS(2), BUTTON_STATUS(3)));
	    }
	    else
		break;
	}

	PDC_LOG(("%s:XCurses_rawgetch() - key %d returned\n", XCLOGMSG, key));

	return key;
}

int XCurses_get_input_fd(void)
{
	PDC_LOG(("%s:XCurses_get_input_fd() - called\n", XCLOGMSG));
	PDC_LOG(("%s:XCurses_get_input_fd() - returning %i\n", XCLOGMSG,
		key_sock));

	return key_sock;
}

bool XCurses_kbhit(void)
{
	int s;

	PDC_LOG(("%s:XCurses_kbhit() - called\n", XCLOGMSG));

	/* Is something ready to be read on the socket ? Must be a key. */

	FD_ZERO(&readfds);
	FD_SET(key_sock, &readfds);

	if ((s = select(FD_SETSIZE, (FD_SET_CAST)&readfds, NULL, 
	    NULL, &socket_timeout)) < 0)
		XCursesExitCursesProcess(3,
			"child - exiting from XCurses_kbhit select failed");

	PDC_LOG(("%s:XCurses_kbhit() - returning %s\n", XCLOGMSG,
		(s == 0) ? "FALSE" : "TRUE"));

	if (s == 0)
		return FALSE;

	return TRUE;
}

int XCursesInstruct(int flag)
{
	char buf[10];

	PDC_LOG(("%s:XCursesInstruct() - called flag %d\n", XCLOGMSG, flag));

	/* Send a request to X */

	memcpy(buf, (char *)&flag, sizeof(int));

	if (write_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitCursesProcess(4, "exiting from XCursesInstruct");

	return OK;
}

int XCursesInstructAndWait(int flag)
{
	int result;
	char buf[10];

	PDC_LOG(("%s:XCursesInstructAndWait() - called\n", XCLOGMSG));

	/* tell X we want to do something */

	XCursesInstruct(flag);

	/* wait for X to say the refresh has occurred*/

	if (read_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCursesInstructAndWait");

	memcpy((char *)&result, buf, sizeof(int));

	if (result != CURSES_CONTINUE)
		XCursesExitCursesProcess(6, "exiting from "
			"XCursesInstructAndWait - synchronization error");

	return OK;
}

int XCurses_transform_line(const chtype *ch, int row,
			   int start_col, int num_cols)
{
	PDC_LOG(("%s:XCurses_transform_line() called: row %d start_col %d "
		"num_cols %d flag %d\n", XCLOGMSG, row, start_col, 
		num_cols, *(Xcurscr + XCURSCR_FLAG_OFF + row)));

	/* loop until we can write to the line */

	while (*(Xcurscr + XCURSCR_FLAG_OFF + row))
		dummy_function();

	*(Xcurscr + XCURSCR_FLAG_OFF + row) = 1;

	memcpy(Xcurscr + XCURSCR_Y_OFF(row) + (start_col * sizeof(chtype)),
		ch, num_cols * sizeof(chtype));

	*(Xcurscr + XCURSCR_START_OFF + row) = start_col;
	*(Xcurscr + XCURSCR_LENGTH_OFF + row) = num_cols;

	*(Xcurscr + XCURSCR_FLAG_OFF + row) = 0;

	return 0;
}

static int XCursesSetupCurses(void)
{
	char wait_buf[5];
	int wait_value;

	PDC_LOG(("%s:XCursesSetupCurses called\n", XCLOGMSG));

	close(display_sockets[1]);
	close(key_sockets[1]);

	display_sock = display_sockets[0];
	key_sock = key_sockets[0];

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	read_socket(display_sock, wait_buf, sizeof(int));
	memcpy((char *)&wait_value, wait_buf, sizeof(int));

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

int XCursesInitscr(const char *display_name, int argc, char *argv[])
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
		rc = XCursesSetupX(display_name, argc, argv);
#endif
		break;

      default:	/* parent */
		shmkey_Xcurscr = pid;
#ifdef XISPARENT
		XCursesProcess = 1;
		otherpid = pid;
		rc = XCursesSetupX(display_name, argc, argv);
#else
		XCursesProcess = 0;
		rc = XCursesSetupCurses();
#endif
	}

	return rc;
}

int XCurses_getclipboard(char **contents, long *length)
{
	int result = 0;
	int len;
	char buf[12];

	PDC_LOG(("%s:XCurses_getclipboard() - called\n", XCLOGMSG));

	XCursesInstructAndWait(CURSES_GET_SELECTION);

	if (read_socket(display_sock, buf, sizeof(int)) < 0)
	    XCursesExitCursesProcess(5,
		"exiting from XCurses_getclipboard");

	memcpy((char *)&result, buf, sizeof(int));

	if (result == PDC_CLIP_SUCCESS)
	{
	    if (read_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitCursesProcess(5,
		    "exiting from XCurses_getclipboard");

	    memcpy((char *)&len, buf, sizeof(int));

	    if (len != 0)
	    {
		*contents = (char *)malloc(len + 1);

		if (!*contents)
		    XCursesExitCursesProcess(6, "exiting from "
			"XCurses_getclipboard - synchronization error");

		if (read_socket(display_sock, *contents, len) < 0)
		    XCursesExitCursesProcess(5,
			"exiting from XCurses_getclipboard");

		*length = len;
	    }
	}

	return result;
}

int XCurses_setclipboard(const char *contents, long length)
{
	int rc;
	char buf[12];		/* big enough for 2 integers */
	long len = length;

	PDC_LOG(("%s:XCurses_setclipboard() - called\n", XCLOGMSG));

	XCursesInstruct(CURSES_SET_SELECTION);
	memcpy(buf, (char *)&len, sizeof(long));

	if (write_socket(display_sock, buf, sizeof(long)) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCurses_setclipboard");

	if (write_socket(display_sock, contents, length) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCurses_setclipboard");

	/* Wait for X to do its stuff. Now expect return code. */

	if (read_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCurses_setclipboard");

	memcpy((char *)&rc, buf, sizeof(int));
	return rc;
}

int XCurses_clearclipboard(void)
{
	int rc;
	char buf[12];		/* big enough for 2 integers */
	long len = 0;

	PDC_LOG(("%s:XCurses_clearclipboard() - called\n", XCLOGMSG));

	XCursesInstruct(CURSES_CLEAR_SELECTION);
	memcpy(buf, (char *)&len, sizeof(long));

	if (write_socket(display_sock, buf, sizeof(long)) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCurses_setclipboard");

	/* Wait for X to do its stuff. Now expect return code. */

	if (read_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitCursesProcess(5,
			"exiting from XCurses_clearclipboard");

	memcpy((char *)&rc, buf, sizeof(int));
	return rc;
}

void XCursesCleanupCursesProcess(int rc)
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

static void XCursesExitCursesProcess(int rc, char *msg)
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
