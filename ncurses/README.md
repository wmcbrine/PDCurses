ncurses build of PDCurses demos
===============================

This directory contains a makefile to compile the PDCurses demos against
the ncurses library.  This allows us to check for differences between
ncurses and PDCurses.

Building
--------

- Thus far,  only GCC on Linux has been tested.  Build with either

        make -f makefile
        make -f makefile WIDE=Y

  Several demos will be compiled.

Distribution Status
-------------------

The files in this directory are released to the Public Domain.

Acknowledgments
---------------

makefile provided by Bill Gray.
