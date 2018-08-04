#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
static struct termios orig_term;
#endif
#include <stdint.h>
#include <assert.h>
#include "curspriv.h"
#include "pdcvt.h"

int PDC_rows = -1, PDC_cols = -1;
bool PDC_resize_occurred = FALSE;
const int STDIN = 0;
chtype PDC_capabilities = 0;

/* COLOR_PAIR to attribute encoding table. */

static short *color_pair_indices = (short *)NULL;
COLORREF *pdc_rgbs = (COLORREF *)NULL;

unsigned long pdc_key_modifiers = 0L;

void PDC_reset_prog_mode( void)
{
}

void PDC_reset_shell_mode( void)
{
}

static int initial_PDC_rows, initial_PDC_cols;

int PDC_resize_screen(int nlines, int ncols)
{
   if( PDC_rows == -1)     /* initscr( ) hasn't been called;  we're just */
      {                    /* setting desired size at startup */
      initial_PDC_rows = nlines;
      initial_PDC_cols = ncols;
      }
   else if( nlines > 1 && ncols > 1)
      {
      printf( "\033[8;%d;%dt", nlines, ncols);
      PDC_rows = nlines;
      PDC_cols = ncols;
      }
   return( 0);
}

void PDC_restore_screen_mode(int i)
{
}

void PDC_save_screen_mode(int i)
{
}

void PDC_scr_close( void)
{
   printf( "\0338");         /* restore cursor & attribs (VT100) */
   printf( "\033[m");         /* set default screen attributes */
   PDC_gotoyx( PDC_cols - 1, 0);
   printf( "\033[?1000l\n");        /* turn off mouse events */
#ifndef _WIN32
   tcsetattr( STDIN, TCSANOW, &orig_term);
#endif
   return;
}


void PDC_scr_free( void)
{
    if (SP)
        free(SP);
    SP = (SCREEN *)NULL;

    if (color_pair_indices)
        free(color_pair_indices);
    color_pair_indices = (short *)NULL;

    if (pdc_rgbs)
        free(pdc_rgbs);
    pdc_rgbs = (COLORREF *)NULL;
}

#ifndef _WIN32
static void sigwinchHandler( int sig)
{
   struct winsize ws;

   if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != -1)
      if( PDC_rows != ws.ws_row || PDC_cols != ws.ws_col)
         {
         PDC_rows = ws.ws_row;
         PDC_cols = ws.ws_col;
         PDC_resize_occurred = TRUE;
         }
}
#endif

#define MAX_LINES 1000
#define MAX_COLUMNS 1000
#define N_COLORS 256

int PDC_scr_open(int argc, char **argv)
{
#ifndef _WIN32
    struct sigaction sa;
    struct termios term;
#endif
    int i, r, g, b;
    char *capabilities = getenv( "PDC_VT");

    if( capabilities)      /* these should really come from terminfo! */
       {
       if( strstr( capabilities, "RGB"))
          PDC_capabilities |= A_RGB_COLOR;
       if( strstr( capabilities, "UND"))
          PDC_capabilities |= A_UNDERLINE;
       if( strstr( capabilities, "BLI"))
          PDC_capabilities |= A_BLINK;
       if( strstr( capabilities, "DIM"))
          PDC_capabilities |= A_DIM;
       if( strstr( capabilities, "STA"))
          PDC_capabilities |= A_STANDOUT;
       }
    SP = calloc(1, sizeof(SCREEN));
    color_pair_indices = (short *)calloc(PDC_COLOR_PAIRS * 2, sizeof( short));
    pdc_rgbs = (COLORREF *)calloc(N_COLORS, sizeof( COLORREF));
    if (!SP || !color_pair_indices || !pdc_rgbs)
        return ERR;

    COLORS = N_COLORS;  /* should give this a try and see if it works! */
    for( i = 0; i < 16; i++)
    {
        const int intensity = ((i & 8) ? 0xff : 0xc0);

        pdc_rgbs[i] = RGB( ((i & COLOR_RED) ? intensity : 0),
                           ((i & COLOR_GREEN) ? intensity : 0),
                           ((i & COLOR_BLUE) ? intensity : 0));
    }
           /* 256-color xterm extended palette:  216 colors in a
            6x6x6 color cube,  plus 24 (not 50) shades of gray */
    for( r = 0; r < 6; r++)
        for( g = 0; g < 6; g++)
            for( b = 0; b < 6; b++)
                pdc_rgbs[i++] = RGB( r ? r * 40 + 55 : 0,
                                   g ? g * 40 + 55 : 0,
                                   b ? b * 40 + 55 : 0);
    for( i = 0; i < 24; i++)
        pdc_rgbs[i + 232] = RGB( i * 10 + 8, i * 10 + 8, i * 10 + 8);
    setbuf( stdin, NULL);
    setbuf( stdout, NULL);
#ifndef _WIN32
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigwinchHandler;
    if (sigaction(SIGWINCH, &sa, NULL) == -1)
    {
        fprintf( stderr, "Sigaction failed\n");
        return( -1);
    }
    sigwinchHandler( 0);
#endif
    SP->mouse_wait = PDC_CLICK_PERIOD;
    SP->visibility = 0;                /* no cursor,  by default */
    SP->curscol = SP->cursrow = 0;
    SP->audible = TRUE;
    SP->mono = FALSE;


#ifdef _WIN32
    PDC_rows = 25;
    PDC_cols = 80;
#else
    while( PDC_get_rows( ) < 1 && PDC_get_columns( ) < 1)
      ;     /* wait for screen to be drawn and size determined */
#endif
    if( initial_PDC_rows > 1 && initial_PDC_cols > 1)
        {
        PDC_resize_screen( initial_PDC_rows, initial_PDC_cols);
        while( PDC_get_rows( ) != initial_PDC_rows
            && PDC_get_columns( ) != initial_PDC_rows)
           ;
        }

    SP->lines = PDC_get_rows();
    SP->cols = PDC_get_columns();

    if (SP->lines < 2 || SP->lines > MAX_LINES
       || SP->cols < 2 || SP->cols > MAX_COLUMNS)
    {
        fprintf(stderr, "LINES value must be >= 2 and <= %d: got %d\n",
                MAX_LINES, SP->lines);
        fprintf(stderr, "COLS value must be >= 2 and <= %d: got %d\n",
                MAX_COLUMNS, SP->cols);

        return ERR;
    }

#ifndef _WIN32
    tcgetattr( STDIN, &orig_term);
    memcpy( &term, &orig_term, sizeof( term));
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr( STDIN, TCSANOW, &term);
#endif
    printf( "\033[?1000h");    /* enable mouse events,  at least on xterm */
               /* NOTE: could send 1003h to get mouse motion events as well */
    printf( "\0337");         /* save cursor & attribs (VT100) */
    PDC_resize_occurred = FALSE;
/*  PDC_reset_prog_mode();   doesn't do anything anyway */
    return( 0);
}

int PDC_set_function_key( const unsigned function, const int new_key)
{
   return( 0);
}

void PDC_set_resize_limits( const int new_min_lines,
                            const int new_max_lines,
                            const int new_min_cols,
                            const int new_max_cols)
{
   return;
}

/* PDC_init_color(), PDC_init_pair(),  and PDC_set_blink() all share a common
issue : after adjusting the display characteristic in question,  all relevant
text should be redrawn.  Call PDC_init_pair( 3, ...),  and all text using
color pair 3 should be redrawn;  call PDC_init_color( 5, ...) and all text
using color index 5 for either foreground or background should be redrawn;
turn "real blinking" on/off,  and all blinking text should be redrawn.
(On platforms where blinking text is controlled by a timer and redrawn every
half second or so,  such as X11,  SDLx,  and Win32a,  this function can be
used for that purpose as well.)

   PDC_show_changes( ) will look for relevant chains of text and redraw them.
For speed/simplicity,  the code looks for the first and last character in
each line that would be affected, then draws those in between.  Often --
perhaps usually -- this will be zero characters, i.e., no text on that
particular line happens to have an attribute requiring redrawing. */

static short get_pair( const chtype ch)

{
   return( (short)( (ch & A_COLOR) >> PDC_COLOR_SHIFT) & (COLOR_PAIRS - 1));
}

static int color_used_for_this_char( const chtype c, const int idx)
{
    const int color = get_pair( c);
    const int rval = (color_pair_indices[color] == idx ||
                     color_pair_indices[color + PDC_COLOR_PAIRS] == idx);

    return( rval);
}

void PDC_show_changes( const short pair, const short idx, const chtype attr)
{
    if( curscr && curscr->_y)
    {
        int i;

        for( i = 0; i < SP->lines - 1; i++)
            if( curscr->_y[i])
            {
                int j = 0, n_chars;
                chtype *line = curscr->_y[i];

         /* skip over starting text that isn't changed : */
                while( j < SP->cols && get_pair( *line) != pair
                       && !color_used_for_this_char( *line, idx)
                       && !(attr & *line))
                {
                    j++;
                    line++;
                }
                n_chars = SP->cols - j;
        /* then skip over text at the end that's not the right color: */
                while( n_chars && get_pair( line[n_chars - 1]) != pair
                       && !color_used_for_this_char( line[n_chars - 1], idx)
                       && !(attr & line[n_chars - 1]))
                    n_chars--;
                assert( n_chars >= 0);
                if( n_chars)
                    PDC_transform_line( i, j, n_chars, line);
            }
    }
}

void PDC_init_pair( short pair, short fg, short bg)
{
    if( color_pair_indices[pair] != fg ||
        color_pair_indices[pair + PDC_COLOR_PAIRS] != bg)
    {
        color_pair_indices[pair] = fg;
        color_pair_indices[pair + PDC_COLOR_PAIRS] = bg;
        PDC_show_changes( pair, -1, 0);
    }
}

int PDC_pair_content( short pair, short *fg, short *bg)
{
    *fg = color_pair_indices[pair];
    *bg = color_pair_indices[pair + PDC_COLOR_PAIRS];
    return OK;
}

bool PDC_can_change_color(void)
{
    return TRUE;
}

int PDC_color_content( short color, short *red, short *green, short *blue)
{
    COLORREF col = pdc_rgbs[color];

    *red = DIVROUND(GetRValue(col) * 1000, 255);
    *green = DIVROUND(GetGValue(col) * 1000, 255);
    *blue = DIVROUND(GetBValue(col) * 1000, 255);

    return OK;
}

int PDC_init_color( short color, short red, short green, short blue)
{
    const COLORREF new_rgb = RGB(DIVROUND(red * 255, 1000),
                                 DIVROUND(green * 255, 1000),
                                 DIVROUND(blue * 255, 1000));

    if( pdc_rgbs[color] != new_rgb)
    {
        pdc_rgbs[color] = new_rgb;
        PDC_show_changes( -1, color, 0);
    }
    return OK;
}
