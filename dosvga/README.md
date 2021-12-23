PDCurses for DOS
================

This directory contains PDCurses source code files specific to the
VGA-capable DOS port.


Building
--------

- Choose the appropriate makefile for your compiler:

        Makefile     - DJGPP
        Makefile.bcc - Borland C++
        Makefile.wcc - Watcom

- For 16-bit compilers, you can change the memory MODEL as a command-
  line option. (Large model is the default, and recommended.) With
  Watcom, specifying "MODEL=f" (flat) will automatically switch to a
  32-bit build.

- Optionally, you can build in a different directory than the platform
  directory by setting PDCURSES_SRCDIR to point to the directory where
  you unpacked PDCurses, and changing to your target directory:

        set PDCURSES_SRCDIR=c:\pdcurses

- Build it:

        make -f makefile

  (For Watcom, use "wmake" instead of "make".) You'll get the library
  (pdcurses.lib or .a, depending on your compiler) and a lot of object
  files. Add the target "demos" to build the sample programs.

  You can also give the optional parameter "WIDE=Y", to build the
  library with wide-character (Unicode) support:

        wmake -f Makefile.wcc WIDE=Y

  (WIDE=Y is untested on the Borland compiler.)

The font can be set via the environment variable PDC_FONT. The font must
be in the PSF format, version 2.


Distribution Status
-------------------

The files in this directory are released to the public domain.


Acknowledgements
----------------

Watcom C port was provided by Pieter Kunst <kunst@prl.philips.nl>

DJGPP port was provided by David Nugent <davidn@csource.oz.au>
