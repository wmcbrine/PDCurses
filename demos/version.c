#include <stdio.h>
#include <curses.h>

/* Example usage of the PDC_version structure declared in curses.h.  The
general idea is that,  while we have various macros defined to tell us
which version of PDCurses we're using (PDC_VER_MAJOR,  PDC_VER_MINOR,
PDC_WIDE,  etc.),  these may not match the library to which we are linked;
it may have been compiled with different flags,  or a different chtype size
(16,  32,  or 64-bit),  etc. This is a small issue with static libraries,
but with DLLs (Windows and OS/2) or shared libraries (*nix and *BSD),  we
may not really control which DLL/shared library happens to be available.

In such cases,  the PDC_version structure (declared within the library but
global/available to the application) tells us how the library was compiled.

Note that this can be used even though we never call initscr().  The idea
is that version checking can be done without attempting function calls,
some of which would fail if (for example) the library had been compiled
with wide characters and the application had not,  or the version numbers
didn't match,  or the size of chtypes differed.  */

int main( const int argc, const char **argv)
{
   static const char *port_text[] = { "X11", "Win32 console", "Win32a",
         "DOS", "OS/2", "SDL1", "SDL2" };

   printf( "Compiled as version %d.%d.%d, chtype size %d\n",
            PDC_VER_MAJOR, PDC_VER_MINOR, PDC_VER_CHANGE,
            (int)sizeof( chtype));
#ifdef PDC_WIDE
   printf( "Compiled with PDC_WIDE defined\n");
#else
   printf( "Compiled with PDC_WIDE undefined\n");
#endif
#ifdef PDC_FORCE_UTF8
   printf( "Compiled with PDC_FORCE_UTF8 defined\n");
#else
   printf( "Compiled with PDC_FORCE_UTF8 undefined\n");
#endif


   printf( "\nInformation on the library :\n");
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

   if( PDC_version.ver_major != PDC_VER_MAJOR
         || PDC_version.ver_minor != PDC_VER_MINOR
         || PDC_version.ver_change != PDC_VER_CHANGE)
      printf( "Versions do not match!\n");
   return( 0);
}
