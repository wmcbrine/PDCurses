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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <curses.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#define DELAYSIZE 200

#if defined(XCURSES)
	char *XCursesProgramName = "firework";
#endif

#ifdef PDCDEBUG
char *rcsid_firework  = "$Id: firework.c,v 1.1 2001/01/10 08:27:38 mark Exp $";
#endif

int main()
{
       int start,end,row,diff,flag,direction,seed;
       int myrefresh();
       void explode();

       initscr();
       nodelay( stdscr, TRUE );
       noecho();
       if (has_colors())
          start_color();
       seed = time((time_t *)0);
       srand(seed);
       while(getch() == ERR)  /* loop until a key is hit */
               {
                do {
                       start = rand() % (COLS -3);
                       end = rand() % (COLS - 3);
                       start = (start < 2) ? 2 : start;
                       end = (end < 2) ? 2 : end;
                       direction = (start > end) ? -1 : 1;
                       diff = abs(start-end);
                   } while (diff<2 || diff>=LINES-2);
                attrset(A_NORMAL);
                for (row=0;row<diff;row++)
                       {
                        mvprintw(LINES - row,start + (row * direction),
                               (direction < 0) ? "\\" : "/");
                        if (flag++)
                               {
                                myrefresh();
                                clear();
                                flag = 0;
                               }
                       }
               if (flag++)
                       {
                        myrefresh();
                        flag = 0;
                       }
               seed = time((time_t *)0);
               srand(seed);
               explode(LINES-row,start+(diff*direction));
               clear();
               myrefresh();
              }
       endwin();
#ifdef XCURSES
       XCursesExit();
#endif
       return(0);
}
void explode(row,col)
int row,col;
{
       clear();
       mvprintw(row,col,"-");
       myrefresh();

       init_pair(1,get_colour(),COLOR_BLACK);
       attrset(COLOR_PAIR(1));
       mvprintw(row-1,col-1," - ");
       mvprintw(row,col-1,"-+-");
       mvprintw(row+1,col-1," - ");
       myrefresh();

       init_pair(1,get_colour(),COLOR_BLACK);
       attrset(COLOR_PAIR(1));
       mvprintw(row-2,col-2," --- ");
       mvprintw(row-1,col-2,"-+++-");
       mvprintw(row,  col-2,"-+#+-");
       mvprintw(row+1,col-2,"-+++-");
       mvprintw(row+2,col-2," --- ");
       myrefresh();

       init_pair(1,get_colour(),COLOR_BLACK);
       attrset(COLOR_PAIR(1));
       mvprintw(row-2,col-2," +++ ");
       mvprintw(row-1,col-2,"++#++");
       mvprintw(row,  col-2,"+# #+");
       mvprintw(row+1,col-2,"++#++");
       mvprintw(row+2,col-2," +++ ");
       myrefresh();

       init_pair(1,get_colour(),COLOR_BLACK);
       attrset(COLOR_PAIR(1));
       mvprintw(row-2,col-2,"  #  ");
       mvprintw(row-1,col-2,"## ##");
       mvprintw(row,  col-2,"#   #");
       mvprintw(row+1,col-2,"## ##");
       mvprintw(row+2,col-2,"  #  ");
       myrefresh();

       init_pair(1,get_colour(),COLOR_BLACK);
       attrset(COLOR_PAIR(1));
       mvprintw(row-2,col-2," # # ");
       mvprintw(row-1,col-2,"#   #");
       mvprintw(row,  col-2,"     ");
       mvprintw(row+1,col-2,"#   #");
       mvprintw(row+2,col-2," # # ");
       myrefresh();
       return;
}
int myrefresh()
{
       napms(DELAYSIZE);
       move(LINES-1,COLS-1);
       refresh();
}

int get_colour()
{
 int attr, tmp;

       attr = (rand() % 8)+1;

       if (attr == 1)
          attr = COLOR_RED;  /* use red twice */
       else if (attr == 2)
          attr = COLOR_BLUE;
       else if (attr == 3)
          attr = COLOR_GREEN;
       else if (attr == 4)
          attr = COLOR_CYAN;
       else if (attr == 5)
          attr = COLOR_RED;
       else if (attr == 6)
          attr = COLOR_MAGENTA;
       else if (attr == 7)
          attr = COLOR_YELLOW;
       else if (attr == 8)
          attr = COLOR_WHITE;

       tmp = (rand() % 2)+1;
       if (tmp > 1)
          attr |= A_BOLD;
       return(attr);
}
