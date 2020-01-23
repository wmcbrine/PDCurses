#include <stdio.h>
#include <wchar.h>
#include <curses.h>
#include <sys/timeb.h>

/* PDCurses/ncurses speed test.  Initializes *curses,  displays
semi-random numbers on the screen for 3000 milliseconds = 3 seconds,
and then tells you how many frames per second you got (i.e.,  how
many complete screen refreshes).

   Results on my somewhat elderly machine,  all in Xubuntu 16.04 :

SDL2: 470 fps in non-wide mode,  87 fps in wide mode
X11 wide mode: 120 fps first time,  180 fps subsequently
X11 non-wide: 120 fps every time
Win32a (run under Wine): 140 fps,  wide and non-wide
Win32 console (run in Wineconsole): 350 fps wide,  40 non-wide
ncurses: 245 fps

   SDL2 uses TrueType (R) fonts in wide mode,  as opposed to simple
bitmapped fonts in 8-bit mode.  So its ability to be blazingly fast
in the latter is unsurprising.

   X11,  I'll guess,  has some sort of optimization for wide-char fonts
(because they get more use),  and perhaps some sort of caching that
would account for why the first load is a little slower.

   Dunno why Win32 console has such a huge variation,  and haven't
tested it under "real" Windows.
*/

int main( const int argc, const char **argv)
{
    unsigned n_frames = 0;
    struct timeb t1, t2;
    int millisec_elapsed = 0;

    initscr();
    cbreak( );
    noecho();
    refresh();
    keypad( stdscr, 1);
    nodelay(stdscr, TRUE);
    ftime( &t1);
    while( millisec_elapsed < 3000 && getch( ) != 'q')
      {
      char buff[11];
      int i, j;
      const int lines = 25, cols = 80;

      n_frames++;
      sprintf( buff, " %9u", (n_frames * 31415926u) % 1000000000u);
      attrset(A_BOLD);
#if 0
      for( i = 0; i < LINES; i++)
         {
         move( i, 0);
         if( i == n_frames % LINES)
            attrset( A_NORMAL);
         for( j = 0; j + 10 < COLS; j += 10)
            addstr( buff);
         }
#endif
      for( i = 0; i < lines; i++)
         {
         move( i, 0);
         if( i == n_frames % lines)
            attrset( A_NORMAL);
         for( j = 0; j + 10 < cols; j += 10)
            addstr( buff);
         }
      ftime( &t2);
      millisec_elapsed = (t2.time - t1.time) * 1000
                        + (t2.millitm - t1.millitm);
      }
    nodelay(stdscr, FALSE);
    move( LINES / 2, COLS / 2 - 20);
    printw( "  %u frames/sec  ", n_frames / 3);
    getch( );
    endwin();
    printf( "%u frames per second\n", n_frames / 3);
    return( 0);
}
