#define USE_UNICODE_ACS_CHARS 1

#include <wchar.h>
#include <assert.h>
#include <stdint.h>

#define USE_UNICODE_ACS_CHARS 1

#include "curspriv.h"
#include "pdcvt.h"
#include "../common/acs_defs.h"
#include "../common/pdccolor.h"

void PDC_gotoyx(int y, int x)
{
   printf( "\033[%d;%dH", y + 1, x + 1);
}

#define STANDOUT_ON   "\033[3m"
#define STANDOUT_OFF  "\033[23m"
#define UNDERLINE_ON  "\033[4m"
#define UNDERLINE_OFF "\033[24m"
#define BLINK_ON      "\033[5m"
#define BLINK_OFF     "\033[25m"
#define BOLD_ON       "\033[1m"
#define BOLD_OFF      "\033[22m"
#define DIM_ON        "\033[2m"
#define DIM_OFF       "\033[22m"
#define STDOUT          0

const chtype MAX_UNICODE = 0x110000;

/* see 'addch.c' for an explanation of how combining chars are handled. */

#if defined( CHTYPE_LONG) && CHTYPE_LONG >= 2 && defined( PDC_WIDE)
   #define USING_COMBINING_CHARACTER_SCHEME
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
    char txt[20];

    PDC_get_rgb_values( ch, &fg, &bg);
    if( bg != prev_bg)
        {
        if( bg == (PACKED_RGB)-1)   /* default background */
            printf( "\033[49m");
        else if( COLORS == 16)
            printf( "\033[4%dm", get_sixteen_color_idx( bg));
        else
            printf( "\033[48;%s", color_string( txt, bg));
        prev_bg = bg;
        }
    if( fg != prev_fg)
        {
        if( fg == (PACKED_RGB)-1)   /* default foreground */
            printf( "\033[39m");
        else if( COLORS == 16)
            printf( "\033[3%dm", get_sixteen_color_idx( fg));
        else
            printf( "\033[38;%s", color_string( txt, fg));
        prev_fg = fg;
        }
}

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    static chtype prev_ch = 0;

    if( !srcp)
    {
        prev_ch = 0;
        printf( BLINK_OFF BOLD_OFF UNDERLINE_OFF STANDOUT_OFF);
        return;
    }
    assert( x >= 0);
    assert( len <= SP->cols - x);
    assert( lineno >= 0);
    assert( lineno < SP->lines);
    assert( len > 0);
    PDC_gotoyx( lineno, x);
    prev_ch = *srcp ^ A_COLOR;
    while( len--)
       {
       int ch = (int)( *srcp & A_CHARTEXT);
       chtype changes = *srcp ^ prev_ch;

       if( (*srcp & A_ALTCHARSET) && ch < 0x80)
          ch = (int)acs_map[ch & 0x7f];
       if( ch < (int)' ' || (ch >= 0x80 && ch <= 0x9f))
          ch = ' ';
       if( SP->termattrs & changes & A_BOLD)
          printf( (*srcp & A_BOLD) ? BOLD_ON : BOLD_OFF);
       if( changes & A_UNDERLINE)
          printf( (*srcp & A_UNDERLINE) ? UNDERLINE_ON : UNDERLINE_OFF);
       if( changes & A_STANDOUT)
          printf( (*srcp & A_STANDOUT) ? STANDOUT_ON : STANDOUT_OFF);
       if( SP->termattrs & changes & A_BLINK)
          printf( (*srcp & A_BLINK) ? BLINK_ON : BLINK_OFF);
       if( changes & (A_COLOR | A_STANDOUT | A_BLINK))
          reset_color( *srcp);
#ifdef PDC_WIDE
       if( ch < 128)
          printf( "%c", (char)ch);
       else if( ch < (int)MAX_UNICODE)
          printf( "%lc", (wchar_t)ch);
#else
       printf( "%c", (char)ch);
#endif
#ifdef USING_COMBINING_CHARACTER_SCHEME
       else if( ch > (int)MAX_UNICODE)      /* chars & fullwidth supported */
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
#endif
       prev_ch = *srcp++;
       }
}

void PDC_doupdate(void)
{
}
