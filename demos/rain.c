/* $Id: rain.c,v 1.5 2006/02/18 23:52:07 wmcbrine Exp $ */

#include <curses.h>
#include <stdlib.h>

/* rain 11/3/1980 EPS/CITHEP */

static int next_j(int j)
{
	if (j == 0)
		j = 4;
	else
		--j;

	if (has_colors())
	{
		int z = rand() % 3;
		chtype color = COLOR_PAIR(z);

		if (z)
			color |= A_BOLD;

		attrset(color);
	}

	return j;
}

int main(int argc, char *argv[])
{
	int x, y, j, r, c;
	static int xpos[5], ypos[5];

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

	r = LINES - 4;
	c = COLS - 4;

	for (j = 5; --j >= 0;)
	{
		xpos[j] = rand() % c + 2;
		ypos[j] = rand() % r + 2;
	}

	for (j = 0;;)
	{
		x = rand() % c + 2;
		y = rand() % r + 2;

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
		case 'q':
		case 'Q':
			curs_set(1);
			endwin();
			exit(EXIT_SUCCESS);
		case 's':
			nodelay(stdscr, FALSE);
			break;
		case ' ':
			nodelay(stdscr, TRUE);
#ifdef KEY_RESIZE
			break;
		case KEY_RESIZE:
# ifdef PDCURSES
			resize_term(0, 0);
			erase();
# endif
			r = LINES - 4;
			c = COLS - 4;
#endif
		}
		napms(50);
	}

	return EXIT_SUCCESS;	/* unreachable */
}
