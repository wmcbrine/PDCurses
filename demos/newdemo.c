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
 *  newdemo.c	-	A demo program using PDCurses. The program illustrate
 *  	 		the use of colours for text output.
 *
 *  Hacks by jbuhler@cs.washington.edu on 12/29/96
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <curses.h>

#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif

#if defined(XCURSES)
	char *XCursesProgramName = "newdemo";
#endif

#ifdef PDCDEBUG
char *rcsid_newdemo  = "$Id: newdemo.c,v 1.1 2001/01/10 08:27:39 mark Exp $";
#endif

/*
 *  The Australian map
 */
char    *AusMap[17] =
{
    "                       A ",
    "           AA         AA ",
    "    N.T. AAAAA       AAAA ",
    "     AAAAAAAAAAA  AAAAAAAA ",
    "   AAAAAAAAAAAAAAAAAAAAAAAAA Qld.",
    " AAAAAAAAAAAAAAAAAAAAAAAAAAAA ",
    " AAAAAAAAAAAAAAAAAAAAAAAAAAAAA ",
    " AAAAAAAAAAAAAAAAAAAAAAAAAAAA ",
    "   AAAAAAAAAAAAAAAAAAAAAAAAA N.S.W.",
    "W.A. AAAAAAAAA      AAAAAA Vic.",
    "       AAA   S.A.     AA",
    "                       A  Tas.",
    ""
};

/*
 *  Funny messages
 */

char    *messages[] =
{
    "Hello from the Land Down Under",
    "The Land of crocs, and a big Red Rock",
    "Where the sunflower runs along the highways",
    "The dusty red roads lead one to loneliness",
    "Blue sky in the morning and",
    "Freezing nights and twinkling stars",
    NULL
};

/*
 *  Wait for user
 */
int WaitForUser()
{
 time_t  t;
 chtype ch;

    nodelay(stdscr,TRUE);
    t = time((time_t *)0);
    while(1)
      {   
       if ((ch = getch()) != ERR)
         {   
          if (ch == '\033') 
            {
             nodelay(stdscr, FALSE);
             return  ch;
            }
          else 
            {
             nodelay(stdscr, FALSE);
             return  0;
            }
         }
       if (time((time_t *)0) - t > 5) 
         {
          nodelay(stdscr, FALSE);
          return  0;
         }
      }
}

/*
 * Test sub windows
 */
int SubWinTest(win)
WINDOW *win;
{
int     w, h, sw, sh, bx, by;
WINDOW  *swin1, *swin2, *swin3;

    wattrset(win, 0);
    w  = win->_maxx;
    h  = win->_maxy;
    bx = win->_begx;
    by = win->_begy;
    sw = w / 3;
    sh = h / 3;
    if((swin1 = derwin(win, sh, sw, 3, 5)) == NULL)
        return  1;
    if((swin2 = subwin(win, sh, sw, by+4, bx+8)) == NULL)
        return  1;
    if((swin3 = subwin(win, sh, sw, by+5, bx+11)) == NULL)
	return  1;

    init_pair(8,COLOR_RED,COLOR_BLUE);
    wbkgd(swin1, COLOR_PAIR(8));
    werase(swin1);
    mvwaddstr(swin1, 0, 3, "Sub-window 1");
    wrefresh(swin1);

    init_pair(9,COLOR_CYAN,COLOR_MAGENTA);
    wbkgd(swin2, COLOR_PAIR(9));
    werase(swin2);
    mvwaddstr(swin2, 0, 3, "Sub-window 2");
    wrefresh(swin2);

    init_pair(10,COLOR_YELLOW,COLOR_GREEN);
    wbkgd(swin3, COLOR_PAIR(10));
    werase(swin3);
    mvwaddstr(swin3, 0, 3, "Sub-window 3");
    wrefresh(swin3);

    delwin(swin1);
    delwin(swin2);
    delwin(swin3);
    WaitForUser();
    return  0;
}

/*
 *  Bouncing balls
 */
int BouncingBalls(win)
WINDOW *win;
{
chtype     c1, c2, c3;
int	w, h;
int     x1, y1, xd1, yd1;
int     x2, y2, xd2, yd2;
int     x3, y3, xd3, yd3;
int     c;

#if defined(PDCURSES)
    curs_set(0);
#endif
    wbkgd( win, COLOR_PAIR(1) );
    wrefresh(win);

    w    = win->_maxx;
    h    = win->_maxy;
    x1   = 2 + rand() % (w - 4);
    y1   = 2 + rand() % (h - 4);
    x2   = 2 + rand() % (w - 4);
    y2   = 2 + rand() % (h - 4);
    x3   = 2 + rand() % (w - 4);
    y3   = 2 + rand() % (h - 4);
    xd1  = 1; yd1 = 1;
    xd2  = 1; yd2 = 0;
    xd3  = 0; yd3 = 1;
    nodelay(stdscr,TRUE);
    while((c = getch()) == ERR)
    {   x1 = xd1 > 0 ? ++x1 : --x1;
        if(x1 <= 1 || x1 >= w - 2)
            xd1 = xd1 ? 0 : 1;
        y1 = yd1 > 0 ? ++y1 : --y1;
        if(y1 <= 1 || y1 >= h - 2)
	    yd1 = yd1 ? 0 : 1;

        x2 = xd2 > 0 ? ++x2 : --x2;
        if(x2 <= 1 || x2 >= w - 2)
            xd2 = xd2 ? 0 : 1;
        y2 = yd2 > 0 ? ++y2 : --y2;
        if(y2 <= 1 || y2 >= h - 2)
            yd2 = yd2 ? 0 : 1;

        x3 = xd3 > 0 ? ++x3 : --x3;
        if(x3 <= 1 || x3 >= w - 2)
	    xd3 = xd3 ? 0 : 1;
        y3 = yd3 > 0 ? ++y3 : --y3;
        if(y3 <= 1 || y3 >= h - 2)
            yd3 = yd3 ? 0 : 1;

        c1 = mvwinch(win, y1, x1);
        c2 = mvwinch(win, y2, x2);
        c3 = mvwinch(win, y3, x3);

        init_pair(8,COLOR_RED,COLOR_GREEN);
        wattrset(win, COLOR_PAIR(8));
	mvwaddch(win, y1, x1, 'O');
        init_pair(9,COLOR_BLUE,COLOR_RED);
        wattrset(win, COLOR_PAIR(9));
        mvwaddch(win, y2, x2, '*');
        init_pair(10,COLOR_YELLOW,COLOR_WHITE);
        wattrset(win, COLOR_PAIR(10));
        mvwaddch(win, y3, x3, '@');
        wmove(win, 0, 0);
        wrefresh(win);
        wattrset(win,0);
	mvwaddch(win, y1, x1, c1);
	mvwaddch(win, y2, x2, c2);
	mvwaddch(win, y3, x3, c3);
	napms(150);
    }
    nodelay(stdscr,FALSE);
    ungetch(c);
    return 0;
}

/*
 *  Trap interrupt
 */
void trap()
{
    endwin();
#ifdef XCURSES
    XCursesExit();
#endif
    exit(0);
}

/*
 *  Main driver
 */
int main(int argc, char *argv[])
{
WINDOW  *win;
int     w, x, y, i, j, len;
time_t  t;
char    *message;
int     width, height;
chtype  save[80], ch;
void    trap();

#ifdef PDCDEBUG
/*  traceon();*/
#endif
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
#if !defined(TC) && !defined(OS2)
    signal(SIGINT, trap);
#endif
    noecho();

/*  refresh stdscr so that reading from it will not cause it to overwrite */
/*  the other windows that are being created */

    refresh();

    width  = 48;
    height = 15;                /* Create a drawing window */
    win = newwin(height, width, (LINES-height)/2, (COLS-width)/2);
    if(win == NULL)
    {   
        endwin();
#ifdef XCURSES
        XCursesExit();
#endif
        return 1;
    }

    while(TRUE)
    {   init_pair(1,COLOR_WHITE,COLOR_BLUE);
/*        wattrset(win, COLOR_PAIR(1));*/
        wbkgd(win, COLOR_PAIR(1));
        werase(win);

        init_pair(2,COLOR_RED,COLOR_RED);
        wattrset(win, COLOR_PAIR(2));
        ch = ' ';
        box(win, ch, ch);
        wrefresh(win);
        wattrset(win, 0);
                                /* Do random output of a character */
        ch = 'a';
        nodelay(stdscr,TRUE);
        for(i=0; i < 5000; ++i)
        {   x = rand() % (width-2)  + 1;
            y = rand() % (height-2) + 1;
            mvwaddch(win, y, x, ch);
            wrefresh(win);
            if (getch() != ERR)
                break;
            if(i == 2000)
            {   ch = 'b';
                init_pair(3,COLOR_CYAN,COLOR_YELLOW);
                wattron(win, COLOR_PAIR(3));
            }
        }
        nodelay(stdscr,FALSE);

        SubWinTest(win);
                                /* Erase and draw green window */
        init_pair(4,COLOR_YELLOW,COLOR_GREEN);
/*        wattrset(win, COLOR_PAIR(4) | A_BOLD);*/
        wbkgd(win, COLOR_PAIR(4) );
        wattrset(win, A_BOLD);
        werase(win);
        wrefresh(win);
                                /* Draw RED bounding box */
        wattrset(win, COLOR_PAIR(2));
        box(win, ' ', ' ');
        wrefresh(win);
                                /* Display Australia map */
        wattrset(win, A_BOLD);
        i = 0;
        while(*AusMap[i])
	{   mvwaddstr(win, i+1, 8, AusMap[i]);
            wrefresh(win);
            napms(100);
            ++i;
        }

        init_pair(5,COLOR_BLUE,COLOR_WHITE);
        wattrset(win, COLOR_PAIR(5) | A_BLINK);
	mvwaddstr(win, height-2, 3, " PDCurses 2.4 for DOS, OS/2, X11 and Win32");
	wrefresh(win);

				/* Draw running messages */
	init_pair(6,COLOR_YELLOW,COLOR_WHITE);
	wattrset(win, COLOR_PAIR(6));
	message = messages[0];
	len = strlen(message);
	j = 0;
	i = 2;
	w = width-2;
	nodelay(win,TRUE);

	/* jbuhler's re-hacked scrolling messages */
	for (j=0; messages[j] != NULL; j++)
		{
		char *message = messages[j];
		int msg_len = strlen(message);
		int scroll_len = w + 2 * msg_len;
		char *scrollbuf = malloc(scroll_len);
		char *visbuf = scrollbuf + msg_len;
		int stop = 0;
		int i;

		for (i=1; i <= w + msg_len; i++)
			{
			memset(visbuf, ' ',w);
			strncpy(scrollbuf + i, message, msg_len);
			mvwaddnstr(win,height/2, 1, visbuf, w);
			wrefresh(win);
			if (wgetch(win) != ERR)
				{
				flushinp();
				stop = 1;
				break;
				}
			delay_output(100);
			}
		free(scrollbuf);
		if (stop)
			break;
		}

        j = 0;
                                /*  Draw running As across in RED */
        init_pair(7,COLOR_RED,COLOR_GREEN);
        wattron(win, COLOR_PAIR(7));
	for(i=2; i < width - 4; ++i)
        {   ch = mvwinch(win, 5, i);
            save[j++] = ch;
            ch = ch & 0x7f;
	    mvwaddch(win, 5, i, ch);
        }
        wrefresh(win);

                                /* Put a message up wait for a key */
        i = height-2;
        wattrset(win, COLOR_PAIR(5));
	mvwaddstr(win, i, 3, "   Type a key to continue or ESC to quit  ");
        wrefresh(win);

        if(WaitForUser() == '\033')
	    break;

        j = 0;                  /* Restore the old line */
	wattrset(win,0);
        for(i=2; i < width - 4; ++i)
            mvwaddch(win, 5, i, save[j++]);
        wrefresh(win);

	BouncingBalls(win); /* leaves a keystroke in the queue */
                                
        if(WaitForUser() == '\033')
            break;
    }
    endwin();
#ifdef XCURSES
    XCursesExit();
#endif
    return 0;
}

/*  End of NEWDEMO.C */
