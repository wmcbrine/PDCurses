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
***************************************************************************
*/
/*
 *
 * This is a test program for the PDCurses screen package for IBM PC type
 * machines.
 * This program was written by John Burnell (johnb@kea.am.dsir.govt.nz)
 *
 *  wrs(5/28/93) -- modified to be consistent (perform identically) with either
 *                  PDCurses or under Unix System V, R4
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef PDCDEBUG
char *rcsid_testcurs  = "$Id: testcurs.c,v 1.5 2002/11/27 11:15:40 mark Exp $";
#endif

#include <stdio.h>
#include <curses.h>

#if defined(XCURSES)
	char *XCursesProgramName = "testcurs";
#endif

#ifdef __STDC__
void inputTest (WINDOW *);
void scrollTest (WINDOW *);
void introTest (WINDOW *);
int initTest (WINDOW **, int, char **);
void outputTest (WINDOW *);
void padTest (WINDOW *);
void display_menu(int,int);
#  if defined(PDCURSES) && !defined(XCURSES)
void resizeTest (WINDOW *);
#  endif
#  ifdef PDCURSES
void clipboardTest(WINDOW *);
#  endif
#else
void inputTest ();
void scrollTest ();
void introTest ();
int initTest ();
void outputTest ();
void padTest ();
void display_menu();
#  if defined(PDCURSES) && !defined(XCURSES)
void resizeTest ();
#  endif
#  ifdef PDCURSES
void clipboardTest();
#  endif
#endif

struct commands
{
 char *text;
#ifdef __STDC__
 void (*function)(WINDOW *);
#else
 void (*function)();
#endif
};
typedef struct commands COMMAND;
#if defined(PDCURSES) && !defined(XCURSES)
# define MAX_OPTIONS 7
#else
# define MAX_OPTIONS 6
#endif
COMMAND command[MAX_OPTIONS] =
{
 {"Intro Test",introTest},
 {"Pad Test",padTest},
#  if defined(PDCURSES) && !defined(XCURSES)
 {"Resize Test",resizeTest},
#endif
 {"Scroll Test",scrollTest},
 {"Input Test",inputTest},
 {"Output Test",outputTest},
#  if defined(PDCURSES)
 {"Clipboard Test",clipboardTest},
#  endif
};

int     width, height;

int main( int argc, char *argv[])
{
WINDOW  *win;
int key,old_option=(-1),new_option=0;
bool quit=FALSE;

#ifdef PDCDEBUG
	PDC_debug("testcurs started\n");
#endif
    if (initTest (&win,argc,argv)) {return 1;}

#ifdef A_COLOR
    if (has_colors())
      {
       init_pair(1,COLOR_WHITE,COLOR_BLUE);
       wbkgd(win, COLOR_PAIR(1));
      }
    else
       wbkgd(win, A_REVERSE);
#else
    wbkgd(win, A_REVERSE);
#endif

    erase();
    display_menu(old_option,new_option);
    while(1)
      {
       noecho();
       keypad(stdscr,TRUE);
       raw();
       key = getch();
       switch(key)
         {
          case 10:
          case 13:
          case KEY_ENTER:
                         erase();
                         refresh();
                         (*command[new_option].function)(win);
                         erase();
                         display_menu(old_option,new_option);
                         break;
          case KEY_UP:
                         new_option = (new_option == 0) ? new_option : new_option-1;
                         display_menu(old_option,new_option);
                         break;
          case KEY_DOWN:
                         new_option = (new_option == MAX_OPTIONS-1) ? new_option : new_option+1;
                         display_menu(old_option,new_option);
                         break;
          case 'Q':
          case 'q':
                         quit = TRUE;
                         break;
          default:       break;
         }
       if (quit == TRUE)
          break;
      }

    delwin (win);
    endwin();
#ifdef XCURSES
    XCursesExit();
#endif
    return 0;
}
#ifdef __STDC__
void Continue (WINDOW *win)
#else
void Continue (win)
WINDOW *win;
#endif
{
    wmove(win, 10, 1);
    wclrtoeol(win);
    mvwaddstr(win, 10, 1, " Press any key to continue");
    wrefresh(win);
    raw();
    wgetch(win);
}

#ifdef __STDC_
int initTest (WINDOW **win, int argc, char *argv[])
#else
int initTest (win, argc, argv)
WINDOW **win;
int argc;
char *argv[];
#endif
{
#ifdef PDCDEBUG
	PDC_debug("initTest called\n");
#endif
/*  traceon(); */
#ifdef XCURSES
    Xinitscr(argc, argv);
#else
    initscr();
#endif
#ifdef PDCDEBUG
	PDC_debug("after initscr()\n");
#endif
#ifdef A_COLOR
    if (has_colors())
       start_color();
#endif
    width  = 60;
    height = 13;                /* Create a drawing window */
    *win = newwin(height, width, (LINES-height)/2, (COLS-width)/2);
    if(*win == NULL)
    {   endwin();
        return 1;
    }
    return(0);
}
#ifdef __STDC__
void introTest (WINDOW *win)
#else
void introTest (win)
WINDOW *win;
#endif
{
    wmove( win, height/2-5, width/2 );
    wvline( win, ACS_VLINE, 10 );
    wmove( win, height/2, width/2-10 );
    whline( win, ACS_HLINE, 20 );
    Continue(win);

    beep ();
    werase(win);

    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh(win);
    cbreak ();
    mvwaddstr(win, 1, 1, "You should have rectangle in the middle of the screen");
    mvwaddstr(win, 2, 1, "You should have heard a beep");
    Continue(win);
    return;
}
#ifdef __STDC__
void scrollTest (WINDOW *win)
#else
void scrollTest (win)
WINDOW *win;
#endif
{
    int i;
    int OldX, OldY;
    char *Message = "The window will now scroll slowly";

/* disable typeahead checking */

    typeahead(-1);

    mvwprintw (win, height - 2, 1, Message);
    wrefresh (win);
    scrollok(win, TRUE);
    for (i = 1; i <= height; i++) {
      napms (150);
      scroll(win);
      wrefresh (win);
    };

    getmaxyx (win, OldY, OldX);
    mvwprintw (win, 6, 1, "The top of the window will scroll");
    wmove (win, 1, 1);
    wsetscrreg (win, 0, 4);
    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh (win);
    for (i = 1; i <= 5; i++) 
      {
       napms (500);
       scroll(win);
       wrefresh (win);
      }

    mvwprintw (win, 3, 1, "The bottom of the window will scroll");
    wmove (win, 8, 1);
    wsetscrreg (win, 5, --OldY);
    box(win, ACS_VLINE, ACS_HLINE);
    wrefresh (win);
    for (i = 5; i <= OldY; i++) 
      {
       napms (300);
       wscrl(win,-1);
       wrefresh (win);
      }
    wsetscrreg (win, 0, OldY);

}
#ifdef __STDC__
void inputTest (WINDOW *win)
#else
void inputTest (win)
WINDOW *win;
#endif
{
    int w, h, bx, by, sw, sh, i, c,num;
    char buffer [80];
    WINDOW *subWin;
    wclear (win);

    getmaxyx(win, h, w);
    getbegyx(win, by, bx);

    sw = w / 3;
    sh = h / 3;
    if((subWin = subwin(win, sh, sw, by + h - sh - 2, bx + w - sw - 2)) == NULL)
        return;

#ifdef A_COLOR
    if (has_colors())
      {
       init_pair(2,COLOR_WHITE,COLOR_RED);
       wbkgd(subWin, COLOR_PAIR(2) | A_BOLD);
      }
    else
       wbkgd(subWin, A_BOLD);
#else
    wbkgd(subWin, A_BOLD);
#endif
    box(subWin, ACS_VLINE, ACS_HLINE);
    wrefresh(win);

    nocbreak();

    wclear (win);
    mvwaddstr(win, 1, 1, "Press keys (or mouse buttons) to show their names");
    mvwaddstr(win, 2, 1, "Press spacebar to finish");
    wrefresh(win);
    keypad(win, TRUE);
    raw();
    noecho();
    typeahead(-1);
#if defined(PDCURSES)
    mouse_set(ALL_MOUSE_EVENTS);
    PDC_save_key_modifiers(TRUE);
    PDC_return_key_modifiers(TRUE);
#endif
    while(1)
      {
       wmove(win,3,5);
       c = wgetch(win);
       wclrtobot(win);
       if (c >= KEY_MIN)
          wprintw(win,"Key Pressed: %s", keyname(c));
       else if (isprint(c))
          wprintw(win,"Key Pressed: %c", c);
       else
          wprintw(win,"Key Pressed: %s", unctrl(c));
#if defined(PDCURSES)
       if (PDC_get_key_modifiers())
       {
          waddstr(win, " Modifier(s):");
          if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_SHIFT)
             waddstr(win," SHIFT");
          if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_CONTROL)
             waddstr(win," CONTROL");
          if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_ALT)
             waddstr(win," ALT");
          if (PDC_get_key_modifiers() & PDC_KEY_MODIFIER_NUMLOCK)
             waddstr(win," NUMLOCK");
       }
       if (c == KEY_MOUSE)
       {
          int button=0;
          request_mouse_pos();
          if (BUTTON_CHANGED(1))
             button = 1;
          else
             if (BUTTON_CHANGED(2))
                button = 2;
             else
                if (BUTTON_CHANGED(3))
                   button = 3;
                else
                   button = 0;
          if (BUTTON_STATUS(button) & BUTTON_MODIFIER_MASK)
          {
             waddstr(win, " Modifier(s):");
             if ((BUTTON_STATUS(button) & BUTTON_MODIFIER_MASK) == BUTTON_SHIFT)
                waddstr(win," SHIFT");
             if ((BUTTON_STATUS(button) & BUTTON_MODIFIER_MASK) == BUTTON_CONTROL)
                waddstr(win," CONTROL");
             if ((BUTTON_STATUS(button) & BUTTON_MODIFIER_MASK) == BUTTON_ALT)
                waddstr(win," ALT");
          }
          wmove(win,4,18);
          wprintw(win,"Button %d: ",button);
          if (MOUSE_MOVED)
             wprintw(win,"moved: ");
          else if ((BUTTON_STATUS(button) & BUTTON_ACTION_MASK) == BUTTON_PRESSED)
             wprintw(win,"pressed: ");
          else if ((BUTTON_STATUS(button) & BUTTON_ACTION_MASK) == BUTTON_CLICKED)
             wprintw(win,"clicked: ");
          else if ((BUTTON_STATUS(button) & BUTTON_ACTION_MASK) == BUTTON_DOUBLE_CLICKED)
             wprintw(win,"double: ");
          else
             wprintw(win,"released: ");
          wprintw(win," Position: Y: %d X: %d",MOUSE_Y_POS,MOUSE_X_POS);
       }
#endif
       wrefresh(win);
       if (c == ' ')
          break;
      }
#if 0
    nodelay(win, TRUE);
    wgetch(win);
    nodelay(win, FALSE);
#endif
#if defined(PDCURSES)
    mouse_set(0L);
    PDC_save_key_modifiers(FALSE);
    PDC_return_key_modifiers(FALSE);
#endif

    wclear (win);
    mvwaddstr(win, 2, 1, "Press some keys for 5 seconds");
    mvwaddstr(win, 1, 1, "Pressing ^C should do nothing");
    wrefresh(win);

    werase (subWin);
    box(subWin, ACS_VLINE, ACS_HLINE);
    for (i = 0; i < 5; i++) 
      {
       mvwprintw (subWin, 1, 1, "Time = %d", i);
       wrefresh(subWin);
       napms(1000);
       flushinp();
      }

    delwin (subWin);
    werase(win);
    flash();
    wrefresh(win);
    napms(500);
    flushinp();

    mvwaddstr(win, 2, 1, "Press a key, followed by ENTER");
    wmove(win, 9, 10);
    wrefresh(win);
    echo();
/*
    noraw();
*/
    keypad(win, TRUE);
    raw();
    wgetnstr(win, buffer, 3);
    flushinp();

    wmove(win, 9, 10);
    wdelch(win);
    mvwaddstr(win, 4, 1, "The character should now have been deleted");
    Continue(win);

    refresh();
    wclear (win);
    echo();
    mvwaddstr(win, 3, 2, "The window should have moved");
    mvwaddstr(win, 4, 2, "This text should have appeared without you pressing a key");
    mvwaddstr(win, 6, 2, "Enter a number then a string seperated by space");
    mvwin(win, 2, 1);
    wrefresh(win);
    mvwscanw(win, 7, 6, "%d %s", &num,buffer);
    mvwprintw(win, 8, 6, "String: %s Number: %d", buffer,num);
    Continue(win);

    refresh();
    wclear (win);
    echo();
    mvwaddstr(win, 3, 2, "Enter a 5 character string: ");
    wgetnstr(win,buffer,5);
    mvwprintw(win, 4, 2, "String: %s", buffer);
    Continue(win);
}
#ifdef __STDC__
void outputTest (WINDOW *win)
#else
void outputTest (win)
WINDOW *win;
#endif
{
    WINDOW *win1;
    char Buffer [80];
    chtype ch;
    int by, bx;

    nl ();
    wclear (win);
    mvwaddstr(win, 1, 1, "You should now have a screen in the upper left corner, and this text should have wrapped");
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
          init_pair(3,COLOR_BLUE,COLOR_WHITE);
          wbkgd(win1, COLOR_PAIR(3));
         }
       else
          wbkgd(win1, A_NORMAL);
#else
       wbkgd(win1, A_NORMAL);
#endif
       wclear (win1);
       mvwaddstr(win1, 5, 1, "This text should appear; using overlay option");
       copywin(win, win1,0,0,0,0,9,49,TRUE);

#if defined(PDCURSES) && !defined(XCURSES)
       box(win1,0xb3,0xc4);
#else
       box(win1,ACS_VLINE,ACS_HLINE);
#endif
       wmove(win1, 8, 26);
       wrefresh(win1);
       wgetch(win1);

       wclear(win1);
       wattron(win1, A_BLINK);
       mvwaddstr(win1, 4, 1, "This blinking text should appear in only the second window");
       wattroff(win1, A_BLINK);
       mvwin(win1, by, bx);
       overlay(win, win1);
       mvwin(win1,14,25);
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
    wclrtobot (win);
    wmove(win, 5, 20);
    wclrtoeol (win);
    mvwaddstr(win, 8, 2, "This line also shouldn't appear");
    wmove(win, 8, 1);
    winsdelln(win,-1);
    Continue(win);

    wmove (win, 5, 9);
    ch = winch (win);

    wclear(win);
    wmove (win, 6, 2);
    waddstr (win, "The next char should be l:  ");
    winsch (win, ch);
    Continue(win);

    mvwinsstr( win, 6, 2, "A1B2C3D4E5" );
    Continue(win);

    wmove(win, 5, 1);
    winsdelln (win,1);
    mvwaddstr(win, 5, 2, "The lines below should have moved down");
    Continue(win);

    wclear(win);
    wmove(win, 2, 2);
    wprintw(win, "This is a formatted string in a window: %d %s\n", 42, "is it");
    mvwaddstr(win, 10, 1, "Enter a string: ");
    wrefresh(win);
    echo();
    wscanw (win, "%s", Buffer);

    printw("This is a formatted string in stdscr: %d %s\n", 42, "is it");
    mvaddstr(10, 1, "Enter a string: ");
    scanw ("%s", Buffer);

    wclear(win);
    curs_set(2);
    mvwaddstr(win, 1, 1, "The cursor should appear as a block");
    Continue(win);

    wclear(win);
    curs_set(0);
    mvwaddstr(win, 1, 1, "The cursor should have disappeared");
    Continue(win);

    wclear(win);
    curs_set(1);
    mvwaddstr(win, 1, 1, "The cursor should be an underline");
    Continue(win);

#ifdef A_COLOR
    if ( has_colors() )
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
    if ( termattrs() & A_BLINK )
       mvwaddstr(win,5, 1, "This terminal supports blinking.");
    else
       mvwaddstr(win,5, 1, "This terminal does NOT support blinking.");

    mvwaddnstr( win, 7,5, "Have a nice day!ok", 16 );
    wrefresh(win);

    mvwinnstr( win, 7,5, Buffer, 18 );
    mvaddstr( LINES-2, 10, Buffer );
    refresh();
    Continue(win);
}

#if defined(PDCURSES) && !defined(XCURSES)
# ifdef __STDC__
void resizeTest(WINDOW *dummy)
# else
void resizeTest(dummy)
WINDOW *dummy;
# endif
{
    WINDOW *win1;

    savetty ();
    clear();
    refresh();
# if defined(PDCURSES) && !defined(XCURSES)
#  if defined(OS2)
    resize_term(50,120);
#  else
    resize_term(50,80);
#  endif
# endif


    win1 = newwin(10, 50, 14, 25);
    if(win1 == NULL)
    {   endwin();
        return;
    }
#ifdef A_COLOR
    if (has_colors())
      {
       init_pair(3,COLOR_BLUE,COLOR_WHITE);
       wattrset(win1, COLOR_PAIR(3));
      }
#endif
    wclear (win1);

    mvwaddstr(win1, 1, 1, "The screen may now have 50 lines");
    Continue(win1);

    wclear (win1);
    resetty ();

    mvwaddstr(win1, 1, 1, "The screen should now be reset");
    Continue(win1);

    delwin(win1);

    clear();
    refresh();

}
#endif

#ifdef __STDC__
void padTest(WINDOW *dummy)
#else
void padTest(dummy)
WINDOW *dummy;
#endif
{
WINDOW *pad,*spad;

 pad = newpad(50,100);
 wattron(pad, A_REVERSE);
 mvwaddstr(pad, 5, 2, "This is a new pad");
 wattrset(pad,0);
 mvwaddstr(pad, 8, 0, "The end of this line should be truncated here:except  now");
 mvwaddstr(pad,11, 1, "This line should not appear.It will now");
 wmove(pad, 10, 1);
 wclrtoeol(pad);
 mvwaddstr(pad, 10, 1, " Press any key to continue");
 prefresh(pad,0,0,0,0,10,45);
 keypad(pad, TRUE);
 raw();
 wgetch(pad);

 spad = subpad(pad,12,25,6,52);
 mvwaddstr(spad, 2, 2, "This is a new subpad");
 box(spad,0,0);
 prefresh(pad,0,0,0,0,15,75);
 keypad(pad, TRUE);
 raw();
 wgetch(pad);

 mvwaddstr(pad, 35, 2, "This is displayed at line 35 in the pad");
 mvwaddstr(pad, 40, 1, " Press any key to continue");
 prefresh(pad,30,0,0,0,10,45);
 keypad(pad, TRUE);
 raw();
 wgetch(pad);

 delwin(pad);
}

#ifdef PDCURSES
#ifdef __STDC__
void clipboardTest (WINDOW *win)
#else
void clipboardTest (win)
WINDOW *win;
#endif
{
    long i,length=0;
    char *Message1 = "This test will display the contents of the system clipboard";
    char *Message2 = "This test will place the following string in the system clipboard:";
    char *text = "This string placed in clipboard by PDCurses test program, testcurs.";
    char *ptr=NULL;

/* disable typeahead checking */

    typeahead(-1);

    clear();
    mvprintw (1, 1, Message1);
    refresh ();
    move(LINES-1, 1);
    clrtoeol();
    mvaddstr(LINES-1, 1, " Press any key to continue");
    refresh();
    raw();
    getch();
    scrollok(stdscr, TRUE);
    i = PDC_getclipboard( &ptr, &length );
    switch(i)
    {
       case PDC_CLIP_ACCESS_ERROR:
          mvprintw (3, 1, "There was an error accessing the clipboard");
          refresh ();
          break;
       case PDC_CLIP_MEMORY_ERROR:
          mvprintw (3, 1, "Unable to allocate memory for clipboard contents");
          break;
       case PDC_CLIP_EMPTY:
          mvprintw (3, 1, "There was no text in the clipboard");
          break;
       default:
          wsetscrreg (stdscr, 0, LINES-1);
          clear();
          mvprintw (1, 1, "Clipboard contents...");
          move(2,0);
          for (i=0;i<length;i++)
          {
             addch(*(ptr+i));
          }
          addch('\n');
          break;
    }
    move(LINES-1, 1);
    clrtoeol();
    mvaddstr(LINES-1, 1, " Press any key to continue");
    refresh();
    raw();
    getch();

    clear();
    mvprintw (1, 1, Message2);
    mvprintw (2, 1, text);
    refresh ();
    i = PDC_setclipboard( text, strlen(text) );
    switch(i)
    {
       case PDC_CLIP_ACCESS_ERROR:
          mvprintw (3, 1, "There was an error accessing the clipboard");
          refresh ();
          break;
       case PDC_CLIP_MEMORY_ERROR:
          mvprintw (3, 1, "Unable to allocate memory for clipboard contents");
          break;
       default:
          mvprintw (3, 1, "The string was placed in the clipboard successfully");
    }
    move(LINES-1, 1);
    clrtoeol();
    mvaddstr(LINES-1, 1, " Press any key to continue");
    refresh();
    raw();
    getch();
}
#endif

#ifdef __STDC__
void display_menu(int old_option,int new_option)
#else
void display_menu(old_option,new_option)
int old_option,new_option;
#endif
{
 register int i;

#ifdef XCURSES
 attrset(A_PROTECT);
#else
 attrset(A_BOLD);
#endif
 mvaddstr(3,20,"PDCurses Test Program");
 attrset(A_NORMAL);

 for (i=0;i<MAX_OPTIONS;i++)
    mvaddstr(5+i,25,command[i].text);
 if (old_option != (-1))
    mvaddstr(5+old_option,25,command[old_option].text);
 attrset(A_REVERSE);
 mvaddstr(5+new_option,25,command[new_option].text);
 attrset(A_NORMAL);
 mvaddstr(13,3,"Use Up and Down Arrows to select - Enter to run - Q to quit");
 refresh();
}

