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
**************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <curses.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define DELAYSIZE 200

#ifdef PDCDEBUG
const char *rcsid_firework =
	"$Id: firework.c,v 1.17 2006/02/05 23:54:31 wmcbrine Exp $";
#endif

void myrefresh(void);
void get_colour(void);
void explode(int, int);

int main(int argc, char **argv)
{
	int start, end, row, diff, flag, direction, seed;

#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
#endif
	nodelay(stdscr, TRUE);
	noecho();

	if (has_colors())
		start_color();

	seed = time((time_t *)0);
	srand(seed);
	flag = 0;
       
	while (getch() == ERR)		/* loop until a key is hit */
	{
		do {
			start = rand() % (COLS -3);
			end = rand() % (COLS - 3);
			start = (start < 2) ? 2 : start;
			end = (end < 2) ? 2 : end;
			direction = (start > end) ? -1 : 1;
			diff = abs(start - end);

		} while (diff < 2 || diff >= LINES - 2);

		attrset(A_NORMAL);

		for (row = 0; row < diff; row++)
		{
			mvprintw(LINES - row, row * direction + start,
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
		explode(LINES - row, diff * direction + start);
		clear();
		myrefresh();
	}

	endwin();
#ifdef XCURSES
	XCursesExit();
#endif
	return 0;
}

void explode(int row, int col)
{
	clear();
	mvprintw(row, col, "-");
	myrefresh();

	--col;

	get_colour();
	mvprintw(row - 1, col, " - ");
	mvprintw(row,     col, "-+-");
	mvprintw(row + 1, col, " - ");
	myrefresh();

	--col;

	get_colour();
	mvprintw(row - 2, col, " --- ");
	mvprintw(row - 1, col, "-+++-");
	mvprintw(row,     col, "-+#+-");
	mvprintw(row + 1, col, "-+++-");
	mvprintw(row + 2, col, " --- ");
	myrefresh();

	get_colour();
	mvprintw(row - 2, col, " +++ ");
	mvprintw(row - 1, col, "++#++");
	mvprintw(row,     col, "+# #+");
	mvprintw(row + 1, col, "++#++");
	mvprintw(row + 2, col, " +++ ");
	myrefresh();

	get_colour();
	mvprintw(row - 2, col, "  #  ");
	mvprintw(row - 1, col, "## ##");
	mvprintw(row,     col, "#   #");
	mvprintw(row + 1, col, "## ##");
	mvprintw(row + 2, col, "  #  ");
	myrefresh();

	get_colour();
	mvprintw(row - 2, col, " # # ");
	mvprintw(row - 1, col, "#   #");
	mvprintw(row,     col, "     ");
	mvprintw(row + 1, col, "#   #");
	mvprintw(row + 2, col, " # # ");
	myrefresh();
}

void myrefresh(void)
{
	napms(DELAYSIZE);
	move(LINES - 1, COLS - 1);
	refresh();
}

void get_colour(void)
{
	static short tbl[] =
	{
		COLOR_RED, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN,
		COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE
	};

	chtype bold = (rand() % 2) ? A_BOLD : A_NORMAL;
	init_pair(1, tbl[rand() % 8], COLOR_BLACK);
	attrset(COLOR_PAIR(1) | bold);
}
