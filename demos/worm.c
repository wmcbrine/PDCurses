/*

	 @@@        @@@    @@@@@@@@@@     @@@@@@@@@@@    @@@@@@@@@@@@
	 @@@        @@@   @@@@@@@@@@@@    @@@@@@@@@@@@   @@@@@@@@@@@@@
	 @@@        @@@  @@@@      @@@@   @@@@           @@@@ @@@  @@@@
	 @@@   @@   @@@  @@@        @@@   @@@            @@@  @@@   @@@
	 @@@  @@@@  @@@  @@@        @@@   @@@            @@@  @@@   @@@
	 @@@@ @@@@ @@@@  @@@        @@@   @@@            @@@  @@@   @@@
	  @@@@@@@@@@@@   @@@@      @@@@   @@@            @@@  @@@   @@@
	   @@@@  @@@@     @@@@@@@@@@@@    @@@            @@@  @@@   @@@
	    @@    @@       @@@@@@@@@@     @@@            @@@  @@@   @@@

				 Eric P. Scott
			  Caltech High Energy Physics
				 October, 1980

		Hacks to turn this into a test frame for cursor movement:
			Eric S. Raymond <esr@snark.thyrsus.com>
				January, 1995

		July 1995 (esr): worms is now in living color! :-)

Options:
	-f			fill screen with copies of 'WORM' at start.
	-l <n>			set worm length
	-n <n>			set number of worms
	-t			make worms leave droppings
	-T <start> <end>	set trace interval
	-S			set single-stepping during trace interval
	-N			suppress cursor-movement optimization

  This program makes a good torture-test for the ncurses cursor-optimization
  code.  You can use -T to set the worm move interval over which movement
  traces will be dumped.  The program stops and waits for one character of
  input at the beginning and end of the interval.

  $Id: worm.c,v 1.3 2006/02/16 07:43:17 wmcbrine Exp $
*/

#include <curses.h>
#include <stdlib.h>

static chtype flavor[] = {
	'O', '*', '#', '$', '%', '0', '@',
};

static const short xinc[] = {
	1, 1, 1, 0, -1, -1, -1, 0
},
yinc[] =
{
	-1, 0, 1, 1, 1, 0, -1, -1
};

static struct worm {
	int orientation, head;
	short *xpos, *ypos;
} worm[40];

static const char *field;
static int length = 16, number = 3;
static chtype trail = ' ';

#ifdef TRACE
static int generation, trace_start, trace_end, singlestep;
#endif

static const struct options {
	int nopts;
	int opts[3];
} normal[8]={
	{ 3, { 7, 0, 1 } }, { 3, { 0, 1, 2 } }, { 3, { 1, 2, 3 } },
	{ 3, { 2, 3, 4 } }, { 3, { 3, 4, 5 } }, { 3, { 4, 5, 6 } },
	{ 3, { 5, 6, 7 } }, { 3, { 6, 7, 0 } }
}, upper[8]={
	{ 1, { 1, 0, 0 } }, { 2, { 1, 2, 0 } }, { 0, { 0, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }, { 2, { 4, 5, 0 } },
	{ 1, { 5, 0, 0 } }, { 2, { 1, 5, 0 } }
}, left[8]={
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 2, { 2, 3, 0 } }, { 1, { 3, 0, 0 } }, { 2, { 3, 7, 0 } },
	{ 1, { 7, 0, 0 } }, { 2, { 7, 0, 0 } }
}, right[8]={
	{ 1, { 7, 0, 0 } }, { 2, { 3, 7, 0 } }, { 1, { 3, 0, 0 } },
	{ 2, { 3, 4, 0 } }, { 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 2, { 6, 7, 0 } }
}, lower[8]={
	{ 0, { 0, 0, 0 } }, { 2, { 0, 1, 0 } }, { 1, { 1, 0, 0 } },
	{ 2, { 1, 5, 0 } }, { 1, { 5, 0, 0 } }, { 2, { 5, 6, 0 } },
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }
}, upleft[8]={
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }, { 1, { 3, 0, 0 } },
	{ 2, { 1, 3, 0 } }, { 1, { 1, 0, 0 } }
}, upright[8]={
	{ 2, { 3, 5, 0 } }, { 1, { 3, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 1, { 5, 0, 0 } }
}, lowleft[8]={
	{ 3, { 7, 0, 1 } }, { 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 1, { 1, 0, 0 } }, { 2, { 1, 7, 0 } }, { 1, { 7, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }
}, lowright[8]={
	{ 0, { 0, 0, 0 } }, { 1, { 7, 0, 0 } }, { 2, { 5, 7, 0 } },
	{ 1, { 5, 0, 0 } }, { 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } },
	{ 0, { 0, 0, 0 } }, { 0, { 0, 0, 0 } }
};

static void cleanup(void)
{
	standend();
	refresh();
	curs_set(1);
	endwin();
}

static float ranf(void)
{
	long r = (rand() & 077777);
	return ((float) r / 32768.);
}

int main(int argc, char *argv[])
{
	const struct options *op;
	struct worm *w;
	short **ref, *ip;
	int x, y, n, h, last, bottom;

	for (x = 1; x < argc; x++)
	{
		char *p;
		p = argv[x];
		if (*p == '-')
			p++;
		switch (*p)
		{
		case 'f':
			field = "WORM";
			break;
		case 'l':
			if (++x == argc)
				goto usage;
			if ((length = atoi(argv[x])) < 2 || length > 1024)
			{
				fprintf(stderr,
					"%s: Invalid length\n", *argv);
				exit(EXIT_FAILURE);
			}
			break;
		case 'n':
			if (++x == argc)
				goto usage;
			if ((number = atoi(argv[x])) < 1 || number > 40)
			{
				fprintf(stderr,
					"%s: Invalid number of worms\n",
					*argv);
				exit(EXIT_FAILURE);
			}
			break;
		case 't':
			trail = '.';
			break;
#ifdef TRACE
		case 'S':
			singlestep = TRUE;
			break;
		case 'T':
			trace_start = atoi(argv[++x]);
			trace_end = atoi(argv[++x]);
			break;
		case 'N':	/* declared by ncurses */
			_nc_optimize_enable ^= OPTIMIZE_ALL;
			break;
#endif				/* TRACE */
		default:
		      usage:
			fprintf(stderr, "usage: %s [-field] [-length #] "
				"[-number #] [-trail]\n", *argv);
			exit(EXIT_FAILURE);
		}
	}

#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
#endif
	noecho();
	cbreak();
	nonl();
	keypad(stdscr, TRUE);

	curs_set(0);

	bottom = LINES - 1;
	last = COLS - 1;

#ifdef A_COLOR
	if (has_colors())
	{
		int bg = COLOR_BLACK;
		start_color();

# define SET_COLOR(num, fg) \
	    init_pair(num + 1, fg, bg); \
	    flavor[num] |= COLOR_PAIR(num + 1) | A_BOLD

		SET_COLOR(0, COLOR_GREEN);
		SET_COLOR(1, COLOR_RED);
		SET_COLOR(2, COLOR_CYAN);
		SET_COLOR(3, COLOR_WHITE);
		SET_COLOR(4, COLOR_MAGENTA);
		SET_COLOR(5, COLOR_BLUE);
		SET_COLOR(6, COLOR_YELLOW);
	}
#endif				/* A_COLOR */

	ref = malloc(sizeof(short *) * LINES);
	for (y = 0; y < LINES; y++)
	{
		ref[y] = malloc(sizeof(short) * COLS);
		for (x = 0; x < COLS; x++)
		{
			ref[y][x] = 0;
		}
	}

#ifdef BADCORNER
	/* if addressing the lower right corner doesn't work in your curses */
	ref[bottom][last] = 1;
#endif				/* BADCORNER */

	for (n = number, w = &worm[0]; --n >= 0; w++)
	{
		w->orientation = w->head = 0;

		if ((ip = malloc(sizeof(short) * (length + 1))) == NULL)
		{
			fprintf(stderr, "%s: out of memory\n", *argv);
			exit(EXIT_FAILURE);
		}

		w->xpos = ip;
		for (x = length; --x >= 0;)
			*ip++ = -1;

		if ((ip = malloc(sizeof(short) * (length + 1))) == NULL)
		{
			fprintf(stderr, "%s: out of memory\n", *argv);
			exit(EXIT_FAILURE);
		}

		w->ypos = ip;
		for (y = length; --y >= 0;)
			*ip++ = -1;
	}

	if (field)
	{
		const char *p = field;

		for (y = bottom; --y >= 0;)
		{
			for (x = COLS; --x >= 0;)
			{
				addch((chtype) (*p++));
				if (!*p)
					p = field;
			}
		}
	}

	napms(10);
	refresh();
#ifndef TRACE
	nodelay(stdscr, TRUE);
#endif

	for (;;)
	{
#ifdef TRACE
		if (trace_start || trace_end)
		{
			if (generation == trace_start)
			{
				trace(TRACE_CALLS);
				getch();
			}
			else if (generation == trace_end)
			{
				trace(0);
				getch();
			}

			if (singlestep && generation > trace_start
			    && generation < trace_end)
				getch();

			generation++;
		}
#else
		int ch;

		if ((ch = getch()) > 0)
		{
# ifdef KEY_RESIZE
		    if (ch == KEY_RESIZE)
		    {
#  ifdef PDCURSES
			resize_term(0, 0);
#  endif
			if (last != COLS - 1)
			{
				for (y = 0; y <= bottom; y++)
				{
					ref[y] = realloc(ref[y],
					    sizeof(short) * COLS);

					for (x = last + 1; x < COLS; x++)
						ref[y][x] = 0;
				}

				last = COLS - 1;
			}

			if (bottom != LINES - 1)
			{
				for (y = LINES; y <= bottom; y++)
					free(ref[y]);

				ref = realloc(ref, sizeof(short *) * LINES);

				for (y = bottom + 1; y < LINES; y++)
				{
					ref[y] = malloc(sizeof(short) * COLS);

					for (x = 0; x < COLS; x++)
						ref[y][x] = 0;
				}

				bottom = LINES - 1;
			}
		    }

# endif /* KEY_RESIZE */

		    /* Make it simple to put this into single-step mode, 
		       or resume normal operation - T. Dickey */

		    if (ch == 'q')
		    {
			cleanup();
			exit(EXIT_SUCCESS);
		    }
		    else if (ch == 's')
			nodelay(stdscr, FALSE);
		    else if (ch == ' ')
			nodelay(stdscr, TRUE);
		}

#endif /* TRACE */

		for (n = 0, w = &worm[0]; n < number; n++, w++)
		{
			if ((x = w->xpos[h = w->head]) < 0)
			{
				move(y = w->ypos[h] = bottom, x =
				     w->xpos[h] = 0);
				addch(flavor[n % sizeof(flavor)]);
				ref[y][x]++;
			}
			else
				y = w->ypos[h];

			if (x > last)
				x = last;

			if (y > bottom)
				y = bottom;

			if (++h == length)
				h = 0;

			if (w->xpos[w->head = h] >= 0)
			{
				int x1, y1;

				x1 = w->xpos[h];
				y1 = w->ypos[h];

				if (y1 < LINES
				    && x1 < COLS && --ref[y1][x1] == 0)
				{
					move(y1, x1);
					addch(trail);
				}
			}

			op = &(x == 0 ? (y == 0 ? upleft :
				(y == bottom ? lowleft : left)) :
				(x == last ? (y == 0 ? upright :
				(y == bottom ? lowright : right)) :
				(y == 0 ? upper :
				(y == bottom ? lower : normal))))
				[w->orientation];

			switch (op->nopts)
			{
			case 0:
				cleanup();
				exit(EXIT_SUCCESS);
			case 1:
				w->orientation = op->opts[0];
				break;
			default:
				w->orientation = op->opts[(int)
					 (ranf() * (float) op->nopts)];
			}

			move(y += yinc[w->orientation], x +=
			     xinc[w->orientation]);

			if (y < 0)
				y = 0;

			addch(flavor[n % sizeof(flavor)]);
			ref[w->ypos[h] = y][w->xpos[h] = x]++;
		}
		napms(10);
		refresh();
	}

	return EXIT_SUCCESS;	/* unreachable */
}
