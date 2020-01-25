#define USE_UNICODE_ACS_CHARS 1

#include <wchar.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define USE_UNICODE_ACS_CHARS 1

#include "curspriv.h"
#include "pdcvt.h"
#include "../common/acs_defs.h"
#include "../common/pdccolor.h"

void PDC_gotoyx(int y, int x)
{
   printf( "\033[%d;%dH", y + 1, x + 1);
}

#define ITALIC_ON     "\033[3m"
#define ITALIC_OFF    "\033[23m"
#define UNDERLINE_ON  "\033[4m"
#define UNDERLINE_OFF "\033[24m"
#define BLINK_ON      "\033[5m"
#define BLINK_OFF     "\033[25m"
#define BOLD_ON       "\033[1m"
#define BOLD_OFF      "\033[22m"
#define DIM_ON        "\033[2m"
#define DIM_OFF       "\033[22m"
#define REVERSE_ON    "\033[7m"
#define REVERSE_OFF   "\033[27m"
#define STDOUT          0

const chtype MAX_UNICODE = 0x110000;

/* see 'addch.c' for an explanation of how combining chars are handled. */

#ifdef USING_COMBINING_CHARACTER_SCHEME
   int PDC_expand_combined_characters( const cchar_t c, cchar_t *added);  /* addch.c */
#endif

static char *color_string( char *otext, const PACKED_RGB rgb)
{
   extern bool PDC_has_rgb_color;      /* pdcscrn.c */

   if( PDC_has_rgb_color)
      sprintf( otext, "2;%d;%d;%dm", Get_RValue( rgb),
                             Get_GValue( rgb), Get_BValue( rgb));
   else
      {
      const int red = Get_RValue( rgb);
      const int green = Get_GValue( rgb);
      const int blue = Get_BValue( rgb);
      int idx;

      if( red == green && red == blue)   /* gray scale: indices from */
         {
         if( red < 27)     /* this would underflow; remap to black */
            idx = COLOR_BLACK;
         else if( red >= 243)    /* this would overflow */
            idx = COLOR_WHITE;
         else
            idx = (red - 3) / 10 + 232;     /* 232 to 255 */
         }
      else
         idx = ((blue - 35) / 40) + ((green - 35) / 40) * 6
                  + ((red - 35) / 40) * 36 + 16;

      sprintf( otext, "5;%dm", idx);
      }
   return( otext);
}

static int get_sixteen_color_idx( const PACKED_RGB rgb)
{
    int rval = 0;

    if( rgb & 0x80)    /* red value >= 128 */
        rval = 1;
    if( rgb & 0x8000)      /* green value >= 128 */
        rval |= 2;
    if( rgb & 0x800000)        /* blue value >= 128 */
        rval |= 4;
    return( rval);
}

static void reset_color( const chtype ch)
{
    static PACKED_RGB prev_bg = (PACKED_RGB)-1;
    static PACKED_RGB prev_fg = (PACKED_RGB)-1;
    PACKED_RGB bg, fg;
    char txt[45];

    PDC_get_rgb_values( ch, &fg, &bg);
    *txt = '\0';
    if( bg != prev_bg)
        {
        if( bg == (PACKED_RGB)-1)   /* default background */
            strcpy( txt, "\033[49m");
        else if( !bg)
            strcpy( txt, "\033[40m");
        else if( COLORS == 16)
            sprintf( txt, "\033[4%dm", get_sixteen_color_idx( bg));
        else
            {
            strcpy( txt, "\033[48;");
            color_string( txt + 5, bg);
            }
        prev_bg = bg;
        }
    if( fg != prev_fg)
        {
        if( fg == (PACKED_RGB)-1)   /* default foreground */
            strcat( txt, "\033[39m");
        else if( COLORS == 16)
            sprintf( txt + strlen( txt), "\033[3%dm", get_sixteen_color_idx( fg));
        else
            {
            strcat( txt, "\033[38;");
            color_string( txt + strlen( txt), fg);
            }
        prev_fg = fg;
        }
    if( *txt)
        fwrite( txt, strlen( txt), 1, stdout);
}

      /* We can output runs up to RUN_LEN wide chars.  Each may become
         four bytes in UTF8,  so we set OBUFF_SIZE = 4 * RUN_LEN.   */
#define RUN_LEN      20
#define OBUFF_SIZE   80

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    static chtype prev_ch = 0;
    static bool force_reset_all_attribs = TRUE;

    if( !srcp)
    {
        prev_ch = 0;
        force_reset_all_attribs = TRUE;
        printf( BLINK_OFF BOLD_OFF UNDERLINE_OFF ITALIC_OFF REVERSE_OFF);
        return;
    }
    while( len > RUN_LEN)     /* break input into RUN_LEN character blocks */
    {
        PDC_transform_line( lineno, x, RUN_LEN, srcp);
        len -= RUN_LEN;
        x += RUN_LEN;
        srcp += RUN_LEN;
    }
    assert( x >= 0);
    assert( len <= SP->cols - x);
    assert( lineno >= 0);
    assert( lineno < SP->lines);
    assert( len > 0);
    PDC_gotoyx( lineno, x);
    if( force_reset_all_attribs || (!x && !lineno))
    {
        force_reset_all_attribs = FALSE;
        prev_ch = ~*srcp;
    }
    while( len)
       {
       int ch = (int)( *srcp & A_CHARTEXT), count = 1;
       chtype changes = *srcp ^ prev_ch;

       if( (*srcp & A_ALTCHARSET) && ch < 0x80)
          ch = (int)acs_map[ch & 0x7f];
       if( ch < (int)' ' || (ch >= 0x80 && ch <= 0x9f))
          ch = ' ';
       if( SP->termattrs & changes & A_BOLD)
          printf( (*srcp & A_BOLD) ? BOLD_ON : BOLD_OFF);
       if( changes & A_UNDERLINE)
          printf( (*srcp & A_UNDERLINE) ? UNDERLINE_ON : UNDERLINE_OFF);
       if( changes & A_ITALIC)
          printf( (*srcp & A_ITALIC) ? ITALIC_ON : ITALIC_OFF);
       if( changes & A_REVERSE)
          printf( (*srcp & A_REVERSE) ? REVERSE_ON : REVERSE_OFF);
       if( SP->termattrs & changes & A_BLINK)
          printf( (*srcp & A_BLINK) ? BLINK_ON : BLINK_OFF);
       if( changes & (A_COLOR | A_STANDOUT | A_BLINK))
          reset_color( *srcp & ~A_REVERSE);
#ifdef USING_COMBINING_CHARACTER_SCHEME
       if( ch > (int)MAX_UNICODE)      /* chars & fullwidth supported */
       {
           cchar_t root, newchar;

           root = ch;
           while( (root = PDC_expand_combined_characters( root,
                              &newchar)) > MAX_UNICODE)
               ;
           printf( "%lc", (wchar_t)root);
           root = ch;
           while( (root = PDC_expand_combined_characters( root,
                              &newchar)) > MAX_UNICODE)
               printf( "%lc", (wchar_t)newchar);
           printf( "%lc", (wchar_t)newchar);
       }
       else if( ch < (int)MAX_UNICODE)
#endif
       {
           size_t bytes_out, bytes_written;
           char obuff[OBUFF_SIZE];

           bytes_out = wctomb( obuff, (wchar_t)ch);
           while( count < len && !((srcp[0] ^ srcp[count]) & ~A_CHARTEXT)
                        && (ch = (srcp[count] & A_CHARTEXT)) < MAX_UNICODE)
           {
               if( (srcp[count] & A_ALTCHARSET) && ch < 0x80)
                  ch = (int)acs_map[ch & 0x7f];
               if( ch < (int)' ' || (ch >= 0x80 && ch <= 0x9f))
                  ch = ' ';
               bytes_out += wctomb( obuff + bytes_out, (wchar_t)ch);
               count++;
           }
           bytes_written = fwrite( obuff, 1, bytes_out, stdout);
           assert( bytes_written == (size_t)bytes_out);
       }
       prev_ch = *srcp;
       srcp += count;
       len -= count;
       }
}

void PDC_doupdate(void)
{
}
