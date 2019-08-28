/* PDCurses */

#include "pdcx11.h"

#include <string.h>

#ifdef PDC_WIDE
# include "../common/acsuni.h"
#else

chtype acs_map[128] =
{
    PDC_ACS(0), PDC_ACS(1), PDC_ACS(2), PDC_ACS(3), PDC_ACS(4),
    PDC_ACS(5), PDC_ACS(6), PDC_ACS(7), PDC_ACS(8), PDC_ACS(9),
    PDC_ACS(10), PDC_ACS(11), PDC_ACS(12), PDC_ACS(13), PDC_ACS(14),
    PDC_ACS(15), PDC_ACS(16), PDC_ACS(17), PDC_ACS(18), PDC_ACS(19),
    PDC_ACS(20), PDC_ACS(21), PDC_ACS(22), PDC_ACS(23), PDC_ACS(24),
    PDC_ACS(25), PDC_ACS(26), PDC_ACS(27), PDC_ACS(28), PDC_ACS(29),
    PDC_ACS(30), PDC_ACS(31), ' ', '!', '"', '#', '$', '%', '&', '\'',
    '(', ')', '*',

    '>', '<', '^', 'v',

    '/',

    PDC_ACS(0),

    '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

    PDC_ACS(1), PDC_ACS(2),

    'b', 'c', 'd', 'e',

    PDC_ACS(7), PDC_ACS(8), '#', 0xa4, PDC_ACS(11), PDC_ACS(12),
    PDC_ACS(13), PDC_ACS(14), PDC_ACS(15), PDC_ACS(16), PDC_ACS(17),
    PDC_ACS(18), PDC_ACS(19), PDC_ACS(20), PDC_ACS(21), PDC_ACS(22),
    PDC_ACS(23), PDC_ACS(24), PDC_ACS(25), PDC_ACS(26), PDC_ACS(27),
    PDC_ACS(28), PDC_ACS(29), PDC_ACS(30), 0xb7,

    PDC_ACS(127)
};

#endif

bool visible_cursor = FALSE;
bool vertical_cursor = FALSE;

/* Convert character positions x and y to pixel positions, stored in
   xpos and ypos */

static void _make_xy(int x, int y, int *xpos, int *ypos)
{
    *xpos = (x * font_width) + xc_app_data.borderWidth;
    *ypos = xc_app_data.normalFont->ascent + (y * font_height) +
            xc_app_data.borderWidth;
}

static void _set_cursor_color(chtype *ch, short *fore, short *back)
{
    int attr;
    short f, b;

    attr = PAIR_NUMBER(*ch);

    if (attr)
    {
        PDC_pair_content(attr, &f, &b);
        *fore = 7 - (f % 8);
        *back = 7 - (b % 8);
    }
    else
    {
        if (*ch & A_REVERSE)
        {
            *back = COLOR_BLACK;
            *fore = COLOR_WHITE;
        }
        else
        {
            *back = COLOR_WHITE;
            *fore = COLOR_BLACK;
        }
    }
}

static void _display_cursor(int old_row, int old_x, int new_row, int new_x)
{
    int xpos, ypos, i;
    chtype *ch;
    short fore = 0, back = 0;

    PDC_LOG(("_display_cursor() - draw char at row: %d col %d\n",
             old_row, old_x));

    /* if the cursor position is outside the boundary of the screen,
       ignore the request */

    if (old_row >= XCursesLINES || old_x >= COLS ||
        new_row >= XCursesLINES || new_x >= COLS)
        return;

    /* display the character at the current cursor position */

    PDC_LOG(("_display_cursor() - draw char at row: %d col %d\n",
             old_row, old_x));

    PDC_transform_line(old_row, old_x, 1, curscr->_y[old_row] + old_x);

    /* display the cursor at the new cursor position */

    if (!SP->visibility)
        return;     /* cursor not displayed, no more to do */

    _make_xy(new_x, new_row, &xpos, &ypos);

    ch = curscr->_y[new_row] + new_x;
    _set_cursor_color(ch, &fore, &back);

    if (vertical_cursor)
    {
        XSetForeground(XCURSESDISPLAY, rect_cursor_gc, colors[back]);

        for (i = 1; i <= SP->visibility; i++)
            XDrawLine(XCURSESDISPLAY, XCURSESWIN, rect_cursor_gc,
                      xpos + i, ypos - xc_app_data.normalFont->ascent,
                      xpos + i, ypos - xc_app_data.normalFont->ascent +
                      font_height - 1);
    }
    else
    {
        /* For block cursors, paint the block with invert. */

        int yp, yh;

        if (SP->visibility == 2)
        {
            yp = ypos - font_height + font_descent;
            yh = font_height;
        }
        else
        {
            yp = ypos - font_height / 4 + font_descent;
            yh = font_height / 4;
        }

        XSetFunction(XCURSESDISPLAY, rect_cursor_gc, GXinvert);
        XFillRectangle(XCURSESDISPLAY, XCURSESWIN, rect_cursor_gc,
            xpos, yp, font_width, yh);
    }

    PDC_LOG(("_display_cursor() - draw cursor at row %d col %d\n",
             new_row, new_x));
}

void XC_redraw_cursor(void)
{
    _display_cursor(SP->cursrow, SP->curscol, SP->cursrow, SP->curscol);
}

void XC_blink_cursor(XtPointer unused, XtIntervalId *id)
{
    PDC_LOG(("XC_blink_cursor() - called:\n"));

    XCursesDisplayCursor();
    XtAppAddTimeOut(app_context, xc_app_data.cursorBlinkRate,
                    XC_blink_cursor, NULL);
}

void XC_blink_text(XtPointer unused, XtIntervalId *id)
{
    int row;
    int j, k;
    chtype *ch;

    PDC_LOG(("XC_blink_text() - called:\n"));

    blinked_off = !blinked_off;

    /* Redraw changed lines on the screen to match the blink state */

    for (row = 0; row < XCursesLINES; row++)
    {
        ch = curscr->_y[row];

        for (j = 0; j < COLS; j++)
            if (ch[j] & A_BLINK)
            {
                k = j;
                while (ch[k] & A_BLINK && k < COLS)
                    k++;

                PDC_transform_line(row, j, k - j, ch + j);

                j = k;
            }
    }

    XC_redraw_cursor();
    XC_draw_border();

    if ((SP->termattrs & A_BLINK) || !blinked_off)
        XtAppAddTimeOut(app_context, xc_app_data.textBlinkRate,
                        XC_blink_text, NULL);
}

void XCursesCursor(int old_row, int old_x, int new_row, int new_x)
{
    PDC_LOG(("XCursesCursor - called\n"));

    visible_cursor = TRUE;
    _display_cursor(old_row, old_x, new_row, new_x);
}

void XCursesDisplayCursor(void)
{
    PDC_LOG(("XCursesDisplayCursor - called. Vis now: "));
    PDC_LOG((visible_cursor ? "1\n" : "0\n"));

    /* If the window is not active, ignore this command. The
       cursor will stay solid. */

    if (window_entered)
    {
        if (visible_cursor)
        {
            /* Cursor currently ON, turn it off */

            int save_visibility = SP->visibility;
            SP->visibility = 0;
            XC_redraw_cursor();
            SP->visibility = save_visibility;
            visible_cursor = FALSE;
        }
        else
        {
            /* Cursor currently OFF, turn it on */

            XC_redraw_cursor();
            visible_cursor = TRUE;
        }
    }
}

int PDC_display_cursor(int oldrow, int oldcol, int newrow, int newcol,
                       int visibility)
{
    PDC_LOG(("PDC_display_cursor() - called: NEW row %d col %d, vis %d\n",
             newrow, newcol, visibility));

    if (visibility == -1)
        XCursesDisplayCursor();
    else
        XCursesCursor(oldrow, oldcol, newrow, newcol);

    return OK;
}

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

    PDC_display_cursor(SP->cursrow, SP->curscol, row, col, SP->visibility);
}

/* update the given physical line to look like the corresponding line in
   curscr */

/* Output a block of characters with common attributes */

static int _new_packet(chtype attr, int len, int col, int row,
#ifdef PDC_WIDE
                       XChar2b *text)
#else
                       char *text)
#endif
{
    XRectangle bounds;
    GC gc;
    int xpos, ypos;
    short fore, back;
    attr_t sysattrs;
    bool rev;

    PDC_pair_content(PAIR_NUMBER(attr), &fore, &back);

    /* Specify the color table offsets */

    sysattrs = SP->termattrs;

    if ((attr & A_BOLD) && !(sysattrs & A_BOLD))
        fore |= 8;
    if ((attr & A_BLINK) && !(sysattrs & A_BLINK))
        back |= 8;

    rev = !!(attr & A_REVERSE);

    /* Determine which GC to use - normal, italic or bold */

    if ((attr & A_ITALIC) && (sysattrs & A_ITALIC))
        gc = italic_gc;
    else if ((attr & A_BOLD) && (sysattrs & A_BOLD))
        gc = bold_gc;
    else
        gc = normal_gc;

    _make_xy(col, row, &xpos, &ypos);

    bounds.x = xpos;
    bounds.y = ypos - font_ascent;
    bounds.width = font_width * len;
    bounds.height = font_height;

    XSetClipRectangles(XCURSESDISPLAY, gc, 0, 0, &bounds, 1, Unsorted);

    if (blinked_off && (sysattrs & A_BLINK) && (attr & A_BLINK))
    {
        XSetForeground(XCURSESDISPLAY, gc, colors[rev ? fore : back]);
        XFillRectangle(XCURSESDISPLAY, XCURSESWIN, gc, xpos, bounds.y,
                       bounds.width, font_height);
    }
    else
    {
        /* Draw it */

        XSetForeground(XCURSESDISPLAY, gc, colors[rev ? back : fore]);
        XSetBackground(XCURSESDISPLAY, gc, colors[rev ? fore : back]);

#ifdef PDC_WIDE
        XDrawImageString16(
#else
        XDrawImageString(
#endif
            XCURSESDISPLAY, XCURSESWIN, gc, xpos, ypos, text, len);

        /* Underline, etc. */

        if (attr & (A_LEFT | A_RIGHT | A_UNDERLINE))
        {
            int k;

            if (SP->line_color != -1)
                XSetForeground(XCURSESDISPLAY, gc, colors[SP->line_color]);

            if (attr & A_UNDERLINE)
                XDrawLine(XCURSESDISPLAY, XCURSESWIN, gc,
                          xpos, ypos + 1, xpos + font_width * len, ypos + 1);

            if (attr & A_LEFT)
                for (k = 0; k < len; k++)
                {
                    int x = xpos + font_width * k;
                    XDrawLine(XCURSESDISPLAY, XCURSESWIN, gc,
                              x, ypos - font_ascent, x, ypos + font_descent);
                }

            if (attr & A_RIGHT)
                for (k = 0; k < len; k++)
                {
                    int x = xpos + font_width * (k + 1) - 1;
                    XDrawLine(XCURSESDISPLAY, XCURSESWIN, gc,
                              x, ypos - font_ascent, x, ypos + font_descent);
                }
        }
    }

    PDC_LOG(("_new_packet() - row: %d col: %d "
             "num_cols: %d fore: %d back: %d text:<%s>\n",
             row, col, len, fore, back, text));

    return OK;
}

/* The core display routine -- update one line of text */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
#ifdef PDC_WIDE
    XChar2b text[513];
#else
    char text[513];
#endif
    chtype old_attr, attr;
    int i, j;

    PDC_LOG(("PDC_transform_line() - called: lineno: %d x: %d "
             "len: %d\n", lineno, x, len));

    if (!len)
        return;

    old_attr = *srcp & A_ATTRIBUTES;

    for (i = 0, j = 0; j < len; j++)
    {
        chtype curr = srcp[j];

        attr = curr & A_ATTRIBUTES;

        if (attr & A_ALTCHARSET && !(curr & 0xff80))
        {
            attr ^= A_ALTCHARSET;
            curr = acs_map[curr & 0x7f];
        }

#ifndef PDC_WIDE
        /* Special handling for ACS_BLOCK */

        if (!(curr & A_CHARTEXT))
        {
            curr |= ' ';
            attr ^= A_REVERSE;
        }
#endif
        if (attr != old_attr)
        {
            if (_new_packet(old_attr, i, x, lineno, text) == ERR)
                return;

            old_attr = attr;
            x += i;
            i = 0;
        }

#ifdef PDC_WIDE
        text[i].byte1 = (curr & 0xff00) >> 8;
        text[i++].byte2 = curr & 0x00ff;
#else
        text[i++] = curr & 0xff;
#endif
    }

    _new_packet(old_attr, i, x, lineno, text);
}
