#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "curses.h"
#include <math.h>
#include <assert.h>
#if defined( __linux)
    #define CONVERT_IMAGES
    #include <unistd.h>
#endif

/* This is inspired by,  but not really based on,  Thomas E. Dickey's
'picsmap' program for ncurses.  Its real purpose is to demonstrate the
ability to use large palettes (basically,  full RGB) and large numbers
of color pairs (up to 2^20 = 1048576 pairs).  That ability is currently
available only in the VT and WinGUI flavors of PDCurses,  but will be
expanded to X11,  SDLx,  and probably WinCon.

   Run with an image file as a command-line argument.  The image will
be displayed using PDCurses' extended color palette and color pairs.
You can pan around the image with the mouse and keyboard, and zoom
in/out using the mouse wheel,  and hit 'r' to rotate the image 90 degrees.

   If you don't specify an image file,  a fake image showing color
gradients is generated.  On Linux,  it'll use 'convert' (ImageMagick)
to turn JPEGs or other image files into PNMs or PGMs.  On DOS/Windows,
you have to give it a PNM or PGM file.

   This program relies on the availability of a 2^24+256=16777472-color
palette and 2^20=1048576 color pairs.  Thus,  any color can be specified,
and up to a million combinations of foreground and background color.
In each cell,  an ACS_BBLOCK ('bottom half block') is shown;  the
bottom half then appears in the foreground color for that character
cell,  and the upper half in the background color for that cell.  Thus,
the 'pixels' making up our image are one character wide,  but only
half a character high.

   We start out with the image scaled to fit entirely within the window.
The loop below goes through 2 * LINES iterations.  On even passes,  we
compute RGB values for the pixels that will go into the top half of each
character cell.  On odd passes,  we're computing the RGB for the bottom
halves,  and each time one is computed,  we spit out another character.
(And,  if the colors have changed,  we allocate another color pair.
So this can chew through as many color pairs as there are character
cells on the current screen.)    */

double aspect = 1.1;       /* The 'bottom block' (half-height cell character) */
               /* is _almost_ square,  but is about 10% wider than it is tall */


/* PNM files give each pixel as an RGB triplet of bytes.  A width by height
image consumes exactly width * height * 3 bytes. */

#ifndef ACS_BBLOCK
   #define ACS_BBLOCK 0x2584
#endif

static int get_rgb_value( char *iptr)
{
   unsigned char red = (unsigned char)iptr[0];
   unsigned char grn = (unsigned char)iptr[1];
   unsigned char blu = (unsigned char)iptr[2];

   return( (int)red | ((int)grn << 8) | ((int)blu << 16));
}

/* Image rotation isn't done fancily here.  We allocate another array of equal
size,  copy pixels into it rearranged to their rotated positions,  and copy
that new array back into the original.  */

static void rotate_pixels_ninety_degrees( const int xsize, const int ysize, char *pixels)
{
   int i, j;
   char *tmp = (char *)malloc( xsize * ysize * 3);

   assert( tmp);
   for( i = 0; i < ysize; i++)
      {
      char *tptr = tmp + (ysize - 1 - i) * 3;

      for( j = xsize; j; j--)
         {
         *tptr++ = *pixels++;
         *tptr++ = *pixels++;
         *tptr++ = *pixels++;
         tptr += (ysize - 1) * 3;
         }
      }
   pixels -= xsize * ysize * 3;
   memcpy( pixels, tmp, xsize * ysize * 3);
   free( tmp);
}

static void invert_pixels( const int xsize, const int ysize, char *pixels)
{
   char *endpix = pixels + (xsize * ysize - 1) * 3;

   while( endpix > pixels)
      {
      char tbuff[3];

      tbuff[0] = pixels[0];  pixels[0] = endpix[0];   endpix[0] = tbuff[0];
      tbuff[1] = pixels[1];  pixels[1] = endpix[1];   endpix[1] = tbuff[1];
      tbuff[2] = pixels[2];  pixels[2] = endpix[2];   endpix[2] = tbuff[2];
      pixels += 3;
      endpix -= 3;
      }
}

/* Make a reasonably interesting image with gradients,  circles,
and hyperbolas */

static void make_fake_image( const char *filename)
{
   FILE *ofile = fopen( filename, "wb");
   int i, j, xsize = 640, ysize = 480, r = 150, r1 = 200;
   unsigned char buff[3];

   fprintf( ofile, "P6\n%d %d\n255\n", xsize, ysize);
   for( i = 0; i < ysize; i++)
      for( j = 0; j < xsize; j++)
         {
         const int dx = j - xsize / 2;
         const int dy = i - ysize / 2;

         buff[0] = (unsigned char)( i * 255 / ysize);
         buff[1] = (unsigned char)( j * 255 / xsize);
         if( dx * dx + dy * dy < r1 * r1)
            buff[1] ^= 0xff;
         if( abs( dx * dx - dy * dy) > r * r)
            buff[2] = 0xc0;
         else
            buff[2] = 0;
         fwrite( buff, 3, 1, ofile);
         }
   fclose( ofile);
}

#if defined( __linux)
    const char *temp_image_name = "/tmp/ickywax.pnm";
#else
    const char *temp_image_name = "ickywax.pnm";
#endif

int main( const int argc, const char *argv[])
{
   FILE *ifile;
   int xsize, ysize;
   char *pixels, buff[100];
   const char *filename = temp_image_name;
   const char *filename_to_show = temp_image_name;
   int c = 0, i, bytes_per_pixel = 3;
   double scale = 0., xpix = 0., ypix = 0.;
   bool show_help = TRUE;

   for( i = 1; i < argc; i++)
      if( argv[i][0] == '-')
         switch( argv[i][1])
            {
            case 'a':
               aspect = atof( argv[i] + 2);
               break;
            }
   if( argc > 1 && argv[1][0] != '-')
      {
      filename_to_show = argv[1];
#ifdef CONVERT_IMAGES
      snprintf( buff, sizeof( buff), "convert %s %s", argv[1], temp_image_name);
      if( system( buff))
         {
         printf( "Couldn't convert %s\n", filename);
         return( -1);
         }
#else
      filename = argv[1];
      temp_image_name = NULL;
#endif
      }
   else
      make_fake_image( temp_image_name);
   ifile = fopen( filename, "rb");
   if( !ifile)
      {
      printf( "'%s' not opened\n", filename_to_show);
      return( -2);
      }
   if( !fgets( buff, sizeof( buff), ifile) || *buff != 'P'
               || buff[1] > '6' || buff[1] < '5')
      {
      printf( "'%s' was not a .pnm file\n", filename_to_show);
      return( -3);
      }
   if( buff[1] == '5')
      bytes_per_pixel = 1;
   if( fgets( buff, sizeof( buff), ifile))
      sscanf( buff, "%d %d", &xsize, &ysize);
   if( !fgets( buff, sizeof( buff), ifile))
      return( -4);
   pixels = (char *)calloc( xsize * ysize, 3);
   assert( pixels);
   if( !fread( pixels, xsize * ysize, bytes_per_pixel, ifile))
      {
      printf( "%d x %d pixels not read\n", xsize, ysize);
      return( -5);
      }
   fclose( ifile);
   if( temp_image_name)
      unlink( temp_image_name);
   if( bytes_per_pixel == 1)     /* expand grayscale to RGB */
      for( i = xsize * ysize - 1; i >= 0; i--)
         pixels[i * 3] = pixels[i * 3 + 1] = pixels[i * 3 + 2] = pixels[i];
   initscr( );
   start_color( );
   cbreak( );
   noecho();
   keypad( stdscr, 1);
#ifdef PDCURSES
   mouse_set( ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION);
#endif
   while( c != 27 && c != 'q')
      {
      int *xloc = (int *)calloc( 2 * COLS, sizeof( int));
      int *rgbs = xloc + COLS;
      int prev_rgb = -1, prev_low_rgb = -1;
      int j, pair_num = 16;
      MEVENT mouse_event;
      double xpix1, ypix1;

      assert( xloc);
      if( !scale)       /* recompute scale */
         {
         double scale1 = (double)xsize / (double)COLS;

         scale = (double)ysize / ((double)LINES * 2. * aspect);
         if( scale < scale1)
            scale = scale1;
         xpix = (double)xsize / 2.;
         ypix = (double)ysize / 2.;
         }
      for( i = 0; i < COLS; i++)
         xloc[i] = (int)floor( xpix + ((double)(i - COLS / 2) * scale));
      erase( );
      for( j = 0; j < LINES * 2; j++)
         {
         int yloc = (int)floor( ypix + ((double)(j - LINES) * scale * aspect));
         char *pptr = pixels + yloc * 3 * xsize;

         if( yloc < 0 || yloc >= ysize)
            pptr = NULL;
         for( i = 0; i < COLS && xloc[i] < 0; i++)
            ;
         if( !(j % 2))
            {
            memset( rgbs, 0, COLS * sizeof( int));
            if( pptr)
               for( ; i < COLS && xloc[i] < xsize; i++)
                  rgbs[i] = get_rgb_value( pptr + xloc[i] * 3);
            }
         else
            {
            move( j / 2, i);
            for( ; i < COLS && xloc[i] < xsize; i++)
               {
               int low_rgb = (pptr ? get_rgb_value( pptr + xloc[i] * 3) : 0);

               if( low_rgb != prev_low_rgb || rgbs[i] != prev_rgb)
                  {
                  init_extended_pair( pair_num, low_rgb + 256, rgbs[i] + 256);
                  attrset( COLOR_PAIR( pair_num));
                  pair_num++;
                  prev_low_rgb = low_rgb;
                  prev_rgb = rgbs[i];
                  }
               addch( ACS_BBLOCK);
               }
            }
         }
      free( xloc);
      attrset( 0);
      mvaddstr( LINES - 1, 0, filename_to_show);
      snprintf( buff, sizeof( buff), " %d x %d pixels", xsize, ysize);
      addstr( buff);
      snprintf( buff, sizeof( buff), "     %d color pairs used", pair_num);
      addstr( buff);
      if( show_help)
         {
         mvaddstr( LINES - 7, 0, "Home key sets default view (image fit to screen)");
         mvaddstr( LINES - 6, 0, "Click mouse to pan;  scroll wheel to zoom in/out");
         mvaddstr( LINES - 5, 0, "Cursor keys also pan; * zooms in, / zooms out");
         mvaddstr( LINES - 4, 0, "'r' rotates clockwise, 'R' CCW, 'i' inverts image");
         mvaddstr( LINES - 3, 0, "Any other key causes this help to appear");
         show_help = FALSE;
         }
      refresh();
      xpix1 = xpix;
      ypix1 = ypix;
      do
         {
         c = getch( );
         if( c == KEY_MOUSE)
            {
#ifdef PDCURSES
            nc_getmouse( &mouse_event);
#else
            getmouse( &mouse_event);
#endif
            xpix1 = xpix + (double)(mouse_event.x - COLS / 2) * scale;
            ypix1 = ypix + (double)(mouse_event.y - LINES / 2) * scale * 2. * aspect;
#ifdef PDCURSES
            if( MOUSE_POS_REPORT)
               {
               c = -1;
               snprintf( buff, sizeof( buff), "x=%5d y=%5d", (int)xpix1, (int)ypix1);
               mvaddstr( LINES - 2, 0, buff);
               }
#endif
            }
         }
         while( c == -1);
      switch( c)
         {
#ifdef PDCURSES
         case KEY_MOUSE:
            {
            double rescale = 1.;

            if( MOUSE_WHEEL_UP)
               rescale = 1. / 1.2;
            else if( MOUSE_WHEEL_DOWN)
               rescale = 1.2;
            else if( !MOUSE_MOVED)
               {
               xpix = xpix1;
               ypix = ypix1;
               }
            scale *= rescale;
            xpix += (xpix1 - xpix) * (1. - rescale);
            ypix += (ypix1 - ypix) * (1. - rescale);
            }
            break;
#endif
         case '*':
            scale /= 1.2;
            break;
         case '/':
            scale *= 1.2;
            break;
         case KEY_A1:
         case KEY_HOME:
            scale = 0.;       /* recompute/recenter */
            break;
         case KEY_UP:
#ifdef KEY_A2
         case KEY_A2:
#endif
            ypix -= LINES * scale / 2.;
            break;
         case KEY_LEFT:
#ifdef KEY_B1
         case KEY_B1:
#endif
            xpix -= COLS * scale / 4.;
            break;
         case KEY_DOWN:
#ifdef KEY_C2
         case KEY_C2:
#endif
            ypix += LINES * scale / 2.;
            break;
         case KEY_RIGHT:
#ifdef KEY_B3
         case KEY_B3:
#endif
            xpix += COLS * scale / 4.;
            break;
#ifdef KEY_RESIZE
         case KEY_RESIZE:
            resize_term( 0, 0);
            break;
#endif
         case 'r':
         case 'R':
            {
            const int tval = xsize;

            rotate_pixels_ninety_degrees( xsize, ysize, pixels);
            if( c == 'R')
               invert_pixels( xsize, ysize, pixels);
            xsize = ysize;
            ysize = tval;
            scale = 0.;
            }
            break;
         case 'i':
            invert_pixels( xsize, ysize, pixels);
            break;
         default:
            show_help = TRUE;
            break;
         }
      }
   free( pixels);
   endwin();
#ifdef PDCURSES
   delscreen( SP);
#endif
   return( 0);
}
