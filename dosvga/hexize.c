#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cp437.h"

/* This code reads through Roman Czyborra's Unifont.hex file,  from

http://unifoundry.com/unifont/index.html

and produces a simple binary representation : sixteen bytes per glyph,
256 glyphs.  It uses the cp437.h data to figure out which Unicode glyph
corresponds to a CP437 glyph,  and writes them out in the form you
see in 'font.h'.

At some point,  I expect to do something similar to create a font
for a wide-char build,  in which event we'll skip the CP437 remapping
(and will have fullwidth characters and,  conceivably,  SMP and
combining chars to handle.)         */

int main( const int argc, const char **argv)
{
   int i, j;
   unsigned idx;
   FILE *ifile = fopen( argv[1], "rb");
   char buff[90];
   char **strings = (char **)calloc( 0x3000, sizeof( char *));

   while( fgets( buff, sizeof( buff), ifile))
      if( sscanf( buff, "%x", &idx) && idx < 0x3000)
         {
         strings[idx] = (char *)malloc( strlen( buff) + 1);
         strcpy( strings[idx], buff);
         }
   fclose( ifile);

   for( i = 0; i < 256; i++)
      {
      char *text = strings[cp437_to_unicode[i]];

      for( j = 0; j < 16; j++)
         printf( " 0x%.2s,", text + 2 * j + 5);
      printf( "\n");
      }
   return( 0);
}
