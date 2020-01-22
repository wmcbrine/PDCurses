#ifdef NO_STDINT_H
   #define uint64_t unsigned long long
   #define uint32_t unsigned long
   #define uint16_t unsigned short
#else
   #include <stdint.h>
#endif
   #include <stdlib.h>
   #include <assert.h>

#define PACKED_RGB uint32_t

#ifndef PACK_RGB
   #define PACK_RGB( red, green, blue) ((red) | ((green)<<8) | ((blue) << 16))
#endif

#include <curspriv.h>
#include "pdccolor.h"

int PDC_blink_state = 0;

static PACKED_RGB *PDC_rgbs;        /* the 'standard' 256-color palette */
static PACKED_RGB ***PDC_rgb_cube;   /* the 'extended' 16M-color palette,
                                    stored as a 256x256x256 array */

int PDC_init_palette( void)
{
    int i, r, g, b;
    const int palette_size = (COLORS > 256 ? 256 : COLORS);

    PDC_rgbs = (PACKED_RGB *)calloc( palette_size, sizeof( PACKED_RGB));
    if( !PDC_rgbs)
        return( -1);
    for( i = 0; i < 16 && i < COLORS; i++)
    {
        const int intensity = ((i & 8) ? 0xff : 0xc0);

        PDC_rgbs[i] = PACK_RGB( ((i & COLOR_RED) ? intensity : 0),
                           ((i & COLOR_GREEN) ? intensity : 0),
                           ((i & COLOR_BLUE) ? intensity : 0));
    }
           /* 256-color xterm extended palette:  216 colors in a
            6x6x6 color cube,  plus 24 (not 50) shades of gray */
    for( r = 0; r < 6; r++)
        for( g = 0; g < 6; g++)
            for( b = 0; b < 6; b++)
                if( i < COLORS)
                    PDC_rgbs[i++] = PACK_RGB( r ? r * 40 + 55 : 0,
                                   g ? g * 40 + 55 : 0,
                                   b ? b * 40 + 55 : 0);
    for( i = 0; i < 24; i++)
        if( i + 232 < COLORS)
            PDC_rgbs[i + 232] = PACK_RGB( i * 10 + 8, i * 10 + 8, i * 10 + 8);
    return( 0);
}

void PDC_free_palette( void)
{
   if( PDC_rgbs)
      free( PDC_rgbs);
   PDC_rgbs = NULL;
   if( PDC_rgb_cube)
      {
      int i, j;

      for( i = 0; i < 256; i++)
         if( PDC_rgb_cube[i])
            {
            for( j = 0; j < 256; j++)
               if( PDC_rgb_cube[i][j])
                  free( PDC_rgb_cube[i][j]);
            free( PDC_rgb_cube[i]);
            }
      free( PDC_rgb_cube);
      PDC_rgb_cube = NULL;
      }
}

PACKED_RGB PDC_get_palette_entry( const int idx)
{
   PACKED_RGB rval;

   if( !PDC_rgbs && PDC_init_palette( ))
      rval = ( idx ? 0xffffff : 0);
   else if( idx < 256)
      rval = PDC_rgbs[idx];
   else
      {
      rval = idx - 256;    /* by default,  rval is a direct 16M color, */
      if( PDC_rgb_cube)    /* but we check to see if it's been reset */
         {                 /* with PDC_set_palette_entry */
         PACKED_RGB **square = PDC_rgb_cube[(rval >> 16) & 0xff];

         if( square)
            {
            PACKED_RGB *array = square[(rval >> 8) & 0xff];

            if( array)
               rval = array[rval & 0xff];
            }
         }
      }
   return( rval);
}

/* Return value is -1 if no palette could be allocated,  0 if the color
didn't change (new RGB matched the old one),  and 1 if the color changed. */

int PDC_set_palette_entry( const int idx, const PACKED_RGB rgb)
{
   int rval;

   if( !PDC_rgbs && PDC_init_palette( ))
      rval = -1;
   else if( idx < 256)
      {
      rval = (PDC_rgbs[idx] == rgb ? 1 : 0);
      PDC_rgbs[idx] = rgb;
      }
   else
      {
      const int red = idx & 0xff;
      const int grn = ((idx - 256) >> 8) & 0xff;
      const int blu = ((idx - 256) >> 16) & 0xff;

      if( !PDC_rgb_cube)
         PDC_rgb_cube = (PACKED_RGB ***)calloc( 256, sizeof( PACKED_RGB **));
      assert( PDC_rgb_cube);

      if( !PDC_rgb_cube[blu])
         PDC_rgb_cube[blu] = (PACKED_RGB **)calloc( 256, sizeof( PACKED_RGB *));
      assert( PDC_rgb_cube[blu]);

      if( !PDC_rgb_cube[blu][grn])
         {
         int i;

         PDC_rgb_cube[blu][grn] = (PACKED_RGB *)calloc( 256, sizeof( PACKED_RGB));
         assert( PDC_rgb_cube[blu][grn]);
         for( i = 0; i < 256; i++)
            PDC_rgb_cube[blu][grn][i] = PACK_RGB( i, grn, blu);
         }
      assert( PDC_rgb_cube[blu][grn]);
      rval = (PDC_rgb_cube[blu][grn][red] == rgb ? 1 : 0);
      PDC_rgb_cube[blu][grn][red] = rgb;
      }
   return( rval);
}

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


void PDC_get_rgb_values( const chtype srcp,
            PACKED_RGB *foreground_rgb, PACKED_RGB *background_rgb)
{
    const int color = (int)(( srcp & A_COLOR) >> PDC_COLOR_SHIFT);
    bool reverse_colors = ((srcp & A_REVERSE) ? TRUE : FALSE);
    bool intensify_backgnd = FALSE;
    bool default_foreground = FALSE, default_background = FALSE;

    {
        short foreground_index, background_index;

        pair_content( (short)color, &foreground_index, &background_index);
        if( foreground_index < 0 && SP->orig_attr)
            default_foreground = TRUE;
        else
            *foreground_rgb = PDC_get_palette_entry( foreground_index);
        if( background_index < 0 && SP->orig_attr)
            default_background = TRUE;
        else
            *background_rgb = PDC_get_palette_entry( background_index);
    }

    if( srcp & A_BLINK)
    {
        if( !(SP->termattrs & A_BLINK))   /* convert 'blinking' to 'bold' */
            intensify_backgnd = TRUE;
        else if( PDC_blink_state)
            reverse_colors ^= 1;
    }
    if( reverse_colors)
    {
        const PACKED_RGB temp = *foreground_rgb;

        *foreground_rgb = *background_rgb;
        *background_rgb = temp;
    }

    if( srcp & A_BOLD & ~SP->termattrs)
        *foreground_rgb = intensified_color( *foreground_rgb);
    if( intensify_backgnd)
        *background_rgb = intensified_color( *background_rgb);
    if( srcp & A_DIM)
    {
        *foreground_rgb = dimmed_color( *foreground_rgb);
        *background_rgb = dimmed_color( *background_rgb);
    }
    if( default_foreground)
        *foreground_rgb = (PACKED_RGB)-1;
    if( default_background)
        *background_rgb = (PACKED_RGB)-1;
}
