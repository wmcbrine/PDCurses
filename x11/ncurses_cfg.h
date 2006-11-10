/* $Id: ncurses_cfg.h,v 1.2 2006/11/10 11:45:08 wmcbrine Exp $
 *
 * This file is only used with the ncurses test programs.
 *
 * Have ncurses-5.5 unpacked in your $(HOME) (you don't need to build 
 * it), or edit ncurses_testdir appropriately in the Makefile. Configure 
 * and build XCurses. (Sorry, other ports won't work yet.) Change to 
 * this directory, and:
 *
 * "make ncurses_tests" to start.
 * "make ncurses_clean" when you're done.
 *
 * Builds: bs gdc hanoi knight tclock ncurses
 */

#include "../config.h"
#include <curses.h>

#define ExitProgram exit

#define HAVE_GETNSTR 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_PANEL_H 1
#define HAVE_LIBPANEL 1
#define HAVE_GETCURX 1
#define HAVE_GETBEGX 1
#define HAVE_GETMAXX 1

/* Fool ncurses.c so it gives us all the tests, and doesn't redefine 
   ACS_ chars
*/

#define NCURSES_VERSION PDCURSES
