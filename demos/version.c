#include <stdio.h>
#include <curses.h>

/* Example usage of the PDC_get_version function declared in curses.h.  The
general idea is that,  while we have various macros defined to tell us
which version of PDCurses we're using (PDC_VER_MAJOR,  PDC_VER_MINOR,
PDC_WIDE,  etc.),  these may not match the library to which we are linked;
it may have been compiled with different flags,  or a different chtype size
(16,  32,  or 64-bit),  etc. This is a small issue with static libraries,
but with DLLs (Windows and OS/2) or shared libraries (*nix and *BSD),  we
may not really control which DLL/shared library happens to be available.

In such cases,  the PDC_VERSION structure tells us how the library was
compiled.

Note that this can be used even though we never call initscr().  The idea
is that version checking can be done without attempting function calls,
some of which would fail if (for example) the library had been compiled
with wide characters and the application had not,  or the version numbers
didn't match,  or the size of chtypes differed.  */

int main( const int argc, const char **argv)
{
   static const char *port_text[] = { "X11", "Win32 console", "Win32a",
         "DOS", "OS/2", "SDL1", "SDL2", "VT" };
   PDC_VERSION vinfo;

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

   PDC_get_version( &vinfo);
   printf( "\nInformation on the library :\n");
   printf( "PDCurses version %d.%d.%d, chtype size %d\n",
         (int)vinfo.major, (int)vinfo.minor,
         (int)vinfo.change, (int)vinfo.csize);
   printf( "Port %d (%s)\n",
         vinfo.port,
         port_text[vinfo.port]);
   printf( "debug = %c   is_wide = %c    is_utf8 = %c\n",
         vinfo.flags & PDC_VFLAG_DEBUG ? 'Y' : 'N',
         vinfo.flags & PDC_VFLAG_WIDE ? 'Y' : 'N',
         vinfo.flags & PDC_VFLAG_UTF8 ? 'Y' : 'N');
   printf( "is_rgb = %c    is_dll = %c\n",
         vinfo.flags & PDC_VFLAG_RGB ? 'Y' : 'N',
         vinfo.flags & PDC_VFLAG_DLL ? 'Y' : 'N');
   printf( "longname is '%s'\n", longname( ));
   printf( "Curses version string is '%s'\n", curses_version( ));

   if( vinfo.major != PDC_VER_MAJOR
             || vinfo.minor != PDC_VER_MINOR
             || vinfo.change != PDC_VER_CHANGE)
      printf( "Versions do not match!\n");
   return( 0);
}
