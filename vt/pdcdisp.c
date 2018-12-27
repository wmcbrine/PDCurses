#define USE_UNICODE_ACS_CHARS 1

#include <wchar.h>
#include <assert.h>

#define USE_UNICODE_ACS_CHARS 1

#include "curses.h"
#include "pdcvt.h"
#include "acs_defs.h"

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


extern int PDC_really_blinking;
extern chtype PDC_capabilities;

    /* This function 'intensifies' a color by shifting it toward white. */
    /* It used to average the input color with white.  Then it did a    */
    /* weighted average:  2/3 of the input color,  1/3 white,   for a   */
    /* lower "intensification" level.                                   */
    /*    Then Mark Hessling suggested that the output level should     */
    /* remap zero to 85 (= 255 / 3, so one-third intensity),  and input */
    /* of 192 or greater should be remapped to 255 (full intensity).    */
    /* Assuming we want a linear response between zero and 192,  that   */
    /* leads to output = 85 + input * (255-85)/192.                     */
    /*    This should lead to proper handling of bold text in legacy    */
    /* apps,  where "bold" means "high intensity".                      */

static PACKED_RGB intensified_color( PACKED_RGB ival)
{
    int rgb, i;
    PACKED_RGB oval = 0;

    for( i = 0; i < 3; i++, ival >>= 8)
    {
        rgb = (int)( ival & 0xff);
        if( rgb >= 192)
            rgb = 255;
        else
            rgb = 85 + rgb * (255 - 85) / 192;
        oval |= ((PACKED_RGB)rgb << (i * 8));
    }
    return( oval);
}

   /* For use in adjusting colors for A_DIMmed characters.  Just */
   /* knocks down the intensity of R, G, and B by 1/3.           */

static PACKED_RGB dimmed_color( PACKED_RGB ival)
{
    unsigned i;
    PACKED_RGB oval = 0;

    for( i = 0; i < 3; i++, ival >>= 8)
    {
        unsigned rgb = (unsigned)( ival & 0xff);

        rgb -= (rgb / 3);
        oval |= ((PACKED_RGB)rgb << (i * 8));
    }
    return( oval);
}


   /* PDC_get_rgb_values(), extract_packed_rgb(), intensified_component(), */
   /* intensified_color(),  and dimmed_color() each exist in x11/x11.c,    */
   /* win32a/pdcdisp.c,  and sdl2/pdcdisp.c in forms slightly modified for */
   /* each platform.  But they all look pretty much alike.  */

            /* PDCurses stores RGBs in fifteen bits,  five bits each */
            /* for red, green, blue.  A PACKED_RGB uses eight bits per */
            /* channel.  Hence the following.                        */
#if defined( CHTYPE_LONG) && CHTYPE_LONG >= 2
static PACKED_RGB extract_packed_rgb( const chtype color)
{
    const int red   = (int)( (color << 3) & 0xf8);
    const int green = (int)( (color >> 2) & 0xf8);
    const int blue  = (int)( (color >> 7) & 0xf8);

    return( PACK_RGB( red, green, blue));
}
#endif

void PDC_get_rgb_values( const chtype srcp,
            PACKED_RGB *foreground_rgb, PACKED_RGB *background_rgb)
{
    const int color = (int)(( srcp & A_COLOR) >> PDC_COLOR_SHIFT);
    bool reverse_colors = ((srcp & A_REVERSE) ? TRUE : FALSE);
    bool intensify_backgnd = FALSE;

#if defined( CHTYPE_LONG) && CHTYPE_LONG >= 2
    if( srcp & A_RGB_COLOR)
    {
        /* Extract RGB from 30 bits of the color field */
        *background_rgb = extract_packed_rgb( srcp >> PDC_COLOR_SHIFT);
        *foreground_rgb = extract_packed_rgb( srcp >> (PDC_COLOR_SHIFT + 15));
    }
    else
#endif
    {
        extern PACKED_RGB *pdc_rgbs;
        short foreground_index, background_index;

        PDC_pair_content( (short)color, &foreground_index, &background_index);
        *foreground_rgb = pdc_rgbs[foreground_index];
        *background_rgb = pdc_rgbs[background_index];
    }

    if( srcp & A_BLINK)
    {
        extern int PDC_really_blinking;          /* see 'pdcsetsc.c' */

        if( !PDC_really_blinking)   /* convert 'blinking' to 'bold' */
            intensify_backgnd = TRUE;
    }
    if( reverse_colors)
    {
        const PACKED_RGB temp = *foreground_rgb;

        *foreground_rgb = *background_rgb;
        *background_rgb = temp;
    }

    if( srcp & A_BOLD)
        *foreground_rgb = intensified_color( *foreground_rgb);
    if( intensify_backgnd)
        *background_rgb = intensified_color( *background_rgb);
    if( srcp & A_DIM)
        *foreground_rgb = dimmed_color( *foreground_rgb);
    if( srcp & A_DIM)
        *background_rgb = dimmed_color( *background_rgb);
}

static char *color_string( char *otext, const PACKED_RGB rgb)
{
   if( PDC_capabilities & A_RGB_COLOR)
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

    if( rgb & 0x800000)    /* red value >= 128 */
        rval = 1;
    if( rgb & 0x8000)      /* green value >= 128 */
        rval |= 2;
    if( rgb & 0x80)        /* blue value >= 128 */
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
        if( COLORS == 16)
            printf( "\033[4%dm", get_sixteen_color_idx( bg));
        else
            printf( "\033[48;%s", color_string( txt, bg));
        prev_bg = bg;
        }
    if( fg != prev_fg)
        {
        if( COLORS == 16)
            printf( "\033[3%dm", get_sixteen_color_idx( fg));
        else
            printf( "\033[38;%s", color_string( txt, fg));
        prev_fg = fg;
        }
}

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    static chtype prev_ch = 0;

    assert( x >= 0);
    assert( len <= SP->cols - x);
    assert( lineno >= 0);
    assert( lineno < SP->lines);
    assert( len > 0);
    PDC_gotoyx( lineno, x);
    while( len--)
       {
       int ch = (int)( *srcp & A_CHARTEXT);
       chtype changes = *srcp ^ prev_ch;

       if( (*srcp & A_ALTCHARSET) && ch < 0x80)
          ch = acs_map[ch & 0x7f];
       if( ch < (int)' ' || (ch >= 0x80 && ch <= 0x9f))
          ch = ' ';
       if( changes & A_BOLD)
          printf( (*srcp & A_BOLD) ? BOLD_ON : BOLD_OFF);
       if( changes & A_UNDERLINE)
          printf( (*srcp & A_UNDERLINE) ? UNDERLINE_ON : UNDERLINE_OFF);
       if( changes & A_STANDOUT)
          printf( (*srcp & A_STANDOUT) ? STANDOUT_ON : STANDOUT_OFF);
       if( PDC_really_blinking && ( changes & A_BLINK))
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
