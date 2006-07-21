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

/* $Id: pdcx11.h,v 1.40 2006/07/21 03:23:50 wmcbrine Exp $ */

#define	CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <signal.h>
#include <ctype.h>
#include <pwd.h>

#include <sys/types.h>
#include <sys/socket.h>
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>   /* AIX needs this for FD_ZERO etc macros */
#endif
#include <sys/ipc.h>
#include <sys/shm.h>

#ifndef XPOINTER_TYPEDEFED
typedef char * XPointer;
#endif

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#include <Intrinsic.h>
#include <StringDefs.h>
#include <Shell.h>

#ifdef USE_XAW3D
# include <Xaw3d/Box.h>
# include <Xaw3d/Scrollbar.h>
#elif defined(USE_NEXTAW)
# include <neXtaw/Box.h>
# include <neXtaw/Scrollbar.h>
#else
# include <Xaw/Box.h>
# include <Xaw/Scrollbar.h>
#endif
#include "x11/ScrollBox.h"

#include "Xmu/StdSel.h"
#include "Xmu/Atoms.h"

#include <keysym.h>
#include <Xatom.h>
#if defined FOREIGN
# include <Xlocale.h>
#endif

#ifdef HAVE_DECKEYSYM_H
# include <DECkeysym.h>
#endif

#ifdef HAVE_SUNKEYSYM_H
# include <Sunkeysym.h>
#endif

#ifdef HAVE_XPM_H
# include <xpm.h>
#endif

#include <curses.h>

#define XCURSCR_Y_SIZE      (XCursesLINES * XCursesCOLS * sizeof(chtype))
#define XCURSCR_FLAG_SIZE   (XCursesLINES * sizeof(int))
#define XCURSCR_START_SIZE  (XCursesLINES * sizeof(int))
#define XCURSCR_LENGTH_SIZE (XCursesLINES * sizeof(int))
#define XCURSCR_ATRTAB_SIZE (MAX_ATRTAB * sizeof(unsigned char))
#define XCURSCR_SIZE        (XCURSCR_FLAG_SIZE + XCURSCR_START_SIZE + \
	XCURSCR_LENGTH_SIZE + XCURSCR_Y_SIZE + XCURSCR_ATRTAB_SIZE)

#define XCURSCR_Y_OFF(y)    ((y) * XCursesCOLS * sizeof(chtype))
#define XCURSCR_FLAG_OFF    (XCURSCR_Y_OFF(0) + XCURSCR_Y_SIZE)
#define XCURSCR_START_OFF   (XCURSCR_FLAG_OFF + XCURSCR_FLAG_SIZE)
#define XCURSCR_LENGTH_OFF  (XCURSCR_START_OFF + XCURSCR_START_SIZE)
#define XCURSCR_ATRTAB_OFF  (XCURSCR_LENGTH_OFF + XCURSCR_LENGTH_SIZE)

#define XtNlines	"lines"
#define XtCLines	"Lines"
#define XtNcols		"cols"
#define XtCCols		"Cols"

#define XtCNormalFont	"NormalFont"
#define XtNnormalFont	"normalFont"
#define XtCItalicFont	"ItalicFont"
#define XtNitalicFont	"italicFont"

#define XtCPointer	"Pointer"
#define XtNpointer	"pointer"

#define XtCShmmin	"Shmmin"
#define XtNshmmin	"shmmin"

#define XtCComposeKey	"ComposeKey"
#define XtNcomposeKey	"composeKey"

#define XtCClickPeriod	"ClickPeriod"
#define XtNclickPeriod	"clickPeriod"

#define XtCDoubleClickPeriod	"DoubleClickPeriod"
#define XtNdoubleClickPeriod	"doubleClickPeriod"

#define XtCScrollbarWidth	"ScrollbarWidth"
#define XtNscrollbarWidth	"scrollbarWidth"

#define XtCCursorBlinkRate	"CursorBlinkRate"
#define XtNcursorBlinkRate	"cursorBlinkRate"

#define XtCPointerForeColor	"PointerForeColor"
#define XtNpointerForeColor	"pointerForeColor"
#define XtCPointerBackColor	"PointerBackColor"
#define XtNpointerBackColor	"pointerBackColor"

#define XtNcursorColor	"cursorColor"
#define XtCCursorColor	"CursorColor"

#define XtNtextCursor	"textCursor"
#define XtCTextCursor	"TextCursor"

#define XtNcolorBlack	"colorBlack"
#define XtCColorBlack	"ColorBlack"
#define XtNcolorRed	"colorRed"
#define XtCColorRed	"ColorRed"
#define XtNcolorGreen	"colorGreen"
#define XtCColorGreen	"ColorGreen"
#define XtNcolorYellow	"colorYellow"
#define XtCColorYellow	"ColorYellow"
#define XtNcolorBlue	"colorBlue"
#define XtCColorBlue	"ColorBlue"
#define XtNcolorMagenta "colorMagenta"
#define XtCColorMagenta "ColorMagenta"
#define XtNcolorCyan	"colorCyan"
#define XtCColorCyan	"ColorCyan"
#define XtNcolorWhite	"colorWhite"
#define XtCColorWhite	"ColorWhite"

#define XtNcolorBoldBlack	"colorBoldBlack"
#define XtCColorBoldBlack	"ColorBoldBlack"
#define XtNcolorBoldRed		"colorBoldRed"
#define XtCColorBoldRed		"ColorBoldRed"
#define XtNcolorBoldGreen	"colorBoldGreen"
#define XtCColorBoldGreen	"ColorBoldGreen"
#define XtNcolorBoldYellow	"colorBoldYellow"
#define XtCColorBoldYellow	"ColorBoldYellow"
#define XtNcolorBoldBlue	"colorBoldBlue"
#define XtCColorBoldBlue	"ColorBoldBlue"
#define XtNcolorBoldMagenta	"colorBoldMagenta"
#define XtCColorBoldMagenta	"ColorBoldMagenta"
#define XtNcolorBoldCyan	"colorBoldCyan"
#define XtCColorBoldCyan	"ColorBoldCyan"
#define XtNcolorBoldWhite	"colorBoldWhite"
#define XtCColorBoldWhite	"ColorBoldWhite"

#ifndef MAX_PATH
# define MAX_PATH 256
#endif

typedef struct
{
	int lines;
	int cols;
	Pixel cursorColor;
	Pixel colorBlack;
	Pixel colorRed;
	Pixel colorGreen;
	Pixel colorYellow;
	Pixel colorBlue;
	Pixel colorMagenta;
	Pixel colorCyan;
	Pixel colorWhite;
	Pixel colorBoldBlack;
	Pixel colorBoldRed;
	Pixel colorBoldGreen;
	Pixel colorBoldYellow;
	Pixel colorBoldBlue;
	Pixel colorBoldMagenta;
	Pixel colorBoldCyan;
	Pixel colorBoldWhite;
	Pixel pointerForeColor;
	Pixel pointerBackColor;
	XFontStruct *normalfont;
	XFontStruct *italicfont;
	char *bitmapFile;
#ifdef HAVE_XPM_H
	char *pixmapFile;
#endif
	char *composeKey;
	Cursor pointer;
	int shmmin;
	int borderWidth;
	int borderColor;
	int clickPeriod;
	int doubleClickPeriod;
	int scrollbarWidth;
	int cursorBlinkRate;
	char *textCursor;
} AppData;

extern AppData app_data;

#define XCURSESNORMALFONTINFO	app_data.normalfont
#define XCURSESITALICFONTINFO	app_data.italicfont
#define XCURSESLINES		app_data.lines
#define XCURSESCOLS		app_data.cols
#define XCURSESBITMAPFILE	app_data.bitmapFile
#ifdef HAVE_XPM_H
# define XCURSESPIXMAPFILE	app_data.pixmapFile
#endif
#define XCURSESCOMPOSEKEY	app_data.composeKey
#define XCURSESPOINTER		app_data.pointer
#define XCURSESPOINTERFORECOLOR app_data.pointerForeColor
#define XCURSESPOINTERBACKCOLOR app_data.pointerBackColor
#define XCURSESCURSORCOLOR	app_data.cursorColor
#define XCURSESSHMMIN		app_data.shmmin
#define XCURSESBORDERWIDTH	app_data.borderWidth
#define XCURSESBORDERCOLOR	app_data.borderColor
#define XCURSESDOUBLECLICKPERIOD app_data.doubleClickPeriod
#define XCURSESCLICKPERIOD	app_data.clickPeriod
#define XCURSESSCROLLBARWIDTH	app_data.scrollbarWidth
#define XCURSESCURSORBLINKRATE	app_data.cursorBlinkRate
#define XCURSESGEOMETRY		app_data.geometry
#define XCURSESTEXTCURSOR	app_data.textCursor

#define XCURSESDISPLAY		(XtDisplay(drawing))
#define XCURSESWIN		(XtWindow(drawing))

#define XCLOGMSG		(XCursesProcess ? "     X" : "CURSES")

#define MAX_COLORS   8  /* maximum of "normal" colours */
#define COLOR_CURSOR 16 /* colour of cursor - 1 more than 2 * MAX_COLORS */
#define COLOR_BORDER 17 /* colour of border - 2 more than 2 * MAX_COLORS */

#ifdef HAVE_XPM_H
# define PDC_NUMBER_OPTIONS 33
#define PDC_NUMBER_APP_RESOURCES 35
#else
# define PDC_NUMBER_OPTIONS 32
# define PDC_NUMBER_APP_RESOURCES 34
#endif
#define PDC_NUMBER_XCURSES_ACTIONS 5

void dummy_function(void);
void get_GC(Display *, Window, GC *, XFontStruct *, int, int, bool);
int get_colors(void);

int XCursesDisplayText(const chtype *, int, int, int, bool);
void XCursesDisplayScreen(bool);
void XCursesDisplayCursor(int, int, int, int);

int PDC_display_cursor(int, int, int, int, int);

void XCursesEnterLeaveWindow(Widget, XtPointer, XEvent *, Boolean *);
Boolean XCursesConvertProc(Widget, Atom *, Atom *, Atom *, XtPointer *,
			   unsigned long *, int *);
void XCursesLoseOwnership(Widget, Atom *);
void XCursesEndwin(void);
void XCursesExitCursesProcess(int, char *);
void XCursesExitXCursesProcess(int, int, char *);
int XCursesInstruct(int);
int XCursesInstructAndWait(int);
int XCursesInitscr(int, char **);

void SelectionOff(void);

int write_socket(int, const char *, int);
int read_socket(int, char *, int);
int write_display_socket_int(int);

int XCursesSetupX(int argc, char *argv[]);
RETSIGTYPE XCursesSigwinchHandler(int signo);

#ifdef _HPUX_SOURCE
# define FD_SET_CAST int *
#else
# define FD_SET_CAST fd_set *
#endif

extern fd_set readfds;
extern fd_set writefds;
extern struct timeval socket_timeout;

extern unsigned char *Xcurscr;
extern int XCursesProcess;
extern int shmidSP;
extern int shmid_Xcurscr;
extern int shmkeySP;
extern int shmkey_Xcurscr;
extern int otherpid;
extern int XCursesLINES;
extern int XCursesCOLS;
extern unsigned char *atrtab;
extern int display_sock;
extern int key_sock;
extern int display_sockets[2];
extern int key_sockets[2];
extern int exit_sock;

extern GC normal_gc, block_cursor_gc, rect_cursor_gc, italic_gc, border_gc;
extern int XCursesFontHeight, XCursesFontWidth;
extern int XCursesFontAscent, XCursesFontDescent;
extern int XCursesWindowWidth, XCursesWindowHeight;
extern int resizeXCursesWindowWidth, resizeXCursesWindowHeight;
extern char *bitmap_file;
#ifdef HAVE_XPM_H
extern char *pixmap_file;
#endif
extern MOUSE_STATUS Trapped_Mouse_status;
extern KeySym compose_key;
extern int compose_mask;
extern int state_mask[8];
extern Atom wm_atom[2];
extern XtTranslations XCursesTranslations;
extern char *defaultTranslations;
extern XtActionsRec XCursesActions[PDC_NUMBER_XCURSES_ACTIONS];
extern char *XCursesClassName;
extern XtAppContext app_context;
extern Widget topLevel, drawing, scrollBox, scrollVert, scrollHoriz;
extern int ReceivedMapNotify;
extern Boolean mouse_selection;
extern chtype *tmpsel;
extern unsigned long tmpsel_length;
extern Pixmap icon_bitmap;
#ifdef HAVE_XPM_H
extern Pixmap icon_pixmap;
extern Pixmap icon_pixmap_mask;
#endif
extern XtResource app_resources[PDC_NUMBER_APP_RESOURCES];
extern XrmOptionDescRec options[PDC_NUMBER_OPTIONS];
extern char global_display_name[100];
extern Bool after_first_curses_request;
extern int windowEntered;
extern int visible_cursor;
extern Bool vertical_cursor;

typedef RETSIGTYPE (*signal_handler)();

void XCursesNonmaskable(Widget w, XtPointer client_data, XEvent *event, 
			Boolean *continue_to_dispatch);
void XCursesExpose(Widget w, XtPointer client_data, XEvent *event, 
		   Boolean *continue_to_dispatch);
signal_handler XCursesSetSignal(int, signal_handler);
void XCursesGetIcon(void);
int XCursesRefreshScrollbar(void);
void XCursesSendKeyToCurses(unsigned long, MOUSE_STATUS *);
void XCursesCursorBlink(XtPointer unused, XtIntervalId *id);
void Scroll_up_down(Widget w, XtPointer client_data, XtPointer call_data);
void Scroll_left_right(Widget w, XtPointer client_data, XtPointer call_data);
void Thumb_up_down(Widget w, XtPointer client_data, XtPointer call_data);
void Thumb_left_right(Widget w, XtPointer client_data, XtPointer call_data);

#ifdef PDCDEBUG
void XCsay(const char *msg);
# define XC_LOG(x) XCsay x
#else
# define XC_LOG(x)
#endif

#ifdef MOUSE_DEBUG
# define MOUSE_LOG(x) printf x
#else
# define MOUSE_LOG(x)
#endif

enum
{
	CURSES_CLEAR_SELECTION, CURSES_DISPLAY_CURSOR,
	CURSES_SET_SELECTION, CURSES_GET_SELECTION, CURSES_TITLE,
	CURSES_REFRESH_SCROLLBAR, CURSES_RESIZE, CURSES_BELL,
	CURSES_CONTINUE, CURSES_CURSOR, CURSES_CHILD, CURSES_REFRESH,
	CURSES_EXIT
};
