/* PDCurses */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <curspriv.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <ctype.h>

#include <sys/types.h>

#include <Intrinsic.h>
#include <StringDefs.h>
#include <Shell.h>

#include <Xatom.h>

#define XCURSESDISPLAY (XtDisplay(drawing))
#define XCURSESWIN     (XtWindow(drawing))

typedef struct
{
    int lines;
    int cols;
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
    XFontStruct *normalFont;
    XFontStruct *italicFont;
    XFontStruct *boldFont;
    char *bitmap;
    char *pixmap;
    Cursor pointer;
    int clickPeriod;
    int doubleClickPeriod;
    int scrollbarWidth;
    int cursorBlinkRate;
    char *textCursor;
    int textBlinkRate;
} XCursesAppData;

extern Pixel colors[PDC_MAXCOL];

extern XCursesAppData xc_app_data;
extern XtAppContext app_context;
extern Widget topLevel, drawing;

extern GC normal_gc, rect_cursor_gc, italic_gc, bold_gc;
extern int font_height, font_width, font_ascent, font_descent;
extern int window_width, window_height;
extern bool blinked_off;

extern int resize_window_width, resize_window_height;
extern bool window_entered;
extern bool xc_resize_now;

extern XIC Xic;
extern bool vertical_cursor;
extern bool visible_cursor;

int PDC_display_cursor(int, int, int, int, int);

void XC_blink_cursor(XtPointer, XtIntervalId *);
void XC_blink_text(XtPointer, XtIntervalId *);
int XC_kb_setup(void);
void XC_redraw_cursor(void);
bool XC_scrollbar_init(const char *);
