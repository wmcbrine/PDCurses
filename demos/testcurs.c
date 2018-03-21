/*
 * This is a test program for PDCurses. Originally by
 * John Burnell <johnb@kea.am.dsir.govt.nz>
 *
 *  wrs(5/28/93) -- modified to be consistent (perform identically)
 *                  with either PDCurses or under Unix System V, R4
 */

#ifndef _XOPEN_SOURCE_EXTENDED
# define _XOPEN_SOURCE_EXTENDED 1
#endif

#ifdef PDC_WIDE
   #define HAVE_WIDE 1
   #include <wchar.h>
   #include <curses.h>
#endif
#ifdef HAVE_NCURSESW
   #define HAVE_WIDE 1
   #include <wchar.h>
   #include <ncursesw/curses.h>
#endif

#ifndef HAVE_WIDE
   #include <curses.h>
   #define HAVE_WIDE 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>

#if defined( PDCURSES)
   #define getmouse nc_getmouse
#else
   #define NCURSES_MOUSE_INTERFACE
#endif


#if defined(PDCURSES) && !defined(XCURSES)
# define HAVE_RESIZE 1
#else
# define HAVE_RESIZE 0
#endif

#ifdef A_COLOR
# define HAVE_COLOR 1
#else
# define HAVE_COLOR 0
#endif

/* Set to non-zero if you want to test the PDCurses clipboard */
/* (obviously,  can't do that with ncurses) */

#if defined( PDCURSES)
   #define HAVE_CLIPBOARD 1
#else
   #define HAVE_CLIPBOARD 0
#endif

void inputTest(WINDOW *);
void scrollTest(WINDOW *);
void introTest(WINDOW *);
int initTest(WINDOW **, int, char **);
void outputTest(WINDOW *);
void padTest(WINDOW *);
void acsTest(WINDOW *);

#if HAVE_COLOR
void colorTest(WINDOW *);
#endif

#if HAVE_RESIZE
void resizeTest(WINDOW *);
#endif

#if HAVE_CLIPBOARD
void clipboardTest(WINDOW *);
#endif

#if HAVE_WIDE
void wideTest(WINDOW *);
#endif

void display_menu(int, int);

struct commands
{
    const char *text;
    void (*function)(WINDOW *);
};

typedef struct commands COMMAND;

#define MAX_OPTIONS (6 + HAVE_COLOR + HAVE_RESIZE + HAVE_CLIPBOARD + HAVE_WIDE)

COMMAND command[MAX_OPTIONS] =
{
    {"Intro Test", introTest},
    {"Pad Test", padTest},
#if HAVE_RESIZE
    {"Resize Test", resizeTest},
#endif
    {"Scroll Test", scrollTest},
    {"Input Test", inputTest},
    {"Output Test", outputTest},
    {"ACS Test", acsTest},
#if HAVE_COLOR
    {"Color Test", colorTest},
#endif
#if HAVE_CLIPBOARD
    {"Clipboard Test", clipboardTest},
#endif
#if HAVE_WIDE
    {"Wide Input", wideTest}
#endif
};

int width, height;
static short background_index = COLOR_BLACK;
static bool report_mouse_movement = FALSE;

int main(int argc, char *argv[])
{
    WINDOW *win;
    int key, old_option = -1, new_option = 0, i;
    bool quit = FALSE;

    setlocale(LC_ALL, "");

#ifdef PDCURSES 
#ifdef PDC_VER_MAJOR	/* so far only seen in 4.0+ */
    PDC_set_resize_limits( 20, 50, 70, 200);
#endif
#endif

    if (initTest(&win, argc, argv))
        return 1;

    for( i = 1; i < argc; i++)
        if( argv[i][0] == '-')
            switch( argv[i][1])
            {
                case 'l': case 'L':
                    setlocale( LC_ALL, argv[i] + 2);
                    break;
                case 'i': case 'I':
                    background_index = (short)atoi( argv[i] + 2);
                    break;
#ifdef PDCURSES
                case 'b': case 'B':
                    PDC_set_blink( TRUE);
                    break;
                case 'm': case 'M':
                    PDC_return_key_modifiers( TRUE);
                    break;
#ifdef PDC_VER_MAJOR	/* so far only seen in 4.0+ */
                case 'r':     /* allow user-resizable windows */
                    {
                        int min_lines, max_lines, min_cols, max_cols;

                        if( sscanf( argv[i] + 2, "%d,%d,%d,%d",
                                       &min_lines, &max_lines,
                                       &min_cols, &max_cols) == 4)
                            PDC_set_resize_limits( min_lines, max_lines,
                                                   min_cols, max_cols);
                    }
                    break;
#endif
#endif
                case 'z':
                    report_mouse_movement = TRUE;
                    break;
                default:
                    break;
            }

#ifdef A_COLOR
    if (has_colors())
    {
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        wbkgd(win, COLOR_PAIR(1));
    }
    else
#endif
        wbkgd(win, A_REVERSE);

#ifdef PDCURSES
#ifdef PDC_VER_MAJOR	/* so far only seen in 4.0+ */
    PDC_set_function_key( FUNCTION_KEY_ABORT, 3 );  /* ctrl-C aborts */
#endif
#endif

    erase();
    display_menu(old_option, new_option);

    while (1)
    {
        noecho();
        keypad(stdscr, TRUE);
        raw();
        mousemask( ALL_MOUSE_EVENTS, NULL);

        key = getch();

        switch(key)
        {
        case KEY_MOUSE:
            {
               const int tmarg = (LINES - (MAX_OPTIONS + 2)) / 2;
               int selected_opt;
               MEVENT mouse_event;

               getmouse( &mouse_event);

               selected_opt = mouse_event.y - tmarg;
               if( selected_opt >= 0 && selected_opt < MAX_OPTIONS)
               {
                  old_option = new_option;
                  new_option = selected_opt;
                  display_menu( old_option, new_option);
               }
               if( mouse_event.bstate & BUTTON1_DOUBLE_CLICKED)
                  key = 10;
            }
            if( key != 10)
               break;
        case 10:
        case 13:
        case KEY_ENTER:
            old_option = -1;
            erase();
            refresh();
            (*command[new_option].function)(win);
            erase();
            display_menu(old_option, new_option);
            break;

        case KEY_PPAGE:
        case KEY_HOME:
            old_option = new_option;
            new_option = 0;
            display_menu(old_option, new_option);
            break;

        case KEY_NPAGE:
        case KEY_END:
            old_option = new_option;
            new_option = MAX_OPTIONS - 1;
            display_menu(old_option, new_option);
            break;

        case KEY_UP:
            old_option = new_option;
            new_option = (new_option == 0) ?
                new_option : new_option - 1;
            display_menu(old_option, new_option);
            break;

        case KEY_DOWN:
            old_option = new_option;
            new_option = (new_option == MAX_OPTIONS - 1) ?
                new_option : new_option + 1;
            display_menu(old_option, new_option);
            break;
        case KEY_RESIZE:
# ifdef PDCURSES
            resize_term(0, 0);
# endif
            old_option = -1;
            erase();
            display_menu(old_option, new_option);
            break;
        case 'Q':
        case 'q':
            quit = TRUE;
        }

        if (quit == TRUE)
            break;
    }

    delwin(win);
    endwin();

    return 0;
}

void Continue(WINDOW *win)
{
    mvwaddstr(win, 10, 1, " Press any key to continue");
    wrefresh(win);
    raw();
    wgetch(win);
    wrefresh(win);
}

void Continue2(void)
{
    move(LINES - 1, 1);
    clrtoeol();
    mvaddstr(LINES - 2, 1, " Press any key to continue");
    refresh();
    raw();
    getch();
}

int initTest(WINDOW **win, int argc, char *argv[])
{
#ifdef XCURSES
    Xinitscr(argc, argv);
#else
    initscr();
#endif
#ifdef A_COLOR
    if (has_colors())
        start_color();
#endif
    /* Create a drawing window */

    width  = 60;
    height = 19;

    *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);

    if (*win == NULL)
    {
        endwin();
        return 1;
    }

    return 0;
}

void introTest(WINDOW *win)
{
    werase(win);
    wmove(win, height / 2 - 5, width / 2);
    wvline(win, ACS_VLINE, 10);
    wmove(win, height / 2, width / 2 - 10);
    whline(win, ACS_HLINE, 20);
    Continue(win);

    beep();
    werase(win);

    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh(win);

    cbreak();
    mvwaddstr(win, 1, 1,
        "You should have a rectangle in the middle of the screen");
    mvwaddstr(win, 2, 1, "You should have heard a beep");
    Continue(win);

    flash();
    mvwaddstr(win, 3, 1, "You should have seen a flash");
    Continue(win);
}

void scrollTest(WINDOW *win)
{
    int i, OldY;
#ifndef PDCURSES
    int OldX;
#endif
    werase(win);
    mvwaddstr(win, height - 2, 1, "The window will now scroll slowly");
    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh(win);
    scrollok(win, TRUE);
    napms(500);

    for (i = 1; i <= height; i++)
    {
        napms(150);
        scroll(win);
        wrefresh(win);
    };

#ifdef PDCURSES
    OldY = getmaxy(win);
#else
    getmaxyx(win, OldY, OldX);
#endif
    mvwaddstr(win, 6, 1, "The top of the window will scroll");
    wmove(win, 1, 1);
    wsetscrreg(win, 0, 4);
    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh(win);

    for (i = 1; i <= 5; i++)
    {
        napms(500);
        scroll(win);
        wrefresh(win);
    }

    mvwaddstr(win, 3, 1, "The bottom of the window will scroll");
    wmove(win, 8, 1);
    wsetscrreg(win, 5, --OldY);
    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh(win);

    for (i = 5; i <= OldY; i++)
    {
        napms(300);
        wscrl(win, -1);
        wrefresh(win);
    }

    wsetscrreg(win, 0, OldY);
}

void inputTest(WINDOW *win)
{
    int w, h, bx, by, sw, sh, i, c, num = 0;
    int line_to_use = 3;
    char buffer[80];
    WINDOW *subWin;
    static const char spinner[4] = "/-\\|";
    int spinner_count = 0;

    wclear(win);

    getmaxyx(win, h, w);
    getbegyx(win, by, bx);

    sw = w / 3;
    sh = h / 3;

    if ((subWin = subwin(win, sh, sw, by + h - sh - 2, bx + w - sw - 2))
        == NULL)
        return;

#ifdef A_COLOR
    if (has_colors())
    {
        init_pair(2, COLOR_WHITE, COLOR_RED);
        wbkgd(subWin, COLOR_PAIR(2) | A_BOLD);
    }
    else
#endif
        wbkgd(subWin, A_BOLD);

    box(subWin, ACS_VLINE, ACS_HLINE);
    wrefresh(win);

    nocbreak();

    wclear (win);
    mvwaddstr(win, 1, 1,
        "Press keys (or mouse buttons) to show their names");
    mvwaddstr(win, 2, 1, "Press spacebar to finish");
    wrefresh(win);

    keypad(win, TRUE);
    raw();
    noecho();

    wtimeout(win, 200);


    mousemask( ALL_MOUSE_EVENTS |
            (report_mouse_movement ? REPORT_MOUSE_POSITION : 0), NULL);
#ifdef PDCURSES
    PDC_save_key_modifiers(TRUE);
#else
    if( report_mouse_movement)
       printf("\033[?1003h\n");   /* used in ncurses with some X-based */
#endif                         /* terms to enable mouse move events */
    curs_set(0);        /* turn cursor off */

    while (1)
    {
        while (1)
        {
            c = wgetch(win);

            if (c == ERR)
            {
                spinner_count++;
                if (spinner_count == 4)
                    spinner_count = 0;
                mvwaddch(win, line_to_use, 3, spinner[spinner_count]);
                wrefresh(win);
            }
            else
                break;
        }
#ifdef PDCURSES
/*      wmove(win, line_to_use + 1, 18);
        wclrtoeol(win);  */
#endif
        mvwaddstr(win, line_to_use, 5, "Key Pressed: ");
        wclrtoeol(win);

        wprintw( win, "(%x) ", c);
        if( has_key( c))
            wprintw(win, "%s", keyname(c));
        else if (isprint(c) || c > 0xff)
            waddch( win, c);
        else
            wprintw(win, "%s", unctrl(c));
        if (c == KEY_MOUSE)
        {
#ifdef NCURSES_MOUSE_INTERFACE
            const mmask_t masks[ ] = {
                  BUTTON1_RELEASED, BUTTON1_PRESSED, BUTTON1_CLICKED,
                  BUTTON1_DOUBLE_CLICKED, BUTTON1_TRIPLE_CLICKED,
                  BUTTON2_RELEASED, BUTTON2_PRESSED, BUTTON2_CLICKED,
                  BUTTON2_DOUBLE_CLICKED, BUTTON2_TRIPLE_CLICKED,
                  BUTTON3_RELEASED, BUTTON3_PRESSED, BUTTON3_CLICKED,
                  BUTTON3_DOUBLE_CLICKED, BUTTON3_TRIPLE_CLICKED,
                  BUTTON4_RELEASED, BUTTON4_PRESSED, BUTTON4_CLICKED,
                  BUTTON4_DOUBLE_CLICKED, BUTTON4_TRIPLE_CLICKED,
#ifdef BUTTON5_RELEASED
                  BUTTON5_RELEASED, BUTTON5_PRESSED, BUTTON5_CLICKED,
                  BUTTON5_DOUBLE_CLICKED, BUTTON5_TRIPLE_CLICKED,
#endif
                  };
#ifdef BUTTON4_RESERVED_EVENT
            const mmask_t reserved_masks[] = {
                  BUTTON1_RESERVED_EVENT, BUTTON2_RESERVED_EVENT,
                  BUTTON3_RESERVED_EVENT, BUTTON4_RESERVED_EVENT };
#endif
            MEVENT mouse_event;

            getmouse( &mouse_event);
            wmove(win, line_to_use, 5);
            wclrtoeol(win);
            wprintw(win, "Posn: Y: %d X: %d", mouse_event.y, mouse_event.x);
            for( i = 0; i < sizeof( masks) / sizeof( masks[0]); i++)
                if( mouse_event.bstate & masks[i])
                {
                    const char *event_names[] = { "released", "pressed", "clicked",
                            "double-clicked", "triple-clicked" };

                     wprintw( win, " Button %d %s", i / 5 + 1, event_names[i % 5]);
                }
#ifdef BUTTON_CTRL
            if( mouse_event.bstate & BUTTON_CTRL)
                  wprintw( win, " Ctrl");
#endif
            if( mouse_event.bstate & BUTTON_ALT)
                  wprintw( win, " Alt");
            if( mouse_event.bstate & BUTTON_SHIFT)
                  wprintw( win, " Shift");
#ifdef BUTTON4_RESERVED_EVENT
            for( i = 0; i < sizeof( reserved_masks) / sizeof( reserved_masks[0]); i++)
                if( mouse_event.bstate & reserved_masks[i])
                    wprintw( win, " Reserved %d", i + 1);
#endif

#else          /* using the 'classic' (undocumented) Sys V mouse functions */
            int button = 0, status = 0;
            request_mouse_pos();

            if (BUTTON_CHANGED(1))
                button = 1;
            else if (BUTTON_CHANGED(2))
                button = 2;
            else if (BUTTON_CHANGED(3))
                button = 3;
            else if (BUTTON_CHANGED(4))   /* added 21 Jan 2011: BJG */
                button = 4;
            else if (BUTTON_CHANGED(5))
                button = 5;
            if( button)
#ifdef PDC_N_EXTENDED_MOUSE_BUTTONS
                status = (button > 3 ? Mouse_status.xbutton[(button) - 4] :
                                       Mouse_status.button[(button) - 1]);
#else
                status = (button > 3 ? 0 :
                                       Mouse_status.button[(button) - 1]);
#endif

            wmove(win, line_to_use, 5);
            wclrtoeol(win);
            wprintw(win, "Button %d: ", button);

            if (MOUSE_MOVED)
                waddstr(win, "moved: ");
            else if (MOUSE_WHEEL_UP)
                waddstr(win, "wheel up: ");
            else if (MOUSE_WHEEL_DOWN)
                waddstr(win, "wheel dn: ");
#ifdef MOUSE_WHEEL_LEFT
            else if (MOUSE_WHEEL_LEFT)
                waddstr(win, "wheel lt: ");
#endif
#ifdef MOUSE_WHEEL_RIGHT
            else if (MOUSE_WHEEL_RIGHT)
                waddstr(win, "wheel rt: ");
#endif
            else if ((status & BUTTON_ACTION_MASK) == BUTTON_PRESSED)
                waddstr(win, "pressed: ");
            else if ((status & BUTTON_ACTION_MASK) == BUTTON_CLICKED)
                waddstr(win, "clicked: ");
            else if ((status & BUTTON_ACTION_MASK) == BUTTON_DOUBLE_CLICKED)
                waddstr(win, "double: ");
            else if ((status & BUTTON_ACTION_MASK) == BUTTON_TRIPLE_CLICKED)
                waddstr(win, "triple: ");
            else if( button)
                waddstr(win, "released: ");

            wprintw(win, "Posn: Y: %d X: %d", MOUSE_Y_POS, MOUSE_X_POS);
            if (button && (status & BUTTON_MODIFIER_MASK))
            {
                if (status & BUTTON_SHIFT)
                    waddstr(win, " SHIFT");

                if (status & BUTTON_CONTROL)
                    waddstr(win, " CONTROL");

                if (status & BUTTON_ALT)
                    waddstr(win, " ALT");
            }
        }
        else if (PDC_get_key_modifiers())
        {
            waddstr(win, " Modifier(s):");
            if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_SHIFT)
                waddstr(win, " SHIFT");

            if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_CONTROL)
                waddstr(win, " CONTROL");

            if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_ALT)
                waddstr(win, " ALT");

            if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_NUMLOCK)
                waddstr(win, " NUMLOCK");

#ifdef PDC_KEY_MODIFIER_REPEAT
            if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_REPEAT)
                waddstr(win, " REPEAT");
#endif
#endif            /* end of mouse display */
        }
        wrefresh(win);

        if (c == ' ')
            break;
        line_to_use++;
        if( line_to_use == 17)
           line_to_use = 3;
    }

    wtimeout(win, -1);  /* turn off timeout() */
    curs_set(1);        /* turn cursor back on */

#ifdef PDCURSES
    mouse_set(0L);
    PDC_save_key_modifiers(FALSE);
/*  PDC_return_key_modifiers(FALSE);   */
#endif
    wclear(win);
#ifdef PDCURSES
#ifdef PDC_VER_MAJOR	/* so far only seen in 4.0+ */
    PDC_set_function_key( FUNCTION_KEY_ABORT, 0 );  /* un-abortable */
#endif
#endif
    mvwaddstr(win, 2, 1, "Press some keys for 5 seconds");
    mvwaddstr(win, 1, 1, "Pressing ^C should do nothing");
    wrefresh(win);

    werase(subWin);
    box(subWin, ACS_VLINE, ACS_HLINE);

    for (i = 0; i < 5; i++)
    {
        mvwprintw(subWin, 1, 1, "Time = %d", i);
        wrefresh(subWin);
        napms(1000);
        flushinp();
    }

#ifdef PDCURSES
#ifdef PDC_VER_MAJOR	/* so far only seen in 4.0+ */
    PDC_set_function_key( FUNCTION_KEY_ABORT, 3 );  /* ctrl-C aborts */
#endif
#endif

    delwin(subWin);
    werase(win);
    flash();
    wrefresh(win);
    napms(500);
    flushinp();

    mvwaddstr(win, 2, 1, "Press a key, followed by ENTER");
    wmove(win, 9, 10);
    wrefresh(win);
    echo();

    keypad(win, TRUE);
    raw();
    wgetnstr(win, buffer, 3);
    flushinp();

    wmove(win, 9, 10);
    wdelch(win);
    mvwaddstr(win, 4, 1, "The character should now have been deleted");
    Continue(win);

    refresh();
    wclear(win);
    echo();
    buffer[0] = '\0';
    mvwaddstr(win, 3, 2, "The window should have moved");
    mvwaddstr(win, 4, 2,
              "This text should have appeared without you pressing a key");
    mvwaddstr(win, 6, 2, "Enter a number then a string separated by space");
    mvwin(win, 2, 1);
    wrefresh(win);
    mvwscanw(win, 7, 6, "%d %s", &num, buffer);
    mvwprintw(win, 8, 6, "String: %s Number: %d", buffer, num);
    Continue(win);

    refresh();
    wclear(win);
    echo();
    mvwaddstr(win, 3, 2, "Enter a 5 character string: ");
    wgetnstr(win, buffer, 5);
    mvwprintw(win, 4, 2, "String: %s", buffer);
    Continue(win);
}

void outputTest(WINDOW *win)
{
    WINDOW *win1;
    char Buffer[80];
    chtype ch;
    int by, bx;

    nl();
    wclear(win);
    mvwaddstr(win, 1, 1, "You should now have a screen in the upper "
                         "left corner, and this text should have wrapped");
    waddstr(win,"\nThis text should be down\n");
    waddstr(win,  "and broken into two here ^");
    Continue(win);

    wclear(win);
    wattron(win, A_BOLD);
    mvwaddstr(win, 1, 1, "A new window will appear with this text in it");
    mvwaddstr(win, 8, 1, "Press any key to continue");
    wrefresh(win);
    wgetch(win);

    getbegyx(win, by, bx);

    if (LINES < 24 || COLS < 75)
    {
        mvwaddstr(win, 5, 1, "Some tests have been skipped as they require a");
        mvwaddstr(win, 6, 1, "display of at least 24 LINES by 75 COLUMNS");
        Continue(win);
    }
    else
    {
        win1 = newwin(10, 50, 14, 25);

        if (win1 == NULL)
        {
            endwin();
            return;
        }

#ifdef A_COLOR
        if (has_colors())
        {
            init_pair(3, COLOR_BLUE, COLOR_WHITE);
            wbkgd(win1, COLOR_PAIR(3));
        }
        else
#endif
            wbkgd(win1, A_NORMAL);

        wclear(win1);
        mvwaddstr(win1, 5, 1, "This text should appear; using overlay option");
        copywin(win, win1, 0, 0, 0, 0, 9, 49, TRUE);
        box(win1, ACS_VLINE, ACS_HLINE);
        wmove(win1, 8, 26);
        wrefresh(win1);
        wgetch(win1);

        wclear(win1);

        wattron(win1, A_BLINK);
        mvwaddstr(win1, 4, 1,
                  "This blinking text should appear in only the second window");
        wattroff(win1, A_BLINK);

        mvwin(win1, by, bx);
        overlay(win, win1);
        mvwin(win1, 14, 25);
        wmove(win1, 8, 26);
        wrefresh(win1);
        wgetch(win1);

        delwin(win1);
    }

    clear();
    wclear(win);
    wrefresh(win);
    mvwaddstr(win, 6, 2, "This line shouldn't appear");
    mvwaddstr(win, 4, 2, "Only half of the next line is visible");
    mvwaddstr(win, 5, 2, "Only half of the next line is visible");
    wmove(win, 6, 1);
    wclrtobot(win);
    wmove(win, 5, 20);
    wclrtoeol(win);
    mvwaddstr(win, 8, 2, "This line also shouldn't appear");
    wmove(win, 8, 1);
    winsdelln(win, -1);
    Continue(win);

    wmove(win, 5, 9);
    ch = winch(win);

    wclear(win);
    wmove(win, 6, 2);
    waddstr(win, "The next char should be l:  ");
    winsch(win, ch);
    Continue(win);

    mvwinsstr(win, 6, 2, "A1B2C3D4E5");
    Continue(win);

    wmove(win, 5, 1);
    winsdelln(win, 1);
    mvwaddstr(win, 5, 2, "The lines below should have moved down");
    Continue(win);

    wclear(win);
    wmove(win, 2, 2);
    wprintw(win, "This is a formatted string in a window: %d %s\n",
            42, "is it");
    mvwaddstr(win, 10, 1, "Enter a string: ");
    wrefresh(win);
    echo();
    wscanw(win, "%s", Buffer);

    printw("This is a formatted string in stdscr: %d %s\n", 42, "is it");
    mvaddstr(10, 1, "Enter a string: ");
    scanw("%s", Buffer);

    wclear(win);
    curs_set(2);
    mvwaddstr(win, 1, 1, "The cursor should be in high-visibility mode");
    Continue(win);

    wclear(win);
    curs_set(0);
    mvwaddstr(win, 1, 1, "The cursor should have disappeared");
    Continue(win);

    wclear(win);
    curs_set(1);
    mvwaddstr(win, 1, 1, "The cursor should be normal");
    Continue(win);

#ifdef A_COLOR
    if (has_colors())
    {
        wclear(win);
        mvwaddstr(win, 1, 1, "Colors should change after you press a key");
        Continue(win);

        init_pair(1, COLOR_RED, COLOR_WHITE);
        wrefresh(win);
    }
#endif
    werase(win);
    mvwaddstr(win, 1, 1, "Information About Your Terminal");
    mvwaddstr(win, 3, 1, termname());
    mvwaddstr(win, 4, 1, longname());

    if (termattrs() & A_BLINK)
        mvwaddstr(win, 5, 1, "This terminal claims to support blinking.");
    else
        mvwaddstr(win, 5, 1, "This terminal does NOT support blinking.");

    mvwaddnstr(win, 7, 5, "Have a nice day!ok", 16);
    wrefresh(win);

    mvwinnstr(win, 7, 5, Buffer, 18);
    mvaddstr(LINES - 2, 10, Buffer);
    refresh();
    Continue(win);
}

#if HAVE_RESIZE
void resizeTest(WINDOW *dummy)
{
    WINDOW *win1;
    int nwidth = 135, nheight = 52;
    int owidth = COLS, oheight = LINES;

    savetty();

    resize_term(nheight, nwidth);

    clear();
    refresh();

    win1 = newwin(10, 50, 14, 25);

    if (win1 == NULL)
    {
        endwin();
        return;
    }

#ifdef A_COLOR
    if (has_colors())
    {
        init_pair(3, COLOR_BLUE, COLOR_WHITE);
        wattrset(win1, COLOR_PAIR(3));
    }

    wclear(win1);
#endif
    mvwaddstr(win1, 0, 0, "The screen may now be resized");
    mvwprintw(win1, 1, 4, "Given size: %d by %d", nwidth, nheight);
    mvwprintw(win1, 2, 4, "Actual size: %d by %d", COLS, LINES);
    Continue(win1);

    wclear(win1);
    resetty();

    mvwaddstr(win1, 0, 0, "The screen should now be reset");
    mvwprintw(win1, 1, 6, "Old size: %d by %d", owidth, oheight);
    mvwprintw(win1, 2, 6, "Size now: %d by %d", COLS, LINES);
    Continue(win1);

    delwin(win1);

    clear();
    refresh();
}
#endif /* HAVE_RESIZE */

void padTest(WINDOW *dummy)
{
    WINDOW *pad, *spad;

    pad = newpad(50, 100);
    wattron(pad, A_REVERSE);
    mvwaddstr(pad, 5, 2, "This is a new pad");
    wattrset(pad, 0);
    mvwaddstr(pad, 8, 0,
        "The end of this line should be truncated here:except  now");
    mvwaddstr(pad, 11, 1, "This line should not appear.It will now");
    wmove(pad, 10, 1);
    wclrtoeol(pad);
    mvwaddstr(pad, 10, 1, " Press any key to continue");
    prefresh(pad, 0, 0, 0, 0, 10, 45);
    keypad(pad, TRUE);
    raw();
    wgetch(pad);

    spad = subpad(pad, 12, 25, 7, 52);
    mvwaddstr(spad, 2, 2, "This is a new subpad");
    box(spad, 0, 0);
    prefresh(pad, 0, 0, 0, 0, 15, 75);
    keypad(pad, TRUE);
    raw();
    wgetch(pad);

    mvwaddstr(pad, 35, 2, "This is displayed at line 35 in the pad");
    mvwaddstr(pad, 40, 1, " Press any key to continue");
    prefresh(pad, 30, 0, 0, 0, 10, 45);
    keypad(pad, TRUE);
    raw();
    wgetch(pad);

    delwin(pad);
}

#if HAVE_CLIPBOARD
void clipboardTest(WINDOW *win)
{
    static const char *text =
        "This string placed in clipboard by PDCurses test program, testcurs.";
    char *ptr = NULL;
    long i, length = 0;

    mvaddstr(1, 1,
             "This test will display the contents of the system clipboard");

    Continue2();

    scrollok(stdscr, TRUE);
    i = PDC_getclipboard(&ptr, &length);

    switch(i)
    {
    case PDC_CLIP_ACCESS_ERROR:
        mvaddstr(3, 1, "There was an error accessing the clipboard");
        refresh();
        break;

    case PDC_CLIP_MEMORY_ERROR:
        mvaddstr(3, 1,
            "Unable to allocate memory for clipboard contents");
        break;

    case PDC_CLIP_EMPTY:
        mvaddstr(3, 1, "There was no text in the clipboard");
        break;

    default:
        wsetscrreg(stdscr, 0, LINES - 1);
        clear();
        mvaddstr(1, 1, "Clipboard contents...");
        mvprintw(2, 1, "%s\n", ptr);
    }

    Continue2();

    clear();
    mvaddstr(1, 1,
        "This test will place the following string in the system clipboard:");
    mvaddstr(2, 1, text);

    i = PDC_setclipboard(text, (long)strlen(text));

    switch(i)
    {
    case PDC_CLIP_ACCESS_ERROR:
        mvaddstr(3, 1, "There was an error accessing the clipboard");
        break;

    case PDC_CLIP_MEMORY_ERROR:
        mvaddstr(3, 1, "Unable to allocate memory for clipboard contents");
        break;

    default:
        mvaddstr(3, 1, "The string was placed in the clipboard successfully");
    }

    Continue2();
}
#endif /* HAVE_CLIPBOARD */

void acsTest(WINDOW *win)
{
    static const char *acs_names[] =
    {
        "ACS_ULCORNER", "ACS_URCORNER", "ACS_LLCORNER", "ACS_LRCORNER",
        "ACS_LTEE", "ACS_RTEE", "ACS_TTEE", "ACS_BTEE", "ACS_HLINE",
        "ACS_VLINE", "ACS_PLUS",

#ifdef ACS_D_ULCORNER
        "ACS_D_ULCORNER", "ACS_D_URCORNER", "ACS_D_LLCORNER", "ACS_D_LRCORNER",
        "ACS_D_LTEE", "ACS_D_RTEE", "ACS_D_TTEE", "ACS_D_BTEE", "ACS_D_HLINE",
        "ACS_D_VLINE", "ACS_D_PLUS",
#endif
#ifdef ACS_SD_ULCORNER
        "ACS_SD_ULCORNER", "ACS_SD_URCORNER", "ACS_SD_LLCORNER",
        "ACS_SD_LRCORNER", "ACS_SD_LTEE",
        "ACS_SD_RTEE", "ACS_SD_TTEE", "ACS_SD_BTEE", "ACS_SD_PLUS",
        "ACS_DS_ULCORNER", "ACS_DS_URCORNER", "ACS_DS_LLCORNER",
        "ACS_DS_LRCORNER", "ACS_DS_LTEE", "ACS_DS_RTEE", "ACS_DS_TTEE",
        "ACS_DS_BTEE", "ACS_DS_PLUS",
#endif
        "ACS_S1",
#ifdef ACS_S3
        "ACS_S3", "ACS_S7",
#endif
        "ACS_S9", "ACS_DIAMOND",
#ifdef ACS_CLUB
        "ACS_CLUB", "ACS_SPADE", "ACS_HEART",
        "ACS_LTBOARD",
#endif
        "ACS_BOARD", "ACS_CKBOARD", "ACS_DEGREE", "ACS_PLMINUS",
        "ACS_BULLET",
#ifdef ACS_SM_BULLET
        "ACS_SM_BULLET", "ACS_MED_BULLET", "ACS_WHITE_BULLET",
        "ACS_PILCROW", "ACS_SECTION", "ACS_SMILE", "ACS_REV_SMILE",
#endif
        "ACS_LARROW", "ACS_RARROW", "ACS_UARROW", "ACS_DARROW",
        "ACS_LANTERN", "ACS_BLOCK",
#ifdef ACS_LEQUAL
        "ACS_LEQUAL", "ACS_GEQUAL", "ACS_NEQUAL",
        "ACS_PI",  "ACS_STERLING",
#endif
#ifdef ACS_CENT
        "ACS_CENT", "ACS_YEN", "ACS_PESETA",
        "ACS_ALPHA", "ACS_BETA", "ACS_GAMMA", "ACS_UP_SIGMA",
        "ACS_LO_SIGMA", "ACS_MU", "ACS_TAU", "ACS_UP_PHI", "ACS_LO_PHI",
        "ACS_OMEGA", "ACS_DELTA", "ACS_INFINITY", "ACS_THETA", "ACS_EPSILON",
        "ACS_INTERSECT", "ACS_SUP2", "ACS_SUP_N", "ACS_TRIPLE_BAR",
        "ACS_APPROX_EQ", "ACS_SQUARE_ROOT", "ACS_NOT", "ACS_REV_NOT",
        "ACS_HALF", "ACS_QUARTER", "ACS_DIVISION",
        "ACS_UP_INTEGRAL", "ACS_LO_INTEGRAL",
        "ACS_UBLOCK", "ACS_BBLOCK",
        "ACS_LBLOCK", "ACS_RBLOCK",
        "ACS_A_ORDINAL", "ACS_O_ORDINAL",
        "ACS_INV_BANG", "ACS_INV_QUERY",
        "ACS_LEFT_ANG_QU", "ACS_RIGHT_ANG_QU",
        "ACS_CENTER_SQU", "ACS_F_WITH_HOOK",
#endif
    };

    const chtype acs_values[] =
    {
        ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER,
        ACS_LTEE, ACS_RTEE, ACS_TTEE, ACS_BTEE, ACS_HLINE,
        ACS_VLINE, ACS_PLUS,

#ifdef ACS_D_ULCORNER
        ACS_D_ULCORNER, ACS_D_URCORNER, ACS_D_LLCORNER, ACS_D_LRCORNER,
        ACS_D_LTEE, ACS_D_RTEE, ACS_D_TTEE, ACS_D_BTEE, ACS_D_HLINE,
        ACS_D_VLINE, ACS_D_PLUS,
#endif
#ifdef ACS_SD_ULCORNER
        ACS_SD_ULCORNER, ACS_SD_URCORNER, ACS_SD_LLCORNER,
        ACS_SD_LRCORNER, ACS_SD_LTEE,
        ACS_SD_RTEE, ACS_SD_TTEE, ACS_SD_BTEE, ACS_SD_PLUS,
        ACS_DS_ULCORNER, ACS_DS_URCORNER, ACS_DS_LLCORNER,
        ACS_DS_LRCORNER, ACS_DS_LTEE, ACS_DS_RTEE, ACS_DS_TTEE,
        ACS_DS_BTEE, ACS_DS_PLUS,
#endif
        ACS_S1,
#ifdef ACS_S3
        ACS_S3, ACS_S7,
#endif
        ACS_S9, ACS_DIAMOND,
#ifdef ACS_CLUB
        ACS_CLUB, ACS_SPADE, ACS_HEART, ACS_LTBOARD,
#endif
        ACS_BOARD, ACS_CKBOARD, ACS_DEGREE, ACS_PLMINUS, ACS_BULLET,
#ifdef ACS_SM_BULLET
        ACS_SM_BULLET, ACS_MED_BULLET, ACS_WHITE_BULLET,
        ACS_PILCROW, ACS_SECTION, ACS_SMILE, ACS_REV_SMILE,
#endif
        ACS_LARROW, ACS_RARROW, ACS_UARROW, ACS_DARROW,
        ACS_LANTERN, ACS_BLOCK,
#ifdef ACS_LEQUAL
        ACS_LEQUAL, ACS_GEQUAL, ACS_NEQUAL,
        ACS_PI,  ACS_STERLING,
#endif
#ifdef ACS_CENT
        ACS_CENT, ACS_YEN, ACS_PESETA,
        ACS_ALPHA, ACS_BETA, ACS_GAMMA, ACS_UP_SIGMA,
        ACS_LO_SIGMA, ACS_MU, ACS_TAU, ACS_UP_PHI, ACS_LO_PHI,
        ACS_OMEGA, ACS_DELTA, ACS_INFINITY, ACS_THETA, ACS_EPSILON,
        ACS_INTERSECT, ACS_SUP2, ACS_SUP_N, ACS_TRIPLE_BAR,
        ACS_APPROX_EQ, ACS_SQUARE_ROOT, ACS_NOT, ACS_REV_NOT,
        ACS_HALF, ACS_QUARTER, ACS_DIVISION,
        ACS_UP_INTEGRAL, ACS_LO_INTEGRAL,
        ACS_UBLOCK, ACS_BBLOCK,
        ACS_LBLOCK, ACS_RBLOCK,
        ACS_A_ORDINAL, ACS_O_ORDINAL,
        ACS_INV_BANG, ACS_INV_QUERY,
        ACS_LEFT_ANG_QU, ACS_RIGHT_ANG_QU,
        ACS_CENTER_SQU, ACS_F_WITH_HOOK,
#endif
    };

#ifdef WACS_S1
    const cchar_t *wacs_values[] =
    {
        WACS_ULCORNER, WACS_URCORNER, WACS_LLCORNER, WACS_LRCORNER,
        WACS_LTEE, WACS_RTEE, WACS_TTEE, WACS_BTEE, WACS_HLINE,
        WACS_VLINE, WACS_PLUS,

#ifdef WACS_D_ULCORNER
        WACS_D_ULCORNER, WACS_D_URCORNER, WACS_D_LLCORNER, WACS_D_LRCORNER,
        WACS_D_LTEE, WACS_D_RTEE, WACS_D_TTEE, WACS_D_BTEE, WACS_D_HLINE,
        WACS_D_VLINE, WACS_D_PLUS,
#endif
#ifdef WACS_SD_ULCORNER
        WACS_SD_ULCORNER, WACS_SD_URCORNER, WACS_SD_LLCORNER,
        WACS_SD_LRCORNER, WACS_SD_LTEE,
        WACS_SD_RTEE, WACS_SD_TTEE, WACS_SD_BTEE, WACS_SD_PLUS,
        WACS_DS_ULCORNER, WACS_DS_URCORNER, WACS_DS_LLCORNER,
        WACS_DS_LRCORNER, WACS_DS_LTEE, WACS_DS_RTEE, WACS_DS_TTEE,
        WACS_DS_BTEE, WACS_DS_PLUS,
#endif
        WACS_S1,
#ifdef WACS_S3
        WACS_S3, WACS_S7,
#endif
        WACS_S9, WACS_DIAMOND,
#ifdef WACS_CLUB
        WACS_CLUB, WACS_SPADE, WACS_HEART, WACS_LTBOARD,
#endif
        WACS_BOARD, WACS_CKBOARD, WACS_DEGREE, WACS_PLMINUS, WACS_BULLET,
#ifdef WACS_SM_BULLET
        WACS_SM_BULLET, WACS_MED_BULLET, WACS_WHITE_BULLET,
        WACS_PILCROW, WACS_SECTION, WACS_SMILE, WACS_REV_SMILE,
#endif
        WACS_LARROW, WACS_RARROW, WACS_UARROW, WACS_DARROW,
        WACS_LANTERN, WACS_BLOCK,
#ifdef WACS_LEQUAL
        WACS_LEQUAL, WACS_GEQUAL, WACS_NEQUAL,
        WACS_PI,  WACS_STERLING,
#endif
#ifdef WACS_CENT
        WACS_CENT, WACS_YEN, WACS_PESETA,
        WACS_ALPHA, WACS_BETA, WACS_GAMMA, WACS_UP_SIGMA,
        WACS_LO_SIGMA, WACS_MU, WACS_TAU, WACS_UP_PHI, WACS_LO_PHI,
        WACS_OMEGA, WACS_DELTA, WACS_INFINITY, WACS_THETA, WACS_EPSILON,
        WACS_INTERSECT, WACS_SUP2, WACS_SUP_N, WACS_TRIPLE_BAR,
        WACS_APPROX_EQ, WACS_SQUARE_ROOT, WACS_NOT, WACS_REV_NOT,
        WACS_HALF, WACS_QUARTER, WACS_DIVISION,
        WACS_UP_INTEGRAL, WACS_LO_INTEGRAL,
        WACS_UBLOCK, WACS_BBLOCK,
        WACS_LBLOCK, WACS_RBLOCK,
        WACS_A_ORDINAL, WACS_O_ORDINAL,
        WACS_INV_BANG, WACS_INV_QUERY,
        WACS_LEFT_ANG_QU, WACS_RIGHT_ANG_QU,
        WACS_CENTER_SQU, WACS_F_WITH_HOOK,
#endif               /* #if WACS_CENT */
    };
#endif               /* #ifdef WACS_S1   */

#ifdef WACS_S1
    static const wchar_t russian[] = {0x0420, 0x0443, 0x0441, 0x0441,
        0x043a, 0x0438, 0x0439, L' ', 0x044f, 0x0437, 0x044b, 0x043a, 0};

    static const wchar_t greek[] = {0x0395, 0x03bb, 0x03bb, 0x03b7,
        0x03bd, 0x03b9, 0x03ba, 0x03ac, 0};

    static const wchar_t georgian[] = {0x10e5, 0x10d0, 0x10e0, 0x10d7,
        0x10e3, 0x10da, 0x10d8, L' ', 0x10d4, 0x10dc, 0x10d0, 0};

    static const wchar_t fullwidth[] = { 0xff26, 0xff55, 0xff4c, 0xff4c,
        0xff57, 0xff49, 0xff44, 0xff54, 0xff48, 0 };  /* "Fullwidth" */

    static const wchar_t combining_marks[] = { L'C', L'o', 0x35c, L'm',
                   L'b', 0x30a, L'i', L'n', L'i', 0x304, L'n', 0x30b, 0x329,
                   L'g', 0x310,
                   L' ', L'C', 0x338, L'h', 0x306,  L'a', 0x361, L'r', L's',
                   0x30e, 0x348, 0 };

#endif

    int i, tmarg = 1, ncols = (COLS - 4) / 19;
    int col_size = (COLS - 4) / ncols;
    int n_items = sizeof( acs_names) / sizeof( acs_names[0]);
    int n_rows = LINES / 2 - 4;

    i = 0;
    while( i < n_items)
    {
        int j, xloc = 3;

        attrset(A_BOLD);
        mvaddstr( 1, (COLS - 23) / 2, "Alternate Character Set");
        attrset(A_NORMAL);
        tmarg = 4;
        while( i < n_items && xloc < COLS - col_size)
        {
            for( j = 0; i < n_items && j < n_rows; j++, i++)
            {
                move( j * 2 + tmarg, xloc);
                addch(acs_values[i]);
                printw(" %s", acs_names[i]);
            }
            xloc += col_size;
        }

        mvaddstr( tmarg + n_rows * 2, 3, curses_version( ));
        move( tmarg + n_rows * 2 + 1, 3);
        printw( "sizeof( chtype) = %d; sizeof( mmask_t) = %d",
                           (int)sizeof( chtype), (int)sizeof( mmask_t));
        mvaddstr(tmarg + n_rows * 2 + 2, 3, "Press any key to continue");
        getch();
        clear( );
    }

#if HAVE_WIDE
    i = 0;
    while( i < n_items)
    {
        int j, xloc = 3;

        attrset(A_BOLD);
        mvaddstr( 1, (COLS - 28) / 2, "Wide Alternate Character Set");
        attrset(A_NORMAL);
        tmarg = 4;
#ifdef WACS_S1
        while( i < n_items && xloc < COLS - col_size)
        {
            for( j = 0; i < n_items && j < n_rows; j++, i++)
            {
                move( j * 2 + tmarg, xloc);
                add_wch( wacs_values[i]);
                printw(" W%s", acs_names[i]);
            }
            xloc += col_size;
        }
#endif
    /* Spanish, Russian, Greek, Georgian, fullwidth, combining */

        tmarg += n_rows * 2;
        mvaddwstr(tmarg, COLS / 8 - 5, L"Espa\xf1ol");
        mvaddwstr(tmarg, 3 * (COLS / 8) - 5, russian);
        mvaddwstr(tmarg, 5 * (COLS / 8) - 5, greek);
        mvaddwstr(tmarg, 7 * (COLS / 8) - 5, georgian);
        mvaddwstr(tmarg + 1, COLS / 8 - 5, fullwidth);

#if(CHTYPE_LONG >= 2)       /* "non-standard" 64-bit chtypes     */
        mvaddwstr(tmarg + 1, 3 * (COLS / 8) - 5, combining_marks);
        mvaddch( tmarg + 1, 7 * (COLS / 8) - 5, (chtype)0x1d11e);
#endif            /* U+1D11E = musical symbol G clef */

        mvaddstr(tmarg + 2, 3, "Press any key to continue");
        getch();
        clear( );
    }
#endif
}

#if HAVE_COLOR

#if CHTYPE_LONG >= 2 || (CHTYPE_LONG == 1 && !defined( PDC_WIDE))
   #define GOT_DIM
#ifdef A_OVERLINE
   #define GOT_OVERLINE
#endif
#ifdef A_STIKEOUT
   #define GOT_STRIKEOUT
#endif
#endif

void colorTest(WINDOW *win)
{
    static const short colors[] =
    {
        COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_BLUE,
        COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE
    };

    static const char *colornames[] =
    {
        "COLOR_BLACK", "COLOR_RED", "COLOR_GREEN", "COLOR_BLUE",
        "COLOR_CYAN", "COLOR_MAGENTA", "COLOR_YELLOW", "COLOR_WHITE"
    };

    chtype fill = ACS_BLOCK;

    int i, j, tmarg, col1, col2, col3, col4, ch;

    if (!has_colors())
        return;

    do
    {
        tmarg = (LINES - 19) / 2;
        col1 = (COLS - 60) / 2;
        col2 = col1 + 15;
        col3 = col2 + 15;
        col4 = col3 + 15;

        attrset(A_BOLD);
        mvaddstr(tmarg, (COLS - 22) / 2, "Color Attribute Macros");
        attrset(A_NORMAL);

        mvaddstr(tmarg + 3, col2 + 2, "A_NORMAL");
        mvaddstr(tmarg + 3, col3 + 3, "A_BOLD");
        mvaddstr(tmarg + 3, col4 + 3, "A_BLINK");

        for (i = 0; i < 8; i++)
        {
            init_pair((short)(i + 4), colors[i], background_index);
            mvaddstr(tmarg + i + 5, col1, colornames[i]);

            for (j = 0; j < 12; j++)
            {
                mvaddch(tmarg + i + 5, col2 + j, fill | COLOR_PAIR(i + 4));
                mvaddch(tmarg + i + 5, col3 + j, fill | COLOR_PAIR(i + 4) | A_BOLD);
                mvaddch(tmarg + i + 5, col4 + j, fill | COLOR_PAIR(i + 4) | A_BLINK);
            }
            attrset( COLOR_PAIR( i + 4) | A_BLINK);
            mvaddstr( tmarg + i + 5, col4 + 5, "Text");
            attrset( COLOR_PAIR( i + 4) | A_BOLD);
            mvaddstr( tmarg + i + 5, col3 + 5, "Text");
            attroff( A_BOLD);
            mvaddstr( tmarg + i + 5, col2 + 5, "Text");
            attrset( A_NORMAL);
        }

        mvprintw(tmarg + 15, col1, "COLORS = %d", COLORS);
        mvprintw(tmarg + 16, col1, "COLOR_PAIRS = %d", COLOR_PAIRS);

#ifdef CHTYPE_LONG
        attrset(A_ITALIC);
        mvprintw( tmarg + 15, col3, "Italic");
        attrset(A_ITALIC | A_BLINK);
        mvprintw( tmarg + 15, col4, "Italic Blink");
        attrset(A_BOLD | A_ITALIC);
        mvprintw( tmarg + 17, col4, "Italic Bold");
        attrset(A_BOLD | A_ITALIC | A_BLINK);
        mvprintw( tmarg + 18, col4, "Italic Blink Bold");
#endif
        attrset(A_BOLD);
        mvprintw( tmarg + 16, col3, "Bold");
        attrset(A_BLINK);
        mvprintw( tmarg + 17, col3, "Blink");

        attrset(A_BLINK | A_BOLD);
        mvprintw( tmarg + 16, col4, "Blink Bold");
/* end BJG addenda */
        attrset(A_NORMAL);

        mvaddstr(tmarg + 19, 3, "Press any key to continue");
        ch = getch();
# ifdef PDCURSES
        if( ch == KEY_RESIZE)
        {
            erase();
            resize_term(0, 0);
        }
# endif
    }  while( ch == KEY_RESIZE);

    if (can_change_color())
    {
        struct
        {
            short red, green, blue;
        } orgcolors[16];

        int MAXCOL = (COLORS >= 16) ? 16 : 8;

        if (MAXCOL < 8)
            return;

        for (i = 0; i < MAXCOL; i++)
            color_content((short)i, &(orgcolors[i].red),
                                    &(orgcolors[i].green),
                                    &(orgcolors[i].blue));

        attrset(A_BOLD);
        mvaddstr(tmarg, (COLS - 22) / 2, " init_color() Example ");
        attrset(A_NORMAL);

        refresh();

        for (i = 0; i < 8; i++)
        {
            init_color(colors[i], (short)(i * 125), 0, (short)(i * 125));

            if (MAXCOL == 16)
                init_color((short)(colors[i] + 8), 0, (short)(i * 125), 0);
        }

        mvaddstr(tmarg + 19, 3, "Press any key to continue");
        getch();
        for (i = 0; i < MAXCOL; i++)
            init_color((short)i, orgcolors[i].red,
                                 orgcolors[i].green,
                                 orgcolors[i].blue);
    }

    if (COLORS >= 256) do
        {
        int x, y, z, lmarg = (COLS - 77) / 2;

        erase();

        attrset(A_BOLD);
        mvaddstr(tmarg, (COLS - 15) / 2, "Extended Colors");
        attrset(A_NORMAL);

        mvaddstr(tmarg + 3, lmarg, "6x6x6 Color Cube (16-231):");

        for (i = 16; i < 256; i++)
            init_pair(i, COLOR_BLACK, i);

        for (i = 16, z = 0; z < 6; z++)
            for (y = 0; y < 6; y++)
                for (x = 0; x < 6; x++)
                {
                    chtype ch = ' ' | COLOR_PAIR(i++);

                    mvaddch(tmarg + 5 + y, z * 13 + x * 2 + lmarg, ch);
                    addch(ch);
                }

        mvaddstr(tmarg + 13, lmarg, "Greyscale (232-255):");

        for (x = 0; x < 24; x++)
        {
            chtype ch = ' ' | COLOR_PAIR(232 + x);

            mvaddch(tmarg + 15, x * 2 + lmarg, ch);
            addch(ch);
        }

#ifdef CHTYPE_LONG
       attrset( A_LEFTLINE);
       mvaddstr( tmarg + 17, col1, "A_LEFTLINE");
       attrset( A_UNDERLINE);
       mvaddstr( tmarg + 18, col1, "A_UNDERLINE");
       attrset( A_RIGHTLINE);
       mvaddstr( tmarg + 19, col1, "A_RIGHTLINE");
#endif
#ifdef GOT_OVERLINE
       attrset( A_OVERLINE);
       mvaddstr( tmarg + 17, col2, "A_OVERLINE");
#ifdef GOT_STRIKEOUT
       attrset( A_STRIKEOUT);
       mvaddstr( tmarg + 18, col2, "A_STRIKEOUT");
#endif
       attrset( A_OVERLINE | A_UNDERLINE);
       mvaddstr( tmarg + 19, col2, "Over/underlined");
#endif
       attrset(A_NORMAL);
       refresh( );
       ch = getch( );
# ifdef PDCURSES
        if( ch == KEY_RESIZE)
            resize_term(0, 0);
# endif
    } while( ch == KEY_RESIZE);
}
#endif

#if HAVE_WIDE
void wideTest(WINDOW *win)
{
    wchar_t tmp[513];
    size_t i;

    attrset(A_BOLD);
    mvaddstr(1, (COLS - 25) / 2, "Wide Character Input Test");
    attrset(A_NORMAL);

    mvaddstr(4, 1, "Enter a string: ");

    echo();

    get_wstr((wint_t *)tmp);
    addstr("\n\n String:\n\n ");
    addwstr(tmp);
    addstr("\n\n\n Hex:\n\n ");

    for (i = 0; i < wcslen(tmp); i++)
    {
        printw("%04x ", tmp[i]);
        addnwstr(tmp + i, 1);
        addstr("  ");
    }

    noecho();

    Continue2();
}
#endif

void display_menu(int old_option, int new_option)
{
    int lmarg = (COLS - 14) / 2,
        tmarg = (LINES - (MAX_OPTIONS + 2)) / 2;

    if (old_option == -1)
    {
        int i;

        attrset(A_BOLD);
        mvaddstr(tmarg - 3, lmarg - 5, "PDCurses Test Program");
        attrset(A_NORMAL);

        for (i = 0; i < MAX_OPTIONS; i++)
            mvaddstr(tmarg + i, lmarg, command[i].text);
    }
    else
        mvaddstr(tmarg + old_option, lmarg, command[old_option].text);

    attrset(A_REVERSE);
    mvaddstr(tmarg + new_option, lmarg, command[new_option].text);
    attrset(A_NORMAL);

    mvaddstr(tmarg + MAX_OPTIONS + 2, lmarg - 23,
             "Use Up and Down Arrows to select - Enter to run - Q to quit");
    refresh();
}
