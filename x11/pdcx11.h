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
* 1994-1999. M.Hessling@qut.edu.au
***************************************************************************
*/

#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <pwd.h>

/*
 * #includes specific to running XCurses with seperate processes
 */
#ifdef USE_PROCESSES
# include <sys/types.h>
# include <sys/socket.h>
# ifdef HAVE_FCNTL_H
#  include <fcntl.h>
# endif
# ifdef HAVE_SYS_SELECT_H
#  include <sys/select.h>   /* AIX needs this for FD_ZERO etc macros */
# endif
# include <sys/ipc.h>
# include <sys/shm.h>
#endif

/*
 * #includes specific to running XCurses with threads
 */
#ifdef USE_THREADS
# include <pthreads.h>
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
#include <Xaw3d/Box.h>
#include <Xaw3d/Scrollbar.h>
#else
#include <Xaw/Box.h>
#include <Xaw/Scrollbar.h>
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
#include <DECkeysym.h>
#endif

#ifdef HAVE_SUNKEYSYM_H
#include <Sunkeysym.h>
#endif

#include <curses.h>

#define XtNlines         "lines"
#define XtCLines         "Lines"
#define XtNcols          "cols"
#define XtCCols          "Cols"

#define XtCNormalFont    "NormalFont"
#define XtNnormalFont    "normalFont"
#define XtCBoldFont      "BoldFont"
#define XtNboldFont      "boldFont"

#define XtCPointer       "Pointer"
#define XtNpointer       "pointer"

#define XtCShmmin        "Shmmin"
#define XtNshmmin        "shmmin"

#define XtCComposeKey    "ComposeKey"
#define XtNcomposeKey    "composeKey"

#define XtCClickPeriod   "ClickPeriod"
#define XtNclickPeriod   "clickPeriod"

#define XtCDoubleClickPeriod   "DoubleClickPeriod"
#define XtNdoubleClickPeriod   "doubleClickPeriod"

#define XtCScrollbarWidth   "ScrollbarWidth"
#define XtNscrollbarWidth   "scrollbarWidth"

#define XtCPointerForeColor "PointerForeColor"
#define XtNpointerForeColor "pointerForeColor"
#define XtCPointerBackColor "PointerBackColor"
#define XtNpointerBackColor "pointerBackColor"

#define XtNcursorColor   "cursorColor"
#define XtCCursorColor   "CursorColor"

#define XtNcolorBlack    "colorBlack"
#define XtCColorBlack    "ColorBlack"
#define XtNcolorRed      "colorRed"
#define XtCColorRed      "ColorRed"
#define XtNcolorGreen    "colorGreen"
#define XtCColorGreen    "ColorGreen"
#define XtNcolorYellow   "colorYellow"
#define XtCColorYellow   "ColorYellow"
#define XtNcolorBlue     "colorBlue"
#define XtCColorBlue     "ColorBlue"
#define XtNcolorMagenta  "colorMagenta"
#define XtCColorMagenta  "ColorMagenta"
#define XtNcolorCyan     "colorCyan"
#define XtCColorCyan     "ColorCyan"
#define XtNcolorWhite    "colorWhite"
#define XtCColorWhite    "ColorWhite"

#define XtNcolorBoldBlack    "colorBoldBlack"
#define XtCColorBoldBlack    "ColorBoldBlack"
#define XtNcolorBoldRed      "colorBoldRed"
#define XtCColorBoldRed      "ColorBoldRed"
#define XtNcolorBoldGreen    "colorBoldGreen"
#define XtCColorBoldGreen    "ColorBoldGreen"
#define XtNcolorBoldYellow   "colorBoldYellow"
#define XtCColorBoldYellow   "ColorBoldYellow"
#define XtNcolorBoldBlue     "colorBoldBlue"
#define XtCColorBoldBlue     "ColorBoldBlue"
#define XtNcolorBoldMagenta  "colorBoldMagenta"
#define XtCColorBoldMagenta  "ColorBoldMagenta"
#define XtNcolorBoldCyan     "colorBoldCyan"
#define XtCColorBoldCyan     "ColorBoldCyan"
#define XtNcolorBoldWhite    "colorBoldWhite"
#define XtCColorBoldWhite    "ColorBoldWhite"

#ifndef MAX_PATH
#  define MAX_PATH 256
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
 XFontStruct *boldfont;
 char *bitmapFile;
 char *composeKey;
 Cursor pointer;
 int shmmin;
 int borderWidth;
 int borderColor;
 int clickPeriod;
 int doubleClickPeriod;
 int scrollbarWidth;
#if 0
 XtWidgetGeometry geometry;
#endif
} AppData;
AppData app_data;

#define XCURSESNORMALFONTINFO   app_data.normalfont
#define XCURSESBOLDFONTINFO     app_data.boldfont
#define XCURSESLINES            app_data.lines
#define XCURSESCOLS             app_data.cols
#define XCURSESBITMAPFILE       app_data.bitmapFile
#define XCURSESCOMPOSEKEY       app_data.composeKey
#define XCURSESPOINTER          app_data.pointer
#define XCURSESPOINTERFORECOLOR app_data.pointerForeColor
#define XCURSESPOINTERBACKCOLOR app_data.pointerBackColor
#define XCURSESCURSORCOLOR      app_data.cursorColor
#define XCURSESSHMMIN           app_data.shmmin
#define XCURSESBORDERWIDTH      app_data.borderWidth
#define XCURSESBORDERCOLOR      app_data.borderColor
#define XCURSESDOUBLECLICKPERIOD app_data.doubleClickPeriod
#define XCURSESCLICKPERIOD      app_data.clickPeriod
#define XCURSESSCROLLBARWIDTH   app_data.scrollbarWidth
#define XCURSESGEOMETRY         app_data.geometry

#define XCURSESDISPLAY      (XtDisplay(drawing))
#define XCURSESWIN          (XtWindow(drawing))

#define MAX_COLORS   8  /* maximum of "normal" colours */
#define COLOR_CURSOR 16 /* colour of cursor - 1 more than 2*MAX_COLORS */
#define COLOR_BORDER 17 /* colour of border - 2 more than 2*MAX_COLORS */
#define PDC_NUMBER_APP_RESOURCES 32
#define PDC_NUMBER_OPTIONS 29
#define PDC_NUMBER_XCURSES_ACTIONS 5

#include "x11.h"

extern GC normal_gc,block_cursor_gc,rect_cursor_gc,normal_highlight_gc,bold_highlight_gc,border_gc;
extern int XCursesFontHeight,XCursesFontWidth;
extern int XCursesFontAscent,XCursesFontDescent;
extern int XCursesWindowWidth,XCursesWindowHeight;
extern int resizeXCursesWindowWidth,resizeXCursesWindowHeight;
extern char *bitmap_file;
extern MOUSE_STATUS Trapped_Mouse_status;
extern unsigned long pdc_key_modifier;
extern KeySym compose_key;
extern int compose_mask;
extern int state_mask[8];
extern Atom wm_atom[2];
extern XtTranslations XCursesTranslations;
extern char *defaultTranslations;
extern XtActionsRec XCursesActions[PDC_NUMBER_XCURSES_ACTIONS];
extern char *XCursesClassName;
extern XtAppContext app_context;
extern Widget topLevel,drawing,d1,scrollBox,scrollVert,scrollHoriz;
extern int ReceivedMapNotify;
extern Boolean mouse_selection;
extern char *tmpsel;
extern unsigned long tmpsel_length;
extern int selection_start_x;
extern int selection_start_y;
extern int selection_end_x;
extern int selection_end_y;
extern Pixmap icon_pixmap;
extern XtResource app_resources[PDC_NUMBER_APP_RESOURCES];
extern XrmOptionDescRec options[PDC_NUMBER_OPTIONS];
extern char global_display_name[100];
extern Bool after_first_curses_request;
/* extern char *XCursesProgramName; */
extern int colors[(2*MAX_COLORS)+2];
extern int windowEntered;

typedef RETSIGTYPE (*signal_handler)();
#ifdef PROTO
void XCursesNonmaskable(Widget w,XtPointer client_data,XEvent *event,Boolean *continue_to_dispatch);
void XCursesExpose(Widget w,XtPointer client_data,XEvent *event,Boolean *continue_to_dispatch);
signal_handler XCursesSetSignal(int,signal_handler);
void XCursesGetIcon(void);
int XCursesRefreshScrollbar(void);
int XCursesSendKeyToCurses(unsigned long, MOUSE_STATUS *);
void XCursesButton(Widget,XEvent *,String *,Cardinal *);
void Scroll_up_down(Widget w, XtPointer client_data, XtPointer call_data);
void Scroll_left_right(Widget w, XtPointer client_data, XtPointer call_data);
void Thumb_up_down(Widget w, XtPointer client_data, XtPointer call_data);
void Thumb_left_right(Widget w, XtPointer client_data, XtPointer call_data);
#else
void XCursesNonmaskable();
void XCursesExpose();
signal_handler XCursesSetSignal();
void XCursesGetIcon();
int XCursesRefreshScrollbar();
int XCursesSendKeyToCurses();
void XCursesButton();
void Scroll_up_down();
void Scroll_left_right();
void Thumb_up_down();
void Thumb_left_right();
#endif
