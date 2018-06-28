#define USE_UNICODE_ACS_CHARS 1

#include <wchar.h>

#define USE_UNICODE_ACS_CHARS 1

#include "pdcvt.h"
#include "curses.h"
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
#define BOLD_ON       "\033[1"
#define BOLD_OFF      "\033[22m"
#define STDOUT          0

const chtype MAX_UNICODE = 0x110000;

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

static COLORREF intensified_color( COLORREF ival)
{
    int rgb, i;
    COLORREF oval = 0;

    for( i = 0; i < 3; i++, ival >>= 8)
    {
        rgb = (int)( ival & 0xff);
        if( rgb >= 192)
            rgb = 255;
        else
            rgb = 85 + rgb * (255 - 85) / 192;
        oval |= ((COLORREF)rgb << (i * 8));
    }
    return( oval);
}

   /* For use in adjusting colors for A_DIMmed characters.  Just */
   /* knocks down the intensity of R, G, and B by 1/3.           */

static COLORREF dimmed_color( COLORREF ival)
{
    unsigned i;
    COLORREF oval = 0;

    for( i = 0; i < 3; i++, ival >>= 8)
    {
        unsigned rgb = (unsigned)( ival & 0xff);

        rgb -= (rgb / 3);
        oval |= ((COLORREF)rgb << (i * 8));
    }
    return( oval);
}


   /* PDC_get_rgb_values(), extract_packed_rgb(), intensified_component(), */
   /* intensified_color(),  and dimmed_color() each exist in x11/x11.c,    */
   /* win32a/pdcdisp.c,  and sdl2/pdcdisp.c in forms slightly modified for */
   /* each platform.  But they all look pretty much alike.  */

            /* PDCurses stores RGBs in fifteen bits,  five bits each */
            /* for red, green, blue.  A COLORREF uses eight bits per */
            /* channel.  Hence the following.                        */
static COLORREF extract_packed_rgb( const chtype color)
{
    const int red   = (int)( (color << 3) & 0xf8);
    const int green = (int)( (color >> 2) & 0xf8);
    const int blue  = (int)( (color >> 7) & 0xf8);

    return( RGB( red, green, blue));
}

void PDC_get_rgb_values( const chtype srcp,
            COLORREF *foreground_rgb, COLORREF *background_rgb)
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
        extern COLORREF *pdc_rgbs;
        short foreground_index, background_index;

        PDC_pair_content( (short)color, &foreground_index, &background_index);
        *foreground_rgb = pdc_rgbs[foreground_index];
        *background_rgb = pdc_rgbs[background_index];
    }

    if( srcp & A_BLINK)
    {
        extern int PDC_really_blinking;          /* see 'pdcsetsc.c' */
//      extern int PDC_blink_state;

        if( !PDC_really_blinking)   /* convert 'blinking' to 'bold' */
            intensify_backgnd = TRUE;
//      else if( PDC_blink_state)
//          reverse_colors = !reverse_colors;
    }
    if( reverse_colors)
    {
        const COLORREF temp = *foreground_rgb;

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

static char *color_string( char *otext, const COLORREF rgb)
{
   if( PDC_capabilities & A_RGB_COLOR)
      sprintf( otext, "2;%d;%d;%dm", GetRValue( rgb),
                             GetGValue( rgb), GetBValue( rgb));
   else
      {
      const int red = GetRValue( rgb);
      const int green = GetGValue( rgb);
      const int blue = GetBValue( rgb);
      const int idx = ((blue - 35) / 40) + ((green - 35) / 40) * 6
                  + ((red - 35) / 40) * 36 + 16;

      sprintf( otext, "5;%dm", idx);
      }
   return( otext);
}

static void reset_color( const chtype ch)
{
    static COLORREF prev_bg = (COLORREF)-1;
    static COLORREF prev_fg = (COLORREF)-1;
    COLORREF bg, fg;
    char txt[20];

    PDC_get_rgb_values( ch, &bg, &fg);
    if( bg != prev_bg)
        {
        printf( "\033[38;%s", color_string( txt, bg));
        prev_bg = bg;
        }
    if( fg != prev_fg)
        {
        printf( "\033[48;%s", color_string( txt, fg));
        prev_fg = fg;
        }
}

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    static chtype prev_ch = 0;

                     /* Seems to me as if the input text to this function */
    if( x < 0)       /* should _never_ be off-screen.  But it sometimes is. */
    {                /* Clipping is therefore necessary. */
        len += x;
        srcp -= x;
        x = 0;
    }
    if( len > SP->cols - x)
       len = SP->cols - x;
    if( lineno >= SP->lines || len <= 0 || lineno < 0)
       return;
    PDC_gotoyx( lineno, x);
    while( len--)
       {
       int ch = (int)( *srcp & A_CHARTEXT);
       chtype changes = *srcp ^ prev_ch;

       if( (*srcp & A_ALTCHARSET) && ch < 0x80)
          ch = acs_map[ch & 0x7f];
       if( ch < (int)' ')
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
       if( ch < MAX_UNICODE)
          printf( "%lc", (wchar_t)ch);
       prev_ch = *srcp++;
       }
}
