PDCurses for DOS
================

This directory contains PDCurses source code files specific to DOS.


Building
--------

- Choose the appropriate makefile for your compiler:

        Makefile     - DJGPP V2
        Makefile.bcc - Borland C++ 3.0+
        Makefile.wcc - Open Watcom 1.8+ (16-bit or 32-bit)
        Makefile.dmc - Digital Mars
        Makefile.msc - Microsoft C

- For 16-bit compilers, you can change the memory MODEL in the makefile.
  (Large model is the default, and recommended.)

  (For Open Watcom, pass MODEL=f (flat model) to "wmake" in order to use
  the 32-bit compiler. Selcting a 16-bit MODEL (e.g. MODEL=l) will
  invoke the 16-bit compiler.)

- Optionally, you can build in a different directory than the platform
  directory by setting PDCURSES_SRCDIR to point to the directory where
  you unpacked PDCurses, and changing to your target directory:

        set PDCURSES_SRCDIR=c:\pdcurses

- Build it:

        make -f makefile

  (For Watcom, use "wmake" instead of "make"; for MSVC, "nmake".) You'll
  get the libraries (pdcurses.lib or .a, depending on your compiler; and
  panel.lib or .a), the demos (*.exe), and a lot of object files.


Distribution Status
-------------------

The files in this directory are released to the Public Domain.


Acknowledgements
----------------

Watcom C port was provided by Pieter Kunst <kunst@prl.philips.nl>

DJGPP 1.x port was provided by David Nugent <davidn@csource.oz.au>
