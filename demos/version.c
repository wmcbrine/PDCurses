#include <stdio.h>
#include <curses.h>

/* Example usage of the PDC_version structure declared in curses.h.  Note
that this can be used even though we never call initscr().  The idea is
that version checking can be done without attempting function calls,
some of which would fail if (for example) the library had been compiled
with wide characters and the application had not,  or the version numbers
didn't match,  or the size of chtypes differed.  */

int main( const int argc, const char **argv)
{
   static const char *port_text[] = { "X11", "Win32 console", "Win32a",
         "DOS", "OS/2", "SDL1", "SDL2" };

   printf( "PDCurses version %d.%d.%d, chtype size %d\n",
         PDC_version.ver_major,
         PDC_version.ver_minor,
         PDC_version.ver_change,
         (int)PDC_version.chtype_size);
   printf( "Port %d (%s)\n",
         PDC_version.port,
         port_text[PDC_version.port]);
   printf( "is_wide = %d    is_utf8 = %d\n",
         PDC_version.is_wide, PDC_version.is_forced_utf8);
   printf( "longname is '%s'\n", longname( ));
   printf( "Curses version string is '%s'\n", curses_version( ));
   return( 0);
}
