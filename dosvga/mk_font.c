#include <stdio.h>
#include <assert.h>

/* Code to convert 8x16 (or shorter height) VGA fonts to a C array.
The input file must be exactly 256 * font_height bytes long,  i.e.,
2560 bytes for an 8x10 font.  At least thus far,  fonts with
widths other than 8 and/or heights greater than 16 are not handled;
we're also assuming 256 CP437 glyphs.  Redirected to 'font.h',
this basically does all the conversion required. */

#define MAX_SIZE 4096

int main( const int argc, const char **argv)
{
   FILE *ifile = fopen( argv[1], "rb");
   int i = 0, bytes_read;
   unsigned char buff[MAX_SIZE];

   assert( ifile);
   bytes_read = (int)fread( buff, 1, sizeof( buff), ifile);
   fclose( ifile);
   assert( bytes_read % 256 == 0);
   printf( "/* Converted from %s using mk_font.c (q.v.) */\n", argv[1]);
   printf( "/* Fonts found in https://hack.org/mc/fonts/vgafonts.tar.gz */\n");
   printf( "unsigned char font_bytes[%d] = {\n   ", bytes_read);
   for( i = 0; i < bytes_read; i++)
      {
      if( i == bytes_read - 1)
         printf( "0x%02x };\n\n", (int)buff[i]);
      else
         printf( "0x%02x,%s", (int)buff[i], (i % 8 != 7) ? " " : "\n   ");
      }
   printf( "int PDC_font_height = %d;\n", bytes_read / 256);
   printf( "int PDC_font_width = 8;\n");
}
