/*
 *  newtest.c   -   Tests certain PDCurses functions,
 *    mostly those in Win32a,  including some of the
 *    new attributes for 64-bit chtype.  I wanted to be
 *    sure the PDC_set_blink and PDC_set_line_color
 *    functions worked,  and that A_OVERLINE and A_STRIKEOUT
 *    displayed properly.  Also tests "extended" SLK functions.
 *
 */
#ifndef _XOPEN_SOURCE_EXTENDED
# define _XOPEN_SOURCE_EXTENDED 1
#endif

#ifdef PDC_WIDE
   #define HAVE_WIDE
   #include <wchar.h>
   #include <curses.h>
#endif
#ifdef HAVE_NCURSESW
   #define HAVE_WIDE
   #include <wchar.h>
   #include <ncursesw/curses.h>
#endif

#ifndef HAVE_WIDE
   #include <curses.h>
#endif

#include <string.h>
#include <stdio.h>
#include <locale.h>

int PDC_write_screen_to_file( const char *filename, WINDOW *win);

static const char *labels[] = {
               "Quit", "Blink", "No labels", "431", "2134", "55",
               "62-really-longer-than-it-should-be-just-for-testing",
               "83", "7", "b", "25",
               "Able", "Baker", "Charlie", "Dog",
               "Easy", "Fox", "Golf", "How", "Item",
               "Jig", "King", "Love", "Mike", "Nan",
               "Oboe", "Peter", "Queen", "Roger", "Sugar",
               "Tear", "Uncle", "Victor", "Whiskey",
               "X-Ray", "Yoke", "Zebra", NULL };


static void slk_setup( const int slk_format)
{
    int i;
#ifdef PDCURSES
    static int old_format = 0xa;

    if( slk_format != old_format)
       slk_init( slk_format);
    old_format = slk_format;
#endif
    for( i = 0; labels[i]; i++)
       slk_set( i + 1, labels[i], 1);
    slk_refresh( );
}

   /* Uses the left/right/under/overline capabilities of Win32a */
   /* to ensure the text is "boxed".  */

void text_in_a_box( const char *istr)
{
   const int len = (int)strlen( istr);

#ifdef CHTYPE_LONG
   attron( A_OVERLINE | A_UNDERLINE | A_LEFTLINE);
   if( len == 1)
      attron( A_RIGHTLINE);
#endif
   addnstr( istr, 1);
   if( len > 1)
      {
#ifdef CHTYPE_LONG
      attroff( A_LEFTLINE);
#endif
      if( len > 2)
         addnstr( istr + 1, len - 2);
#ifdef CHTYPE_LONG
      attron( A_RIGHTLINE);
#endif
      addnstr( istr + len - 1, 1);
      }
#ifdef CHTYPE_LONG
   attroff( A_OVERLINE | A_UNDERLINE | A_LEFTLINE | A_RIGHTLINE);
#endif
}

#define COL1 2
#define COL2 (COL1 + 30)
#define COL3 72

#define N_CURSORS 9
      /* There are nine different cursor types;  see below for the list.  */
      /* You specify two types,  and the cursor blinks between them.      */
      /* Default is between "underline" and "invisible".  Set both states */
      /* to the same value to get an unblinking cursor.                   */

#ifdef _WIN32
#define PURE_WINDOWS_VERSION  1
#endif

/* Among other things,  'newtest' demonstrates how to make a Win32a
PDCurses app that is a for-real,  "pure Windows" version (instead of
a console application).  Doing this is quite easy,  and has certain
advantages.  If the app is invoked from a command prompt,  the only
difference you'll see is that the app runs separately (that is,  you
can continue to use the command prompt,  switching between it,  your
PDCurses/Win32a app,  and other processes).  Which is the main reason
I did it;  it meant that I could invoke a PDCurses-based text editor,
for example,  and still have use of the command line.

   (NOTE that,  for reasons I don't actually understand,  this happens
when the Visual C++ compiler is used.  With MinGW or OpenWatcom,  it's
still "really" a console app.)

   To do it,  we ensure that the usual main() function has an alternative
dummy_main() form,  taking the same arguments as main().  We add a
WinMain() function,  whose sole purpose is to reformulate lpszCmdLine
into argc/argv form,  and pass it on to dummy_main().  And,  of course,
we can switch back to a "normal" console app by removing the above
#define PURE_WINDOWS_VERSION line.             */

#ifdef PURE_WINDOWS_VERSION
#undef MOUSE_MOVED
#include <windows.h>

int dummy_main( int argc, char **argv);

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
   char *argv[30];
   int i, argc = 1;

   argv[0] = "newtest";
   for( i = 0; lpszCmdLine[i]; i++)
       if( lpszCmdLine[i] != ' ' && (!i || lpszCmdLine[i - 1] == ' '))
          argv[argc++] = lpszCmdLine + i;

   for( i = 0; lpszCmdLine[i]; i++)
       if( lpszCmdLine[i] == ' ')
          lpszCmdLine[i] = '\0';

   return dummy_main( argc, (char **)argv);
}

int dummy_main( int argc, char **argv)
#else       /* "usual",  console-app version: */
int main( int argc, char **argv)
#endif
{
    int quit = 0, i,  use_slk = 1;
#ifdef PDCURSES
    bool blink_state = FALSE;
    int fmt = 0xa;
#else
    int fmt = 3;   /* for ncurses,  this is the 4-4-4 SLK format */
#endif
    int cursor_state_1 = 0, cursor_state_2 = 1;
    int cursor_y = 19, cursor_x = 51;
    int show_slk_index_line = 0;
    int redraw = 1;
    unsigned extra_character_to_show = 0;
#ifdef HAVE_WIDE
    unsigned unicode_offset = 0x80;
#endif

    setlocale(LC_ALL, "");
    ttytype[0] = 25;   ttytype[1] = 90;         /* Allow 25 to 90 lines... */
    ttytype[2] = 80;   ttytype[3] = (char)200;  /* ...and 80 to 200 columns */
         /* (This program gets weird artifacts when smaller than 25x80.) */
    for( i = 1; i < argc; i++)
        if( argv[i][0] == '-')
            switch( argv[i][1])
            {
                case 's':
                    use_slk = 0;
                    break;
                case 'l':
                    setlocale( LC_ALL, argv[i] + 2);
                    break;
                case 'e':
                    sscanf( argv[i] + 2, "%x", &extra_character_to_show);
                    break;
                case 'f':
                    sscanf( argv[i] + 2, "%x", (unsigned *)&fmt);
                    break;
                case 'i':
                    show_slk_index_line = 1;
                    break;
                case 'r':     /* allow user-resizable windows */
                    {
                        int min_lines, max_lines, min_cols, max_cols;

                        if( sscanf( argv[i] + 2, "%d,%d,%d,%d",
                                       &min_lines, &max_lines,
                                       &min_cols, &max_cols) == 4)
                        {
                            ttytype[0] = min_lines;
                            ttytype[1] = max_lines;
                            ttytype[2] = min_cols;
                            ttytype[3] = max_cols;
                        }
                    }
                    break;
                case 'd':     /* set window size before initscr */
                    {
                        int n_lines, n_cols;

                        if( sscanf( argv[i] + 2, "%d,%d", &n_lines,
                                    &n_cols) == 2)
                            resize_term( n_lines, n_cols);
                    }
                    break;
#ifdef HAVE_WIDE
                case 'u':
                    sscanf( argv[i] + 2, "%x", &unicode_offset);
                    break;
#endif
                default:
                    printf( "Option '%s' unrecognized\n", argv[i]);
                    break;
            }
    if( use_slk)
       slk_init( show_slk_index_line ? -fmt : fmt);
#ifdef XCURSES
    Xinitscr(argc, argv);
#else
    initscr();
#endif
    if( use_slk)
       slk_setup( show_slk_index_line ? -fmt : fmt);

    start_color();

# if defined(NCURSES_VERSION) || (defined(PDC_BUILD) && PDC_BUILD > 3000)
    use_default_colors();
# endif
    cbreak();
    noecho();
    clear();
    refresh();
#ifdef __PDCURSES__
    PDC_set_title( "NewTest: tests various PDCurses features");
#endif
    keypad( stdscr, TRUE);
    init_pair( 1, COLOR_WHITE, COLOR_BLACK);
    init_pair( 2, COLOR_BLACK, COLOR_YELLOW);

    mousemask( ALL_MOUSE_EVENTS, NULL);
    attrset( COLOR_PAIR( 1));
    while( !quit)
    {
        char buff[80];
        const int xmax = getmaxx( stdscr);
        const int ymax = getmaxy( stdscr);
        const int color_block_start = 54;
        int c, color_block_cols = (xmax - color_block_start) / 2;
        const int color_block_lines = 19;
        const char *cursor_state_text[N_CURSORS] = {
                  "Invisible (click to change) ",
                  "Underscore (click to change)",
                  "Block (click to change)     ",
                  "Outline (click to change)   ",
                  "Caret (click to change)     ",
                  "Half-block (click to change)",
                  "Central (click to change)   ",
                  "Cross (click to change)     ",
                  "Heavy box (click to change) " };

        if( color_block_cols < 0)
            color_block_cols = 0;
        if( redraw)
        {
            int line = 21;

            mvaddstr( 1, COL1, "'Normal' white-on-black");
            mvaddstr( 2, COL1, longname( ));
#if(CHTYPE_LONG >= 2)       /* "non-standard" 64-bit chtypes     */
            attron( A_DIM);
            mvaddstr( 15, 41, "Dimmed text");
            attroff( A_DIM);
#endif
#ifdef HAVE_WIDE
            mvaddwstr( 3, COL1, L"'Normal' text,  but wide");
#endif
            attron( A_BLINK);
            mvaddstr( 6, 40, "Blinking");
            attron( A_BOLD);
            mvaddstr( 8, 40, "BlinkBold");
            attron( A_ITALIC);
            mvaddstr( 0, COL2, "BlinkBoldItalic");
            attrset( COLOR_PAIR( 3));
            attron( A_UNDERLINE);
#ifdef HAVE_WIDE
            mvaddstr( 1, COL2, "Underlined");
            addwstr( L"WideUnder");
#endif
            attrset( COLOR_PAIR( 1));
            attron( A_UNDERLINE | A_ITALIC);
            mvaddstr( 2, COL2, "UnderlinedItalic");
            attrset( COLOR_PAIR( 2));
            attron( A_BLINK);
            mvaddstr( 4, COL1, "Black-on-yellow blinking");

            attrset( COLOR_PAIR( 1));
            move( 4, COL2);
            text_in_a_box( "Text in a box");

#ifdef CHTYPE_LONG
            attrset( COLOR_PAIR( 6));
            attron( A_STRIKEOUT);
            mvaddstr( 10, 40, "Strikeout");
            attrset( COLOR_PAIR( 1));
#endif

#ifdef HAVE_WIDE
            move( 11, 40);
            text_in_a_box( "Next Ucode pg");
            if( unicode_offset)
               {
               move( 12, 40);
               text_in_a_box( "Prev Ucode pg");
               }
            mvprintw( 13, 40, "U+%04x ", unicode_offset);

#endif

            for( i = 0; i < 128; i++)
            {                 /* Show extended characters: */
                char buff[6];
#ifdef HAVE_WIDE
                wchar_t wbuff[3];

                sprintf( buff, "%02x ",
                           (unsigned)( i + unicode_offset) & 0xff);
                mvaddstr( 5 + i % 16, (i / 16) * 5, buff);
                wbuff[0] = (wchar_t)( i + unicode_offset);
                wbuff[1] = (wchar_t)' ';
                wbuff[2] = (wchar_t)0;
                addwstr( wbuff);
#else

                sprintf( buff, "%02x %c", i + 128, (char)(i + 128));
                mvaddstr( 5 + i % 16, (i / 16) * 5, buff);
#endif
            }

#if(CHTYPE_LONG >= 2)       /* "non-standard" 64-bit chtypes     */
            for( i = 0; i < 3 && line < ymax; i++, line++)
            {                 /* Demonstrate full RGB color control: */
                int j;
                const char *output_text[3] = {
                    "Red on green to white on black   | (you can get full RGB colors when desired,",
                    "Blue on yellow to black on red | with palette coloring still being available)",
                    "White on red to green on blue,  underlined and italic" };
                const int len = (int)strlen( output_text[i]);

                move( line, 1);
                for( j = 0; j < len && j + 1 < xmax; j++)
                {
                    attr_t output_color;
                    const int oval = j * 31 / len;
                    const int reverse = 31 - oval;

                    if( !i)
                        output_color = A_RGB( 31, oval, oval, 0, reverse, 0);
                    else if( i == 1)
                        output_color = A_RGB( 0, 0, reverse, 31, reverse, 0);
                    else
                    {
                        output_color = A_RGB( reverse, 31, reverse,
                               reverse, 0, oval);
                        output_color |= A_UNDERLINE | A_ITALIC;
                    }
                    attrset( output_color);
                    addch( output_text[i][j]);
                }
            }
#endif         /* #if(CHTYPE_LONG >= 2) */
            redraw = 0;
            attrset( COLOR_PAIR( 1));
            if( extra_character_to_show && ymax > 23)
                mvaddch( 23, 63, (chtype)extra_character_to_show);

#ifdef HAVE_WIDE
            for( i = 0; i < 6; i++)
            {
                static const wchar_t spanish[] = L"Espa\xf1ol";
                const int line0 = line + i / 3;
                const int col = 5 + 25 * (i % 3);

                static const wchar_t russian[] = {0x0420, 0x0443, 0x0441, 0x0441,
                   0x043a, 0x0438, 0x0439, L' ', 0x044f, 0x0437, 0x044b, 0x043a, 0};

                static const wchar_t greek[] = {0x0395, 0x03bb, 0x03bb, 0x03b7,
                   0x03bd, 0x03b9, 0x03ba, 0x03ac, 0};

                static const wchar_t georgian[] = {0x10e5, 0x10d0, 0x10e0, 0x10d7,
                   0x10e3, 0x10da, 0x10d8, L' ', 0x10d4, 0x10dc, 0x10d0, 0};

                static const wchar_t fullwidth[] = { 0xff26, 0xff55, 0xff4c, 0xff4c,
                   0xff57, 0xff49, 0xff44, 0xff54, 0xff48, 0 };  /* "Fullwidth" */

                static const wchar_t combining_marks[] = { L'C', L'o', 0x35c, L'm',
                   L'b', 0x30a, L'i', L'n', L'i', 0x304, L'n', 0x30b, 0x329,
                   L'g', 0x310,
                   L' ', L'C', 0x338, L'h', 0x306,  L'a', 0x361, L'r', L's',
                   0x30e, 0x348, 0 };

                static const wchar_t *texts[6] = { spanish, russian, greek,
                                georgian, fullwidth, combining_marks};

                if( line0 < ymax && col < xmax)
                   mvaddnwstr( line0, 5 + 25 * (i % 3), texts[i], xmax - col);
            }
            line += 2;
#endif
        mvaddstr( line, 1, curses_version( ));

#ifdef MAYBE_TRY_THIS_SOMEWHERE_ELSE
        mvaddstr(  1, COL3, "Click on cursor descriptions to");
        mvaddstr(  2, COL3, "cycle through possible cursors");
        mvaddstr(  3, COL3, "Click on colors at left to change");
        mvaddstr(  4, COL3, "colors used for under/over/outlining");
        mvaddstr(  5, COL3, "Click 'Blink' at bottom to toggle");
        mvaddstr(  6, COL3, "'real' blinking vs. 'highlit' blink");
#endif
        }

        mvaddnstr( 19, color_block_start, cursor_state_text[cursor_state_1],
                                 xmax - color_block_start);
        mvaddnstr( 20, color_block_start, cursor_state_text[cursor_state_2],
                                 xmax - color_block_start);
        curs_set( (cursor_state_1 << 8) | cursor_state_2);
        for( i = 0; i < color_block_cols * color_block_lines; i++)
        {
            const int n_color_blocks = (COLOR_PAIRS < 256 ? COLOR_PAIRS : 256);

            attrset( COLOR_PAIR( i >= n_color_blocks ? 2 : i));
            if( i > 2 && i < n_color_blocks)
               init_pair((short)i, (short)i, COLOR_BLACK);
            if( !(i % color_block_cols))
               move( i / color_block_cols, color_block_start);
            attron( A_REVERSE);
            addstr( "  ");
        }
        move( cursor_y, cursor_x);
        refresh();
        c = getch( );
        attrset( COLOR_PAIR( 1));
        if( c == KEY_RESIZE)
        {
            redraw = 1;
            resize_term( 0, 0);
        }
        else if( c == KEY_F(1) || c == 27)
            quit = 1;
#ifdef PDCURSES
        else if( c == KEY_F(2))
        {
            blink_state ^= 1;
            PDC_set_blink( blink_state);
        }
#endif
        else if( c == KEY_F(3))   /* toggle SLKs */
        {
            use_slk ^= 1;
            if( use_slk)
                slk_restore( );
            else
                slk_clear( );
        }
        else if( c >= KEY_F(4) && c < KEY_F(12))
        {
            sscanf( labels[c - KEY_F(1)], "%x", (unsigned *)&fmt);
            if( use_slk)
                slk_setup( show_slk_index_line ? -fmt : fmt);
        }
        if( c != KEY_MOUSE)
        {
            sprintf( buff, "Key %s", keyname( c));
            if( !memcmp( buff + 4, "UNKNOWN", 7))
                sprintf( buff + 11, " (%x)", c);
            strcat( buff, " hit                 ");
            buff[COL2 - COL1] = '\0';
            mvaddstr( 0, COL1, buff);
        }
        else
        {
            MEVENT mouse_event;
#ifdef __PDCURSES__
            nc_getmouse( &mouse_event);
#else
            getmouse( &mouse_event);
#endif
            sprintf( buff, "Mouse at %d x %d: %x     ", mouse_event.x,
                              mouse_event.y, (unsigned)mouse_event.bstate);
            cursor_x = mouse_event.x;
            cursor_y = mouse_event.y;
            mvaddstr( 0, COL1, buff);
            if( mouse_event.x >= color_block_start
                            && mouse_event.y < color_block_lines)
            {
                int new_color = (mouse_event.x - color_block_start) / 2
                              + mouse_event.y * color_block_cols;

                if( new_color >= 256)
                    new_color = -1;
#ifdef PDCURSES
                PDC_set_line_color( (short)new_color);
#endif
            }
#ifdef PDCURSES
            else if( mouse_event.x >= color_block_start)
            {
                int shift = ((mouse_event.bstate & BUTTON_MODIFIER_SHIFT) ?
                           N_CURSORS - 1 : 1);

                if( mouse_event.y == 19)  /* blink/non-blink toggle */
                    cursor_state_1 = (cursor_state_1 + shift) % N_CURSORS;
                else if( mouse_event.y == 20)  /* cycle cursor state */
                    cursor_state_2 = (cursor_state_2 + shift) % N_CURSORS;
            }
#endif
#ifdef HAVE_WIDE
            else if( mouse_event.x >= 40 && mouse_event.x < 40 + 10)
               {
               if( mouse_event.y == 11)
                  {
                  redraw = 1;
                  unicode_offset += 0x80;
                  }
               else if( mouse_event.y == 12 && unicode_offset)
                  {
                  redraw = 1;
                  unicode_offset -= 0x80;
                  }
               }
#endif
        }
    }

    endwin();

    return 0;
}

