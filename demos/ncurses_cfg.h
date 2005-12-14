/* This file is only used with the ncurses test programs.
 *
 * Have ncurses-5.5 unpacked in your $(HOME) (you don't need to build 
 * it), or edit ncurses_testdir appropriately in the Makefile. Configure 
 * and build XCurses. (Sorry, other ports won't work yet.) Change to 
 * this directory, and:
 *
 * "make ncurses_tests" to start.
 * "make ncurses_clean" when you're done.
 *
 * Builds: bs gdc hanoi knight rain tclock worm ncurses view
 */

#include "../config.h"
#include <curses.h>

#define ExitProgram XCursesExit(),exit

#define HAVE_GETNSTR 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_PANEL_H 1
#define HAVE_LIBPANEL 1

/* Some functions we don't have */

#define __NCURSES_H

#ifdef HAVE_PROTO
WINDOW *getwin(FILE *filep) { return (WINDOW *)NULL; }
int putwin(WINDOW *win, FILE *filep) { return ERR; }
int tigetnum(const char *capname) { return -2; }
int wchgat(WINDOW *win, int n, attr_t attr, short color, const void *opts) { return ERR; }
#else
WINDOW *getwin(filep) { return (WINDOW *)NULL; }
int putwin(win, filep) { return ERR; }
int tigetnum(capname) { return -2; }
int wchgat(win, n, attr, color, opts) { return ERR; }
#endif
