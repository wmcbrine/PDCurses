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
/*+-------------------------------------------------------------------------
	ptest.c
--------------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef PDCDEBUG
char *rcsid_ptest = "$Id: ptest.c,v 1.11 2006/01/11 23:19:52 wmcbrine Exp $";
#endif

#include <curses.h>
#include <panel.h>

PANEL *p1, *p2, *p3, *p4, *p5;
WINDOW *w1, *w2, *w3, *w4, *w5;

long nap_msec = 1;

char *mod[] = 
{
	"test ", "TEST ", "(**) ", "*()* ", "<--> ", "LAST "
};

#if defined(HAVE_PROTO) && !defined(__STDC__)
# define __STDC__ 1
#endif

#ifndef Args
# if __STDC__
#  define Args(x) x
# else
#  define Args(x) ()
# endif
#endif

PANEL *mkpanel Args((int, int, int, int));
void fill_panel Args((PANEL *));
void pflush Args((void));
void wait_a_while Args((long));
void saywhat Args((const char *));
void rmpanel Args((PANEL *));

/*+-------------------------------------------------------------------------
	wait_a_while(msec)
--------------------------------------------------------------------------*/
#if __STDC__
void wait_a_while(long msec)
#else
void wait_a_while(msec)
long msec;
#endif
{
	/* timeout(msec); */
	getch();
}

/*+-------------------------------------------------------------------------
	saywhat(text)
--------------------------------------------------------------------------*/
#if __STDC__
void saywhat(const char *text)
#else
void saywhat(text)
char *text;
#endif
{
	wmove(stdscr, LINES - 1, 0);
	wprintw(stdscr, "%-20.20s", text);
}

/*+-------------------------------------------------------------------------
  mkpanel(rows, cols, tly, tlx) - alloc a win and panel and associate them
--------------------------------------------------------------------------*/
#if __STDC__
PANEL *mkpanel(int rows, int cols, int tly, int tlx)
#else
PANEL *mkpanel(rows, cols, tly, tlx)
int rows;
int cols;
int tly;
int tlx;
#endif
{
	WINDOW *win = newwin(rows, cols, tly, tlx);
	PANEL *pan = (PANEL *)0;

	if (win)
	{
		pan = new_panel(win);

		if (!pan)
			delwin(win);
	}

	return pan;
}

/*+-------------------------------------------------------------------------
	rmpanel(pan)
--------------------------------------------------------------------------*/
#if __STDC__
void rmpanel(PANEL *pan)
#else
void rmpanel(pan)
PANEL *pan;
#endif
{
	WINDOW *win = pan->win;

	del_panel(pan);
	delwin(win);
}

/*+-------------------------------------------------------------------------
	pflush()
--------------------------------------------------------------------------*/
void pflush Args((void))
{
	update_panels();
	doupdate();
}

/*+-------------------------------------------------------------------------
	fill_panel(win)
--------------------------------------------------------------------------*/
#if __STDC__
void fill_panel(PANEL *pan)
#else
void fill_panel(pan)
PANEL *pan;
#endif
{
	WINDOW *win = pan->win;
	char num = *((char *)pan->user + 1);
	int y, x, maxy, maxx;

	box(win, 0, 0);  
	wmove(win, 1, 1);
	wprintw(win, "-pan%c-", num);
	getmaxyx(win, maxy, maxx);

	for (y = 2; y < maxy - 1; y++)
		for (x = 1; x < maxx - 1; x++)
		{
			wmove(win, y, x);
			waddch(win, num);
		}
}

/*+-------------------------------------------------------------------------
	main(argc,argv)
--------------------------------------------------------------------------*/
#if __STDC__
int main(int argc, char **argv)
#else
int main(argc, argv)
int argc;
char **argv;
#endif
{
	int itmp, y,x;

	if ((argc > 1) && atol(argv[1]))
		nap_msec = atol(argv[1]);

#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
#endif

	for (y = 0; y < LINES - 1; y++)
		for (x = 0; x < COLS; x++)
			wprintw(stdscr, "%d", (y + x) % 10);

	for (y = 0; y < 5; y++)
	{
		p1 = mkpanel(10, 10, 0, 0);
		w1 = panel_window(p1);
		set_panel_userptr(p1, "p1");

		p2 = mkpanel(14, 14, 5, 5);
		w2 = panel_window(p2);
		set_panel_userptr(p2, "p2");

		p3 = mkpanel(6, 8, 12, 12);
		w3 = panel_window(p3);
		set_panel_userptr(p3, "p3");

		p4 = mkpanel(10, 10, 10, 30);
		w4 = panel_window(p4);
		set_panel_userptr(p4, "p4");

		p5 = mkpanel(10, 10, 13, 37);
		w5 = panel_window(p5);
		set_panel_userptr(p5, "p5");

		fill_panel(p1);
		fill_panel(p2);
		fill_panel(p3);
		fill_panel(p4);
		fill_panel(p5);
		hide_panel(p4);
		hide_panel(p5);
		pflush();
		wait_a_while(nap_msec);

		saywhat("h3 s1 s2 s4 s5;");
		move_panel(p1, 0, 0);
		hide_panel(p3);
		show_panel(p1);
		show_panel(p2);
		show_panel(p4);
		show_panel(p5);
		pflush();
		wait_a_while(nap_msec);

		saywhat("s1;");
		show_panel(p1);
		pflush();
		wait_a_while(nap_msec);

		saywhat("s2;");
		show_panel(p2);
		pflush();
		wait_a_while(nap_msec);

		saywhat("m2;");
		move_panel(p2, 10, 10);
		pflush();
		wait_a_while(nap_msec);

		saywhat("s3;");
		show_panel(p3);
		pflush();
		wait_a_while(nap_msec);

		saywhat("m3;");
		move_panel(p3, 5, 5);
		pflush();
		wait_a_while(nap_msec);

		saywhat("b3;");
		bottom_panel(p3);
		pflush();
		wait_a_while(nap_msec);

		saywhat("s4;");
		show_panel(p4);
		pflush();
		wait_a_while(nap_msec);

		saywhat("s5;");
		show_panel(p5);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t3;");
		top_panel(p3);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t1;");
		top_panel(p1);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t2;");
		top_panel(p2);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t3;");
		top_panel(p3);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t4;");
		top_panel(p4);
		pflush();
		wait_a_while(nap_msec);

		for (itmp = 0; itmp < 6; itmp++)
		{
			saywhat("m4;");
			wmove(w4, 3, 1);
			waddstr(w4, mod[itmp]);
			move_panel(p4, 4, itmp * 10);
			wmove(w5, 4, 1);
			waddstr(w5, mod[itmp]);
			pflush();
			wait_a_while(nap_msec);
			saywhat("m5;");
			wmove(w4, 4, 1);
			waddstr(w4, mod[itmp]);
			move_panel(p5, 7, itmp * 10 + 6);
			wmove(w5, 3, 1);
			waddstr(w5, mod[itmp]);
			pflush();
			wait_a_while(nap_msec);
		}

		saywhat("m4;");
		move_panel(p4, 4, itmp * 10);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t5;");
		top_panel(p5);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t2;");
		top_panel(p2);
		pflush();
		wait_a_while(nap_msec);

		saywhat("t1;");
		top_panel(p1);
		pflush();
		wait_a_while(nap_msec);

		saywhat("d2;");
		rmpanel(p2);
		pflush();
		wait_a_while(nap_msec);

		saywhat("h3;");
		hide_panel(p3);
		pflush();
		wait_a_while(nap_msec);

		saywhat("d1;");
		rmpanel(p1);
		pflush();
		wait_a_while(nap_msec);

		saywhat("d4; ");
		rmpanel(p4);
		pflush();
		wait_a_while(nap_msec);

		saywhat("d5; ");
		rmpanel(p5);
		pflush();
		wait_a_while(nap_msec);

		if (nap_msec == 1)
			break;

		nap_msec = 100L;
	}

	endwin();
#ifdef XCURSES
	XCursesExit();
#endif
	return 0;
}	/* end of main */

/* end of ptest.c */
