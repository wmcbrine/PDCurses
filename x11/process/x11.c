/**************************************************************************
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
**************************************************************************/

#include "pdcx11.h"

#include <stdlib.h>

#ifdef PDCDEBUG
const char rcsid_x11 =
	"$Id: x11.c,v 1.33 2006/02/15 07:46:36 wmcbrine Exp $";
#endif

extern AppData app_data;

int visible_cursor = 0;
int windowEntered = 1;
static char *XCursesProgramName;

void XCursesExitXCursesProcess(int rc, int sig, char *msg)
{
	if (rc || sig)
	    fprintf(stderr,
		"%s:XCursesExitXCursesProcess() - called: rc:%d sig:%d <%s>\n",
		XCLOGMSG, rc, sig, msg);

	shmdt((char *)SP);
	shmdt((char *)Xcurscr);
	shmctl(shmidSP, IPC_RMID, 0);
	shmctl(shmid_Xcurscr, IPC_RMID, 0);

	XCursesEndwin();

	shutdown(display_sock, 2);
	close(display_sock);

	shutdown(exit_sock, 2);
	close(exit_sock);

	shutdown(key_sock, 2);
	close(key_sock);

	if (sig)
		kill(otherpid, sig);	/* to kill parent process */

	_exit(rc);
}

/* This function redraws the entire screen. */

void XCursesDisplayScreen(bool highlight)
{
	int row;

	PDC_LOG(("%s:XCursesDisplayScreen() - called:\n", XCLOGMSG));

	for (row = 0; row < XCursesLINES; row++)
	{
		/* loop until we can write to the line -- Patch by: 
		   Georg Fuchs, georg.fuchs@rz.uni-regensburg.de */

		while (*(Xcurscr + XCURSCR_FLAG_OFF + row))
			dummy_function();

		*(Xcurscr + XCURSCR_FLAG_OFF + row) = 1;

		XCursesDisplayText((chtype *)(Xcurscr + XCURSCR_Y_OFF(row)),
			row, 0, COLS, highlight);

		*(Xcurscr + XCURSCR_FLAG_OFF + row) = 0;
	}

	XCursesDisplayCursor(SP->cursrow, SP->curscol, SP->cursrow, 
		SP->curscol);

	/* Draw the border if required */

	if (XCURSESBORDERWIDTH)
		XDrawRectangle(XCURSESDISPLAY, XCURSESWIN, border_gc,
			XCURSESBORDERWIDTH / 2, XCURSESBORDERWIDTH / 2,
			XCursesWindowWidth - XCURSESBORDERWIDTH,
			XCursesWindowHeight - XCURSESBORDERWIDTH);
}

/* This function draws those portions of the screen that have changed. */

int XCursesRefreshScreen(void)
{
	int row, start_col, num_cols;

	PDC_LOG(("%s:XCursesRefreshScreen() - called:\n", XCLOGMSG));

	for (row = 0; row < XCursesLINES; row++)
	{
		num_cols = (int)*(Xcurscr + XCURSCR_LENGTH_OFF + row);

		if (num_cols != 0)
		{
			while (*(Xcurscr + XCURSCR_FLAG_OFF + row))
				dummy_function();

			*(Xcurscr + XCURSCR_FLAG_OFF + row) = 1;

			start_col = (int)*(Xcurscr + XCURSCR_START_OFF + row);

			XCursesDisplayText((chtype *)(Xcurscr + 
				XCURSCR_Y_OFF(row) + (start_col * 
				sizeof(chtype))), row, start_col, 
				num_cols, FALSE);

			*(Xcurscr + XCURSCR_LENGTH_OFF + row) = 0;
			*(Xcurscr + XCURSCR_FLAG_OFF + row) = 0;
		}
	}

	if (mouse_selection)
		SelectionOff();

	XCursesDisplayCursor(SP->cursrow, SP->curscol, SP->cursrow, 
		SP->curscol);

	return 0;
}

void XCursesProcessRequestsFromCurses(XtPointer client_data, int *fid,
				      XtInputId *id) 
{ 
	int s, idx;
	int old_row, new_row;
	int old_x, new_x;
	int pos, num_cols;

	long length;

	char buf[12];		/* big enough for 2 integers */ 
	char title[1024];	/* big enough for window title */ 

	unsigned char save_atrtab[MAX_ATRTAB]; 

	PDC_LOG(("%s:XCursesProcessRequestsFromCurses()\n", XCLOGMSG));

	if (!ReceivedMapNotify) 
	    return; 

	FD_ZERO(&readfds); 
	FD_SET(display_sock, &readfds); 

	if ((s = select(FD_SETSIZE, (FD_SET_CAST)&readfds, NULL, 
	  NULL, &socket_timeout)) < 0)
	    XCursesExitXCursesProcess(2, SIGKILL,
		"exiting from XCursesProcessRequestsFromCurses - select failed");

	if (s == 0)	/* no requests pending - should never happen!*/ 
	    return; 

	if (FD_ISSET(display_sock, &readfds)) 
	{ 
	    /* read first integer to determine total message has 
	       been received */

	    PDC_LOG(("%s:XCursesProcessRequestsFromCurses() - "
		"before read_socket()\n", XCLOGMSG));

	    if (read_socket(display_sock, buf, sizeof(int)) < 0) 
		XCursesExitXCursesProcess(3, SIGKILL,
		  "exiting from XCursesProcessRequestsFromCurses - first read");

	    PDC_LOG(("%s:XCursesProcessRequestsFromCurses() - "
		"after read_socket()\n", XCLOGMSG));

	    memcpy((char *)&num_cols, buf, sizeof(int)); 
	    after_first_curses_request = True; 

	    switch(num_cols) 
	    { 
	    case 0:
		break; 

	    case CURSES_EXIT:	/* request from curses to stop */
		XC_LOG(("CURSES_EXIT received from child\n"));
		XCursesExitXCursesProcess(0, 0,
		    "XCursesProcess requested to exit by child");
		break;

	    case CURSES_BELL:
		XC_LOG(("CURSES_BELL received from child\n"));
		XBell(XCURSESDISPLAY, 50);
		break;

	    case CURSES_CLEAR:
		XC_LOG(("CURSES_CLEAR received from child\n"));
		XClearWindow(XCURSESDISPLAY, XCURSESWIN);
		break;

	    case CURSES_FLASH:
		XC_LOG(("CURSES_FLASH received from child\n"));
#if 0 
		XFillRectangle(XCURSESDISPLAY, XCURSESWIN, 
		    normal_highlight_gc, 10, 10, 
		    XCursesWindowWidth - 10, XCursesWindowHeight - 10);

		napms(50);

		XFillRectangle(XCURSESDISPLAY, XCURSESWIN, 
		    normal_highlight_gc, 10, 10, 
		    XCursesWindowWidth - 10, XCursesWindowHeight - 10);

		XCursesDisplayCursor(SP->cursrow, SP->curscol,
		    SP->cursrow, SP->curscol);
#endif 
		old_x = CURSES_CONTINUE; 
		memcpy(buf, (char *)&old_x, sizeof(int)); 

		if (write_socket(display_sock, buf, sizeof(int)) < 0) 
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses");
		break; 

	    /* request from curses to confirm completion of display */ 

	    case CURSES_REFRESH:
		XC_LOG(("CURSES_REFRESH received from child\n"));
		visible_cursor = 1; 
		XCursesRefreshScreen(); 
		XCursesDisplayCursor(SP->cursrow, SP->curscol, 
		    SP->cursrow, SP->curscol); 

		old_x = CURSES_CONTINUE; 
		memcpy(buf, (char *)&old_x, sizeof(int));

		if (write_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses");
		break;

	    case CURSES_REFRESH_SCROLLBAR:
		XCursesRefreshScrollbar();
		break;

	    case CURSES_CURSOR:
		XC_LOG(("CURSES_CURSOR received from child\n"));
		if (read_socket(display_sock, buf, sizeof(int) * 2) < 0)
		    XCursesExitXCursesProcess(5, SIGKILL,
			"exiting from CURSES_CURSOR "
			"XCursesProcessRequestsFromCurses");

		memcpy((char *)&pos, buf, sizeof(int)); 

		old_row = pos & 0xFF;
		old_x = pos >> 8;
		idx = sizeof(int);
		memcpy((char *)&pos, buf + idx, sizeof(int));

		new_row = pos & 0xFF;
		new_x = pos >> 8;
		visible_cursor = 1;
		XCursesDisplayCursor(old_row, old_x, new_row, new_x);
		break;

	    case CURSES_DISPLAY_CURSOR:
		XC_LOG(("CURSES_DISPLAY_CURSOR received from child. "
			"Vis now: "));
		XC_LOG((visible_cursor ? "1\n" : "0\n"));

		/* If the window is not active, ignore this 
		   command. The cursor will stay solid. */

		if (windowEntered) 
		{ 
		    if (visible_cursor) 
		    { 
			/* Cursor currently ON, turn it off */

			int save_visibility = SP->visibility;
			SP->visibility = 0;

			XCursesDisplayCursor(SP->cursrow, SP->curscol, 
			    SP->cursrow, SP->curscol);

			SP->visibility = save_visibility;
			visible_cursor = 0;
		    } 
		    else 
		    { 
			/* Cursor currently OFF, turn it on */ 

			XCursesDisplayCursor(SP->cursrow, SP->curscol,
			    SP->cursrow, SP->curscol);

			visible_cursor = 1; 
		    } 
		} 

		break; 

	    case CURSES_TITLE:
		XC_LOG(("CURSES_TITLE received from child\n"));

		if (read_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(5, SIGKILL,
			"exiting from CURSES_TITLE "
			"XCursesProcessRequestsFromCurses");

		memcpy((char *)&pos, buf, sizeof(int));

		if (read_socket(display_sock, title, pos) < 0)
		    XCursesExitXCursesProcess(5, SIGKILL,
			"exiting from CURSES_TITLE "
			"XCursesProcessRequestsFromCurses");

		XtVaSetValues(topLevel, XtNtitle, title, NULL);
		break;

	    case CURSES_RESIZE:
		after_first_curses_request = False;
		XC_LOG(("CURSES_RESIZE received from child\n"));

		SP->lines = XCursesLINES = ((resizeXCursesWindowHeight -
		    (2 * XCURSESBORDERWIDTH)) / XCursesFontHeight);

		LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;

		SP->cols = COLS = XCursesCOLS = ((resizeXCursesWindowWidth -
		    (2 * XCURSESBORDERWIDTH)) / XCursesFontWidth);

		XCursesWindowWidth = resizeXCursesWindowWidth;
		XCursesWindowHeight = resizeXCursesWindowHeight;
		visible_cursor = 1;

		/* Draw the border if required */ 

		if (XCURSESBORDERWIDTH) 
		    XDrawRectangle(XCURSESDISPLAY, XCURSESWIN, border_gc,
			XCURSESBORDERWIDTH / 2, XCURSESBORDERWIDTH / 2,
			XCursesWindowWidth - XCURSESBORDERWIDTH,
			XCursesWindowHeight - XCURSESBORDERWIDTH);

		/* Detach and drop the current shared memory segment and 
		   create and attach to a new segment */

		memcpy(save_atrtab, atrtab, sizeof(save_atrtab));

		SP->XcurscrSize = XCURSCR_SIZE;
		shmdt((char *)Xcurscr);
		shmctl(shmid_Xcurscr, IPC_RMID, 0);

		if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,
		    SP->XcurscrSize + XCURSESSHMMIN, 0700 | IPC_CREAT)) < 0)
		{ 
		    perror("Cannot allocate shared memory for curscr");
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses");
		} 

		Xcurscr = (unsigned char*)shmat(shmid_Xcurscr, 0, 0);
		memset(Xcurscr, 0, SP->XcurscrSize);
		atrtab = (unsigned char *)(Xcurscr + XCURSCR_ATRTAB_OFF);
		memcpy(atrtab, save_atrtab, sizeof(save_atrtab));
 
		old_x = CURSES_CONTINUE; 
		memcpy(buf, (char *)&old_x, sizeof(int)); 

		if (write_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses"); 
		break; 

	    case CURSES_GET_SELECTION:
		XC_LOG(("CURSES_GET_SELECTION received from child\n"));
		old_x = CURSES_CONTINUE;
		memcpy(buf, (char *)&old_x, sizeof(int));

		if (write_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses");

		XtGetSelectionValue(topLevel, XA_PRIMARY, XA_STRING,
		    XCursesRequestorCallbackForGetSelection,
		    (XtPointer)NULL, 0);

		break;

	    case CURSES_SET_SELECTION:
		XC_LOG(("CURSES_SET_SELECTION received from child\n"));

		if (read_socket(display_sock, buf, sizeof(long)) < 0)
		    XCursesExitXCursesProcess(5, SIGKILL,
			"exiting from CURSES_TITLE "
			"XCursesProcessRequestsFromCurses");

		memcpy((char *)&length, buf, sizeof(long));

		if (length > (long)tmpsel_length)
		{
		    if (tmpsel_length == 0)
			tmpsel = (char *)malloc(length + 1);
		    else
			tmpsel = (char *)realloc(tmpsel, length + 1);
		}

		if (!tmpsel)
		{
		    old_x = PDC_CLIP_MEMORY_ERROR;

		    memcpy(buf, (char *)&old_x, sizeof(int));

		    if (write_socket(display_sock, buf, sizeof(int)) < 0)
			XCursesExitXCursesProcess(4, SIGKILL,
			    "exiting from XCursesProcessRequestsFromCurses");
		    break;
		}

		if (read_socket(display_sock, tmpsel, length) < 0)
		    XCursesExitXCursesProcess(5, SIGKILL,
			"exiting from CURSES_TITLE XCursesProcessRequestsFromCurses");

		tmpsel_length = length;
		tmpsel[length] = '\0';

		if (XtOwnSelection(topLevel, XA_PRIMARY, CurrentTime,
		    XCursesConvertProc, XCursesLoseOwnership, NULL) == False)
		{
		    old_x = PDC_CLIP_ACCESS_ERROR;
		    free(tmpsel);
		    tmpsel = NULL;
		    tmpsel_length = 0;
		}
		else
		    old_x = PDC_CLIP_SUCCESS;

		SelectionOff();
		memcpy(buf, (char *)&old_x, sizeof(int));

		if (write_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses");
		break;

	    case CURSES_CLEAR_SELECTION:
		XC_LOG(("CURSES_CLEAR_SELECTION received from child\n"));
		old_x = CURSES_CONTINUE;
		memcpy(buf, (char *)&old_x, sizeof(int));

		if (write_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesProcessRequestsFromCurses");

		SelectionOff();
		break;

	    default: 
		PDC_LOG(("%s:Unknown request %d\n", XCLOGMSG, num_cols));
	    } 
	} 
} 

int XCursesSetupX(char *display_name, int argc, char *argv[])
{
	static char *myargv[] = {"PDCurses", NULL};
	extern bool sb_started;

	int italic_font_valid;
	XColor pointerforecolor, pointerbackcolor;
	XrmValue rmfrom, rmto;
	XWMHints wmhints;
	char wait_buf[5];
	int wait_value = 0, i = 0;
	int minwidth = 0, minheight = 0;

	PDC_LOG(("%s:XCursesSetupX called\n", XCLOGMSG));

	/* The following kludge is to force XtVaAppInitialize() to 
	   recognize the name of the program.  Without it, a default 
	   value of "PDCurses" is used. */

	if (!argv)
	{
		argv = myargv;
		argc = 1;
	}

	XCursesProgramName = argv[0];

	/* Keep open the 'write' end of the socket so the XCurses 
	   process can send a CURSES_EXIT to itself from within the 
	   signal handler */

	exit_sock = display_sockets[0];
	display_sock = display_sockets[1];

	close(key_sockets[0]);
	key_sock = key_sockets[1];

	/* Trap all signals when XCurses is the child process, but only 
	   if they haven't already been ignored by the application. */

	for (i = 0; i < PDC_MAX_SIGNALS; i++)
		if (XCursesSetSignal(i, XCursesSignalHandler) == SIG_IGN)
			XCursesSetSignal(i, SIG_IGN);

	/* Start defining X Toolkit things */

#if XtSpecificationRelease > 4
	XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
#endif

	/* If a DISPLAY value has been supplied, set the env variable 
	   DISPLAY */

	if (display_name)
	{
		strcpy(global_display_name, "DISPLAY=");
		strcat(global_display_name, display_name);
		putenv(global_display_name);
	}

	/* Exit if no DISPLAY variable set */

	if (getenv("DISPLAY") == NULL)
	{
		fprintf(stderr, "Error: no DISPLAY variable set\n");
		kill(otherpid, SIGKILL);
		return ERR;
	}

	/* Initialise the top level widget */

	topLevel = XtVaAppInitialize(&app_context, XCursesClassName,
		options, XtNumber(options), &argc, argv, NULL, NULL);

	XtVaGetApplicationResources(topLevel, &app_data, app_resources,
		XtNumber(app_resources), NULL);

	/* Check application resource values here */

	XCursesFontWidth = XCURSESNORMALFONTINFO->max_bounds.rbearing - 
		XCURSESNORMALFONTINFO->min_bounds.lbearing;

	XCursesFontHeight = XCURSESNORMALFONTINFO->max_bounds.ascent + 
		XCURSESNORMALFONTINFO->max_bounds.descent;

	XCursesFontAscent = XCURSESNORMALFONTINFO->max_bounds.ascent;
	XCursesFontDescent = XCURSESNORMALFONTINFO->max_bounds.descent;

	/* Check that the italic font and normal fonts are the same size */
	/* This appears backwards */

	italic_font_valid = XCursesFontWidth !=
		XCURSESITALICFONTINFO->max_bounds.rbearing -
		XCURSESITALICFONTINFO->min_bounds.lbearing ||
		XCursesFontHeight !=
		XCURSESITALICFONTINFO->max_bounds.ascent + 
		XCURSESITALICFONTINFO->max_bounds.descent;

	/* Calculate size of display window */

	XCursesCOLS = XCURSESCOLS;
	XCursesLINES = XCURSESLINES;

	XCursesWindowWidth = XCursesFontWidth * XCursesCOLS +
		2 * XCURSESBORDERWIDTH;

	XCursesWindowHeight = XCursesFontHeight * XCursesLINES +
		2 * XCURSESBORDERWIDTH;

	minwidth = XCursesFontWidth * 2 + XCURSESBORDERWIDTH * 2;
	minheight = XCursesFontHeight * 2 + XCURSESBORDERWIDTH * 2;

	/* Set up the icon for the application; the default is an
	   internal one for XCurses. Then set various application level
	   resources. */

	XCursesGetIcon();

#ifdef HAVE_XPM_H
	if (XCURSESPIXMAPFILE != NULL && strcmp(XCURSESPIXMAPFILE, "") != 0)
		XtVaSetValues(topLevel, XtNminWidth, minwidth,
			XtNminHeight, minheight, XtNiconPixmap,
			icon_pixmap, XtNiconMask, icon_pixmap_mask, NULL);
	else
#endif
		XtVaSetValues(topLevel, XtNminWidth, minwidth, 
			XtNminHeight, minheight, XtNiconPixmap, 
			icon_bitmap, NULL);

	/* Create a BOX widget in which to draw */

	if (XCURSESSCROLLBARWIDTH != 0 && sb_started)
	{
		scrollBox = XtVaCreateManagedWidget(XCursesProgramName, 
			scrollBoxWidgetClass, topLevel, XtNwidth, 
			XCursesWindowWidth + XCURSESSCROLLBARWIDTH, 
			XtNheight, XCursesWindowHeight + 
			XCURSESSCROLLBARWIDTH, XtNwidthInc, 
			XCursesFontWidth, XtNheightInc, 
			XCursesFontHeight, NULL);

		drawing = XtVaCreateManagedWidget(XCursesProgramName, 
			boxWidgetClass, scrollBox, XtNwidth, 
			XCursesWindowWidth, XtNheight, 
			XCursesWindowHeight, XtNwidthInc, 
			XCursesFontWidth, XtNheightInc, 
			XCursesFontHeight, NULL);

		scrollVert = XtVaCreateManagedWidget("scrollVert", 
			scrollbarWidgetClass, scrollBox, XtNorientation, 
			XtorientVertical, XtNheight, 
			XCursesWindowHeight, XtNwidth, 
			XCURSESSCROLLBARWIDTH, NULL);

		XtAddCallback(scrollVert, XtNscrollProc,
			Scroll_up_down, drawing);

		XtAddCallback(scrollVert, XtNjumpProc,
			Thumb_up_down, drawing);

		scrollHoriz = XtVaCreateManagedWidget("scrollHoriz", 
			scrollbarWidgetClass, scrollBox, XtNorientation, 
			XtorientHorizontal, XtNwidth, 
			XCursesWindowWidth, XtNheight, 
			XCURSESSCROLLBARWIDTH, NULL);

		XtAddCallback(scrollHoriz, XtNscrollProc, 
			Scroll_left_right, drawing);

		XtAddCallback(scrollHoriz, XtNjumpProc, 
			Thumb_left_right, drawing);
	}
	else
	{
		drawing = XtVaCreateManagedWidget(XCursesProgramName, 
			boxWidgetClass, topLevel, XtNwidth, 
			XCursesWindowWidth, XtNheight, 
			XCursesWindowHeight, XtNwidthInc, 
			XCursesFontWidth, XtNheightInc, 
			XCursesFontHeight, NULL);

		XtVaSetValues(topLevel, XtNwidthInc, XCursesFontWidth, 
			XtNheightInc, XCursesFontHeight, NULL);
	}

	/* Process any default translations */

	XCursesTranslations = XtParseTranslationTable(defaultTranslations);
	XtAugmentTranslations(drawing, XCursesTranslations);
	XtAppAddActions(app_context, XCursesActions, XtNumber(XCursesActions));

	/* Process the supplied colors */

	if (get_colors() == ERR)
	{
		kill(otherpid, SIGKILL);
		return ERR;
	}

	/* Determine text cursor alignment from resources */

	if (strcmp(XCURSESTEXTCURSOR, "vertical") == 0)
		vertical_cursor = True;

	/* Now have LINES and COLS. Set these in the shared SP so the 
	   curses program can find them. */

	LINES = XCursesLINES;
	COLS = XCursesCOLS;

	if ((shmidSP = shmget(shmkeySP, sizeof(SCREEN) + XCURSESSHMMIN, 
	    0700 | IPC_CREAT)) < 0)
	{
		perror("Cannot allocate shared memory for SCREEN");
		kill(otherpid, SIGKILL);
		return ERR;
	}

	SP = (SCREEN*)shmat(shmidSP, 0, 0);
	memset(SP, 0, sizeof(SCREEN));
	SP->XcurscrSize = XCURSCR_SIZE;
	SP->lines = XCursesLINES;
	SP->cols = XCursesCOLS;

	PDC_LOG(("%s:SHM size for curscr %d\n", XCLOGMSG, SP->XcurscrSize));

	if ((shmid_Xcurscr = shmget(shmkey_Xcurscr, SP->XcurscrSize + 
	    XCURSESSHMMIN, 0700 | IPC_CREAT)) < 0)
	{
		perror("Cannot allocate shared memory for curscr");
		kill(otherpid, SIGKILL);
		shmdt((char *)SP);
		shmctl(shmidSP, IPC_RMID, 0);
		return ERR;
	}

	Xcurscr = (unsigned char *)shmat(shmid_Xcurscr, 0, 0);
	memset(Xcurscr, 0, SP->XcurscrSize); 
	atrtab = (unsigned char *)(Xcurscr + XCURSCR_ATRTAB_OFF);
	PDC_init_atrtab();

	PDC_LOG(("%s:shmid_Xcurscr %d shmkey_Xcurscr %d LINES %d COLS %d\n",
		XCLOGMSG, shmid_Xcurscr, shmkey_Xcurscr, LINES, COLS));

	/* Add Event handlers to the drawing widget */

	XtAddEventHandler(drawing, ExposureMask, False, XCursesExpose, NULL);
	XtAddEventHandler(drawing, StructureNotifyMask, False, 
		XCursesStructureNotify, NULL);
	XtAddEventHandler(drawing, EnterWindowMask | LeaveWindowMask, 
		False, XCursesEnterLeaveWindow, NULL);

	XtAddEventHandler(topLevel, 0, True, XCursesNonmaskable, NULL);

	/* Add input handler from display_sock (requests from curses 
	   program) */

	XtAppAddInput(app_context, display_sock, 
		(XtPointer)XtInputReadMask, 
		XCursesProcessRequestsFromCurses, NULL);

	/* If there is a cursorBlink resource, start the Timeout event */

	if (XCURSESCURSORBLINKRATE)
		XtAppAddTimeOut(app_context, XCURSESCURSORBLINKRATE, 
			XCursesCursorBlink, NULL);

	/* Leave telling the curses process that it can start to here so 
	   that when the curses process makes a request, the Xcurses 
	   process can service the request.*/

	wait_value = CURSES_CHILD;
	memcpy(wait_buf, (char *)&wait_value, sizeof(int));
	write_socket(display_sock, wait_buf, sizeof(int));

	XtRealizeWidget(topLevel);

	/* Handle trapping of the WM_DELETE_WINDOW property */

	wm_atom[0] = XInternAtom(XtDisplay(topLevel), 
		"WM_DELETE_WINDOW", False);

	XSetWMProtocols(XtDisplay(topLevel), XtWindow(topLevel), 
		wm_atom, 1);

	/* Create the Graphics Context for drawing. This MUST be done 
	   AFTER the associated widget has been realized. */

	XC_LOG(("before get_GC\n"));

	get_GC(XCURSESDISPLAY, XCURSESWIN, &normal_gc, 
		XCURSESNORMALFONTINFO, COLOR_WHITE, COLOR_BLACK, FALSE);

	if (italic_font_valid)
		get_GC(XCURSESDISPLAY, XCURSESWIN, &italic_gc, 
			XCURSESITALICFONTINFO, COLOR_WHITE, COLOR_BLACK, 
			FALSE);
	else
		get_GC(XCURSESDISPLAY, XCURSESWIN, &italic_gc, 
			XCURSESNORMALFONTINFO, COLOR_WHITE, COLOR_BLACK, 
			FALSE);

	get_GC(XCURSESDISPLAY, XCURSESWIN, &block_cursor_gc,
		XCURSESNORMALFONTINFO, COLOR_BLACK, COLOR_CURSOR, FALSE);

	get_GC(XCURSESDISPLAY, XCURSESWIN, &rect_cursor_gc, 
		XCURSESNORMALFONTINFO, COLOR_CURSOR, COLOR_BLACK, FALSE);

	get_GC(XCURSESDISPLAY, XCURSESWIN, &border_gc, 
		XCURSESNORMALFONTINFO, COLOR_BORDER, COLOR_BLACK, FALSE);

	XSetLineAttributes(XCURSESDISPLAY, rect_cursor_gc, 2, LineSolid, 
		CapButt, JoinMiter);

	XSetLineAttributes(XCURSESDISPLAY, border_gc, 
		XCURSESBORDERWIDTH, LineSolid, CapButt, JoinMiter);

	/* Set the cursor for the application */

	XDefineCursor(XCURSESDISPLAY, XCURSESWIN, XCURSESPOINTER);
	rmfrom.size = sizeof(Pixel);
	rmto.size = sizeof(XColor);

#if XtSpecificationRelease > 4
	rmto.addr = (XPointer)&pointerforecolor;
	rmfrom.addr = (XPointer)&(XCURSESPOINTERFORECOLOR);
	XtConvertAndStore(drawing, XtRPixel, &rmfrom, XtRColor, &rmto);
#else
	rmto.addr = (char *)&pointerforecolor;
	rmfrom.addr = (char *)&(XCURSESPOINTERFORECOLOR);
	XtConvert(drawing, XtRPixel, &rmfrom, XtRColor, &rmto);
#endif

	rmfrom.size = sizeof(Pixel);
	rmto.size = sizeof(XColor);

#if XtSpecificationRelease > 4
	rmfrom.addr = (XPointer)&(XCURSESPOINTERBACKCOLOR);
	rmto.addr = (XPointer)&pointerbackcolor;
	XtConvertAndStore(drawing, XtRPixel, &rmfrom, XtRColor, &rmto);
#else
	rmfrom.addr = (char *)&(XCURSESPOINTERBACKCOLOR);
	rmto.addr = (char *)&pointerbackcolor;
	XtConvert(drawing, XtRPixel, &rmfrom, XtRColor, &rmto);
#endif

	XRecolorCursor(XCURSESDISPLAY, XCURSESPOINTER,
		&pointerforecolor, &pointerbackcolor);

	/* Convert the supplied compose key to a Keysym */

	compose_key = XStringToKeysym(XCURSESCOMPOSEKEY);

	if (compose_key && IsModifierKey(compose_key))
	{
		int i, j;
		KeyCode *kcp;
		XModifierKeymap *map;
		KeyCode compose_keycode = 
			XKeysymToKeycode(XCURSESDISPLAY, compose_key);

		map = XGetModifierMapping(XCURSESDISPLAY);
		kcp = map->modifiermap;

		for (i = 0; i < 8; i++)
		{
			for (j = 0; j < map->max_keypermod; j++, kcp++)
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

#ifdef FOREIGN
	sleep(20);

	if ((Xim = XOpenIM(XCURSESDISPLAY,NULL,NULL,NULL)) == NULL)
	{
		perror("Cannot open Input Method");
		kill(otherpid, SIGKILL);
		shmdt((char *)SP);
		shmdt((char *)Xcurscr);
		shmctl(shmidSP, IPC_RMID, 0);
		shmctl(shmid_Xcurscr, IPC_RMID, 0);
		return ERR;
	}

	XGetIMValues(Xim,XNQueryInputStyle, &im_supported_styles, NULL);
	my_style = XIMPreeditNone | XIMStatusNone;

	if ((Xic = XCreateIC(Xim, XNInputStyle, my_style, NULL)) == NULL)
	{
		perror("ERROR: Cannot create input context");
		kill(otherpid, SIGKILL);
		shmdt((char *)SP);
		shmdt((char *)Xcurscr);
		shmctl(shmidSP, IPC_RMID, 0);
		shmctl(shmid_Xcurscr, IPC_RMID, 0);
		return ERR;
	}

	XFree(im_supported_styles);
	XGetICValues(Xic, XNFilterEvents, &im_event_mask, NULL);
	XtAddEventHandler(drawing, im_event_mask, False, NULL, NULL);
	XSetICFocus(Xic);
#endif

	/* Keep focus on Click-to-front window managers. Change courtesy 
	   of Jean-Pierre Demailly. */

	wmhints.flags = InputHint;
	wmhints.input = True;

	XSetWMHints(XtDisplay(topLevel), XtWindow(topLevel), &wmhints);

	/* Wait for events */

	XtAppMainLoop(app_context);
	return OK;			/* won't get here */
}

RETSIGTYPE XCursesSignalHandler(int signo)
{
	char buf[10];
	int flag = CURSES_EXIT;

	PDC_LOG(("%s:XCursesSignalHandler() - called: %d\n", XCLOGMSG, signo));

	/* Patch by: Georg Fuchs */

	XCursesSetSignal(signo, XCursesSignalHandler);

#ifdef SIGTSTP
	if (signo == SIGTSTP)
	{
		pause();
		return;
	}
#endif
#ifdef SIGCONT
	if (signo == SIGCONT)
		return;
#endif
#ifdef SIGCLD
	if (signo == SIGCLD)
		return;
#endif
#ifdef SIGTTIN
	if (signo == SIGTTIN)
		return;
#endif
#ifdef SIGWINCH
	if (signo == SIGWINCH)
		return;
#endif

	/* End of patch by: Georg Fuchs */

	XCursesSetSignal(signo, SIG_IGN);

	/* Send a CURSES_EXIT to myself */

	memcpy(buf, (char *)&flag, sizeof(int));

	if (write_socket(exit_sock, buf, sizeof(int)) < 0)
		XCursesExitXCursesProcess(7, signo,
			"exiting from XCursesSignalHandler");
}

void XCursesRequestorCallbackForGetSelection(Widget w, XtPointer data,
					     Atom *selection, Atom *type,
					     XtPointer value,
					     unsigned long *length, int *format)
{
	int rc;
	char buf[12];		/* big enough for 2 integers */
	char *string = (char *)value;

	PDC_LOG(("%s:XCursesRequestorCallbackForSelection() - called\n",
		XCLOGMSG));

	if ((value == NULL) && (*length == 0))
	{
		rc = PDC_CLIP_EMPTY;
		memcpy(buf, (char *)&rc, sizeof(int));

		if (write_socket(display_sock, buf, sizeof(int)) < 0)
		    XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesRequestorCallbackForGetSelection");
		return;
	}

	/* Here all is OK, send PDC_CLIP_SUCCESS, then length, then 
	   contents */

	rc = PDC_CLIP_SUCCESS;
	memcpy(buf, (char *)&rc, sizeof(int));
	if (write_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesRequestorCallbackForGetSelection");

	rc = *(length);
	memcpy(buf, (char *)&rc, sizeof(int));
	if (write_socket(display_sock, buf, sizeof(int)) < 0)
		XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesRequestorCallbackForGetSelection");

	if (write_socket(display_sock, string, *length) < 0)
		XCursesExitXCursesProcess(4, SIGKILL,
			"exiting from XCursesRequestorCallbackForGetSelection");
}

void XCursesStructureNotify(Widget w, XtPointer client_data, XEvent *event,
			    Boolean *continue_to_dispatch)
{
	PDC_LOG(("%s:XCursesStructureNotify called\n", XCLOGMSG));

	switch(event->type)
	{
	case ConfigureNotify:
		XC_LOG(("ConfigureNotify received\n"));

		/* Window has been resized, change width and height to 
		   send to place_text and place_graphics in next Expose. 
		   Also will need to kill (SIGWINCH) curses process if 
		   screen size changes. */

		resizeXCursesWindowWidth = (event->xconfigure.width);
		resizeXCursesWindowHeight = (event->xconfigure.height);

		after_first_curses_request = False;

#ifdef SIGWINCH
		SP->resized = 1;

		kill(otherpid, SIGWINCH);
#endif
		XCursesSendKeyToCurses((unsigned long)KEY_RESIZE, NULL);
		break;

	case MapNotify:
		XC_LOG(("MapNotify received\n"));

		ReceivedMapNotify = 1;

		/* Draw the window border */

		if (XCURSESBORDERWIDTH)
			XDrawRectangle(XCURSESDISPLAY, XCURSESWIN, 
				border_gc, XCURSESBORDERWIDTH / 2, 
				XCURSESBORDERWIDTH / 2, 
				XCursesWindowWidth - XCURSESBORDERWIDTH, 
				XCursesWindowHeight - XCURSESBORDERWIDTH);
		break;

	default:
		PDC_LOG(("%s:XCursesStructureNotify - unknown event %d\n",
			XCLOGMSG, event->type));
	}
}
