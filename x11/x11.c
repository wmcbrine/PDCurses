/* PDCurses */

#include "pdcx11.h"

#include <xpm.h>

#include <stdlib.h>
#include <string.h>

#ifndef MAX_PATH
# define MAX_PATH 256
#endif

XCursesAppData xc_app_data;

/* Default icons for XCurses applications.  */

#include "../common/icon64.xpm"
#include "../common/icon32.xpm"

GC normal_gc, rect_cursor_gc, italic_gc, bold_gc;
int window_width, window_height;
int resize_window_width = 0, resize_window_height = 0;

XtAppContext app_context;
Widget topLevel, drawing;

static Atom wm_atom[2];
static String class_name = "XCurses";
static int received_map_notify = 0;
static Pixmap icon_pixmap;
static Pixmap icon_pixmap_mask;

bool window_entered = TRUE;

static bool exposed = FALSE;

bool blinked_off;

int XCursesLINES = 24;
int XCursesCOLS = 80;
int font_height, font_width, font_ascent, font_descent;

bool xc_resize_now = FALSE;

/* Macros just for app_resources */

#ifdef PDC_WIDE
# define DEFNFONT "-misc-fixed-medium-r-normal--20-200-75-75-c-100-iso10646-1"
# define DEFIFONT "-misc-fixed-medium-o-normal--20-200-75-75-c-100-iso10646-1"
# define DEFBFONT "-misc-fixed-bold-r-normal--20-200-75-75-c-100-iso10646-1"
#else
# define DEFNFONT "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1"
# define DEFIFONT "-misc-fixed-medium-o-normal--13-120-75-75-c-70-iso8859-1"
# define DEFBFONT "-misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-1"
#endif

#define APPDATAOFF(n) XtOffsetOf(XCursesAppData, n)

#define RINT(name1, name2, value) { \
                #name1, #name2, XtRInt, \
                sizeof(int), APPDATAOFF(name1), XtRImmediate, \
                (XtPointer)value \
        }

#define RPIXEL(name1, name2, value) { \
                #name1, #name2, XtRPixel, \
                sizeof(Pixel), APPDATAOFF(name1), XtRString, \
                (XtPointer)#value \
        }

#define RCOLOR(name, value) RPIXEL(color##name, Color##name, value)


#define RSTRINGP(name1, name2, param) { \
                #name1, #name2, XtRString, \
                MAX_PATH, APPDATAOFF(name1), XtRString, (XtPointer)param \
        }

#define RSTRING(name1, name2) RSTRINGP(name1, name2, "")

#define RFONT(name1, name2, value) { \
                #name1, #name2, XtRFontStruct, \
                sizeof(XFontStruct), APPDATAOFF(name1), XtRString, \
                (XtPointer)value \
        }

#define RCURSOR(name1, name2, value) { \
                #name1, #name2, XtRCursor, \
                sizeof(Cursor), APPDATAOFF(name1), XtRString, \
                (XtPointer)#value \
        }

static XtResource app_resources[] =
{
    RINT(lines, Lines, 24),
    RINT(cols, Cols, 80),

    RPIXEL(cursorColor, CursorColor, Red),

    RCOLOR(Black, Black),
    RCOLOR(Red, red3),
    RCOLOR(Green, green3),
    RCOLOR(Yellow, yellow3),
    RCOLOR(Blue, blue3),
    RCOLOR(Magenta, magenta3),
    RCOLOR(Cyan, cyan3),
    RCOLOR(White, Grey),

    RCOLOR(BoldBlack, grey40),
    RCOLOR(BoldRed, red1),
    RCOLOR(BoldGreen, green1),
    RCOLOR(BoldYellow, yellow1),
    RCOLOR(BoldBlue, blue1),
    RCOLOR(BoldMagenta, magenta1),
    RCOLOR(BoldCyan, cyan1),
    RCOLOR(BoldWhite, White),

    RFONT(normalFont, NormalFont, DEFNFONT),
    RFONT(italicFont, ItalicFont, DEFIFONT),
    RFONT(boldFont, BoldFont, DEFBFONT),

    RSTRING(bitmap, Bitmap),
    RSTRING(pixmap, Pixmap),
    RCURSOR(pointer, Pointer, xterm),

    RPIXEL(pointerForeColor, PointerForeColor, Black),
    RPIXEL(pointerBackColor, PointerBackColor, White),

    RINT(doubleClickPeriod, DoubleClickPeriod, (PDC_CLICK_PERIOD * 2)),
    RINT(clickPeriod, ClickPeriod, PDC_CLICK_PERIOD),
    RINT(scrollbarWidth, ScrollbarWidth, 15),
    RINT(cursorBlinkRate, CursorBlinkRate, 0),

    RSTRING(textCursor, TextCursor),
    RINT(textBlinkRate, TextBlinkRate, 500)
};

#undef RCURSOR
#undef RFONT
#undef RSTRING
#undef RCOLOR
#undef RPIXEL
#undef RINT
#undef APPDATAOFF
#undef DEFBFONT
#undef DEFIFONT
#undef DEFNFONT

/* Macros for options */

#define COPT(name) {"-" #name, "*" #name, XrmoptionSepArg, NULL}
#define CCOLOR(name) COPT(color##name)

static XrmOptionDescRec options[] =
{
    COPT(lines), COPT(cols), COPT(normalFont), COPT(italicFont),
    COPT(boldFont), COPT(bitmap),
    COPT(pixmap),
    COPT(pointer), COPT(clickPeriod),
    COPT(doubleClickPeriod), COPT(scrollbarWidth),
    COPT(pointerForeColor), COPT(pointerBackColor),
    COPT(cursorBlinkRate), COPT(cursorColor), COPT(textCursor),
    COPT(textBlinkRate),

    CCOLOR(Black), CCOLOR(Red), CCOLOR(Green), CCOLOR(Yellow),
    CCOLOR(Blue), CCOLOR(Magenta), CCOLOR(Cyan), CCOLOR(White),

    CCOLOR(BoldBlack), CCOLOR(BoldRed), CCOLOR(BoldGreen),
    CCOLOR(BoldYellow), CCOLOR(BoldBlue), CCOLOR(BoldMagenta),
    CCOLOR(BoldCyan), CCOLOR(BoldWhite)
};

#undef CCOLOR
#undef COPT

static void _get_gc(GC *gc, XFontStruct *font_info, int fore, int back)
{
    XGCValues values;

    /* Create default Graphics Context */

    *gc = XCreateGC(XCURSESDISPLAY, XCURSESWIN, 0L, &values);

    /* specify font */

    XSetFont(XCURSESDISPLAY, *gc, font_info->fid);

    XSetForeground(XCURSESDISPLAY, *gc, colors[fore]);
    XSetBackground(XCURSESDISPLAY, *gc, colors[back]);
}

static void _get_icon(void)
{
    Status rc;

    PDC_LOG(("_get_icon() - called\n"));

    if (xc_app_data.pixmap && xc_app_data.pixmap[0]) /* supplied pixmap */
    {
        XpmReadFileToPixmap(XtDisplay(topLevel),
                            RootWindowOfScreen(XtScreen(topLevel)),
                            (char *)xc_app_data.pixmap,
                            &icon_pixmap, &icon_pixmap_mask, NULL);
    }
    else if (xc_app_data.bitmap && xc_app_data.bitmap[0]) /* supplied bitmap */
    {
        unsigned file_bitmap_width = 0, file_bitmap_height = 0;
        int x_hot = 0, y_hot = 0;

        rc = XReadBitmapFile(XtDisplay(topLevel),
                             RootWindowOfScreen(XtScreen(topLevel)),
                             (char *)xc_app_data.bitmap,
                             &file_bitmap_width, &file_bitmap_height,
                             &icon_pixmap, &x_hot, &y_hot);

        if (BitmapOpenFailed == rc)
            fprintf(stderr, "bitmap file %s: not found\n",
                    xc_app_data.bitmap);
        else if (BitmapFileInvalid == rc)
            fprintf(stderr, "bitmap file %s: contents invalid\n",
                    xc_app_data.bitmap);
    }
    else
    {
        XIconSize *icon_size;
        int size_count = 0, max_height = 0, max_width = 0;

        icon_size = XAllocIconSize();

        rc = XGetIconSizes(XtDisplay(topLevel),
                           RootWindowOfScreen(XtScreen(topLevel)),
                           &icon_size, &size_count);

        /* if the WM can advise on icon sizes... */

        if (rc && size_count)
        {
            int i;

            PDC_LOG(("size_count: %d rc: %d\n", size_count, rc));

            for (i = 0; i < size_count; i++)
            {
                if (icon_size[i].max_width > max_width)
                    max_width = icon_size[i].max_width;
                if (icon_size[i].max_height > max_height)
                    max_height = icon_size[i].max_height;

                PDC_LOG(("min: %d %d\n",
                         icon_size[i].min_width, icon_size[i].min_height));

                PDC_LOG(("max: %d %d\n",
                         icon_size[i].max_width, icon_size[i].max_height));

                PDC_LOG(("inc: %d %d\n",
                         icon_size[i].width_inc, icon_size[i].height_inc));
            }
        }

        XFree(icon_size);

        XpmCreatePixmapFromData(XtDisplay(topLevel),
              RootWindowOfScreen(XtScreen(topLevel)),
              (max_width >= 64 && max_height >= 64) ? icon64 : icon32,
              &icon_pixmap, &icon_pixmap_mask, NULL);
    }
}

/* Redraw the entire screen */

static void _display_screen(void)
{
    int row;

    PDC_LOG(("_display_screen() - called\n"));

    if (!curscr)
        return;

    for (row = 0; row < XCursesLINES; row++)
        PDC_transform_line(row, 0, COLS, curscr->_y[row]);

    XC_redraw_cursor();
}

static void _handle_expose(Widget w, XtPointer client_data, XEvent *event,
                           Boolean *unused)
{
    PDC_LOG(("_handle_expose() - called\n"));

    /* ignore all Exposes except last */

    if (event->xexpose.count)
        return;

    exposed = TRUE;

    if (received_map_notify)
        _display_screen();
}

static void _handle_nonmaskable(Widget w, XtPointer client_data, XEvent *event,
                                Boolean *unused)
{
    XClientMessageEvent *client_event = (XClientMessageEvent *)event;

    PDC_LOG(("_handle_nonmaskable called: event %d\n", event->type));

    if (event->type == ClientMessage)
    {
        PDC_LOG(("ClientMessage received\n"));

        /* This code used to include handling of WM_SAVE_YOURSELF, but
           it resulted in continual failure of THE on my Toshiba laptop.
           Removed on 3-3-2001. Now only exits on WM_DELETE_WINDOW. */

        if ((Atom)client_event->data.s[0] == wm_atom[0])
            XCursesExit();
    }
}

static void _handle_enter_leave(Widget w, XtPointer client_data,
                                XEvent *event, Boolean *unused)
{
    PDC_LOG(("_handle_enter_leave called\n"));

    switch(event->type)
    {
    case EnterNotify:
        PDC_LOG(("EnterNotify received\n"));

        window_entered = TRUE;
        break;

    case LeaveNotify:
        PDC_LOG(("LeaveNotify received\n"));

        window_entered = FALSE;

        /* Display the cursor so it stays on while the window is
           not current */

        XC_redraw_cursor();
        break;

    default:
        PDC_LOG(("_handle_enter_leave - unknown event %d\n", event->type));
    }
}

void XCursesExit(void)
{
    if (icon_pixmap)
        XFreePixmap(XCURSESDISPLAY, icon_pixmap);
    if (icon_pixmap_mask)
        XFreePixmap(XCURSESDISPLAY, icon_pixmap_mask);

    XFreeGC(XCURSESDISPLAY, normal_gc);
    XFreeGC(XCURSESDISPLAY, italic_gc);
    XFreeGC(XCURSESDISPLAY, bold_gc);
    XFreeGC(XCURSESDISPLAY, rect_cursor_gc);
    XDestroyIC(Xic);

    _exit(0);
}

static void _handle_structure_notify(Widget w, XtPointer client_data,
                                     XEvent *event, Boolean *unused)
{
    PDC_LOG(("_handle_structure_notify() - called\n"));

    switch(event->type)
    {
    case ConfigureNotify:
        PDC_LOG(("ConfigureNotify received\n"));

        /* Window has been resized, change width and height to send to
           place_text and place_graphics in next Expose. */

        resize_window_width = event->xconfigure.width;
        resize_window_height = event->xconfigure.height;

        SP->resized = TRUE;
        xc_resize_now = TRUE;
        break;

    case MapNotify:
        PDC_LOG(("MapNotify received\n"));

        received_map_notify = 1;

        break;

    default:
        PDC_LOG(("_handle_structure_notify - unknown event %d\n",
                 event->type));
    }
}

static void _pointer_setup(void)
{
    XColor pointerforecolor, pointerbackcolor;
    XrmValue rmfrom, rmto;

    XDefineCursor(XCURSESDISPLAY, XCURSESWIN, xc_app_data.pointer);
    rmfrom.size = sizeof(Pixel);
    rmto.size = sizeof(XColor);

    rmto.addr = (XPointer)&pointerforecolor;
    rmfrom.addr = (XPointer)&(xc_app_data.pointerForeColor);
    XtConvertAndStore(drawing, XtRPixel, &rmfrom, XtRColor, &rmto);

    rmfrom.size = sizeof(Pixel);
    rmto.size = sizeof(XColor);

    rmfrom.addr = (XPointer)&(xc_app_data.pointerBackColor);
    rmto.addr = (XPointer)&pointerbackcolor;
    XtConvertAndStore(drawing, XtRPixel, &rmfrom, XtRColor, &rmto);

    XRecolorCursor(XCURSESDISPLAY, xc_app_data.pointer,
                   &pointerforecolor, &pointerbackcolor);
}

int XCursesInitscr(int argc, char *argv[])
{
    char *myargv[] = {"PDCurses", NULL};
    extern bool sb_started;

    bool italic_font_valid, bold_font_valid;
    int i = 0;
    int minwidth, minheight;

    PDC_LOG(("XCursesInitscr() - called\n"));

    if (!argv)
    {
        argv = myargv;
        argc = 1;
    }

    /* Start defining X Toolkit things */

#if XtSpecificationRelease > 4
    XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
#endif

    /* Exit if no DISPLAY variable set */

    if (!getenv("DISPLAY"))
    {
        fprintf(stderr, "Error: no DISPLAY variable set\n");
        return ERR;
    }

    /* Initialise the top level widget */

    topLevel = XtVaAppInitialize(&app_context, class_name, options,
                                 XtNumber(options), &argc, argv, NULL, NULL);

    XtVaGetApplicationResources(topLevel, &xc_app_data, app_resources,
                                XtNumber(app_resources), NULL);

    /* Check application resource values here */

    font_width = xc_app_data.normalFont->max_bounds.rbearing -
                 xc_app_data.normalFont->min_bounds.lbearing;

    font_ascent = xc_app_data.normalFont->max_bounds.ascent;
    font_descent = xc_app_data.normalFont->max_bounds.descent;
    font_height = font_ascent + font_descent;

    /* Check that the italic font and normal fonts are the same size */

    italic_font_valid = font_width ==
        xc_app_data.italicFont->max_bounds.rbearing -
        xc_app_data.italicFont->min_bounds.lbearing;

    bold_font_valid = font_width ==
        xc_app_data.boldFont->max_bounds.rbearing -
        xc_app_data.boldFont->min_bounds.lbearing;

    /* Calculate size of display window */

    XCursesCOLS = xc_app_data.cols;
    XCursesLINES = xc_app_data.lines;

    window_width = font_width * XCursesCOLS;
    window_height = font_height * XCursesLINES;

    minwidth = font_width * 2;
    minheight = font_height * 2;

    /* Set up the icon for the application; the default is an internal
       one for PDCurses. Then set various application level resources. */

    _get_icon();

    XtVaSetValues(topLevel, XtNminWidth, minwidth, XtNminHeight,
                  minheight, XtNbaseWidth, 0, XtNbaseHeight, 0,
                  XtNbackground, 0, XtNiconPixmap, icon_pixmap,
                  XtNiconMask, icon_pixmap_mask, NULL);

    /* Create a widget in which to draw */

    if (!XC_scrollbar_init(argv[0]))
    {
        drawing = topLevel;

        XtVaSetValues(topLevel, XtNwidth, window_width, XtNheight,
            window_height, XtNwidthInc, font_width, XtNheightInc,
            font_height, NULL);
    }

    /* Determine text cursor alignment from resources */

    if (!strcmp(xc_app_data.textCursor, "vertical"))
        vertical_cursor = TRUE;

    /* Now have LINES and COLS. */

    LINES = XCursesLINES;
    COLS = XCursesCOLS;

    SP = calloc(1, sizeof(SCREEN));

    SP->lines = XCursesLINES;
    SP->cols = XCursesCOLS;

    SP->mouse_wait = xc_app_data.clickPeriod;
    SP->audible = TRUE;

    SP->termattrs = A_COLOR | A_ITALIC | A_UNDERLINE | A_LEFT | A_RIGHT |
                    A_REVERSE;

    /* Add Event handlers to the drawing widget */

    XtAddEventHandler(drawing, ExposureMask, False, _handle_expose, NULL);
    XtAddEventHandler(drawing, StructureNotifyMask, False,
                      _handle_structure_notify, NULL);
    XtAddEventHandler(drawing, EnterWindowMask | LeaveWindowMask, False,
                      _handle_enter_leave, NULL);
    XtAddEventHandler(topLevel, 0, True, _handle_nonmaskable, NULL);

    /* If there is a cursorBlink resource, start the Timeout event */

    if (xc_app_data.cursorBlinkRate)
        XtAppAddTimeOut(app_context, xc_app_data.cursorBlinkRate,
                        XC_blink_cursor, NULL);

    XtRealizeWidget(topLevel);

    /* Handle trapping of the WM_DELETE_WINDOW property */

    wm_atom[0] = XInternAtom(XtDisplay(topLevel), "WM_DELETE_WINDOW", False);

    XSetWMProtocols(XtDisplay(topLevel), XtWindow(topLevel), wm_atom, 1);

    /* Create the Graphics Context for drawing. This MUST be done AFTER
       the associated widget has been realized. */

    PDC_LOG(("before _get_gc\n"));

    _get_gc(&normal_gc, xc_app_data.normalFont, COLOR_WHITE, COLOR_BLACK);

    _get_gc(&italic_gc, italic_font_valid ? xc_app_data.italicFont :
            xc_app_data.normalFont, COLOR_WHITE, COLOR_BLACK);

    _get_gc(&bold_gc, bold_font_valid ? xc_app_data.boldFont :
            xc_app_data.normalFont, COLOR_WHITE, COLOR_BLACK);

    _get_gc(&rect_cursor_gc, xc_app_data.normalFont,
            COLOR_CURSOR, COLOR_BLACK);

    XSetLineAttributes(XCURSESDISPLAY, rect_cursor_gc, 2,
                       LineSolid, CapButt, JoinMiter);

    /* Set the pointer for the application */

    _pointer_setup();

    if (ERR == XC_kb_setup())
        return ERR;

    while (!exposed)
    {
        XEvent event;

        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    }

    XCursesLINES = SP->lines;
    LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;
    XCursesCOLS = COLS = SP->cols;

    atexit(XCursesExit);

    return OK;
}
