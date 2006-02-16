/* $Id: rain.c,v 1.1 2006/02/16 06:11:39 wmcbrine Exp $ */

#include <curses.h>
#include <signal.h>
#include <stdlib.h>

/* rain 11/3/1980 EPS/CITHEP */

static void onsig(int n)
{
	curs_set(1);
	endwin();
	exit(EXIT_FAILURE);
}

static float ranf(void)
{
	long r = (rand() & 077777);
	return ((float) r / 32768.);
}

static int next_j(int j)
{
	if (j == 0)
		j = 4;
	else
		--j;

	if (has_colors())
	{
		int z = (int) (3 * ranf());
		chtype color = COLOR_PAIR(z);

		if (z)
			color |= A_BOLD;

		attrset(color);
	}

	return j;
}

int main(int argc, char *argv[])
{
	int x, y, j;
	static int xpos[5], ypos[5];
	float r;
	float c;

	for (j = SIGHUP; j <= SIGTERM; j++)
		if (signal(j, SIG_IGN) != SIG_IGN)
			signal(j, onsig);

#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
#endif

	if (has_colors())
	{
		int bg = COLOR_BLACK;

		start_color();
		init_pair(1, COLOR_BLUE, bg);
		init_pair(2, COLOR_CYAN, bg);
	}

	nl();
	noecho();
	curs_set(0);
	timeout(0);
	keypad(stdscr, TRUE);

	r = (float) (LINES - 4);
	c = (float) (COLS - 4);

	for (j = 5; --j >= 0;)
	{
		xpos[j] = (int) (c * ranf()) + 2;
		ypos[j] = (int) (r * ranf()) + 2;
	}

	for (j = 0;;)
	{
		x = (int) (c * ranf()) + 2;
		y = (int) (r * ranf()) + 2;

		mvaddch(y, x, '.');

		mvaddch(ypos[j], xpos[j], 'o');

		j = next_j(j);
		mvaddch(ypos[j], xpos[j], 'O');

		j = next_j(j);
		mvaddch(ypos[j] - 1, xpos[j], '-');
		mvaddstr(ypos[j], xpos[j] - 1, "|.|");
		mvaddch(ypos[j] + 1, xpos[j], '-');

		j = next_j(j);
		mvaddch(ypos[j] - 2, xpos[j], '-');
		mvaddstr(ypos[j] - 1, xpos[j] - 1, "/ \\");
		mvaddstr(ypos[j], xpos[j] - 2, "| O |");
		mvaddstr(ypos[j] + 1, xpos[j] - 1, "\\ /");
		mvaddch(ypos[j] + 2, xpos[j], '-');

		j = next_j(j);
		mvaddch(ypos[j] - 2, xpos[j], ' ');
		mvaddstr(ypos[j] - 1, xpos[j] - 1, "   ");
		mvaddstr(ypos[j], xpos[j] - 2, "     ");
		mvaddstr(ypos[j] + 1, xpos[j] - 1, "   ");
		mvaddch(ypos[j] + 2, xpos[j], ' ');

		xpos[j] = x;
		ypos[j] = y;

		switch (getch())
		{
		case ('q'):
		case ('Q'):
			curs_set(1);
			endwin();
			exit(EXIT_SUCCESS);
		case 's':
			nodelay(stdscr, FALSE);
			break;
		case ' ':
			nodelay(stdscr, TRUE);
			break;
#ifdef KEY_RESIZE
		case (KEY_RESIZE):
# ifdef PDCURSES
			resize_term(0, 0);
# endif
			r = (float) (LINES - 4);
			c = (float) (COLS - 4);
			break;
#endif
		}
		napms(50);
	}
}
