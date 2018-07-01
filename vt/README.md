PDCurses for VT
================

This directory contains PDCurses source code files specific to VT.  Note
that the name is misleading;  it uses an olio of control sequences from

https://www.gnu.org/software/screen/manual/html_node/Control-Sequences.html

some of which are VT100 or VT200 or ANSI or xterm sequences.  It assumes
that 256 colors are available.

This is a first pass.  Much work remains to be done (see 'Caveats' below).

Building
--------

In Linux,  run `make` or `make WIDE=Y`.  In *BSD,  use `gmake` or `gmake WIDE=Y`.

In theory,  this could be tweaked to work in Windows 10,  which now has
some VT emulation.

Caveats
-------

As currently set up,  this is a bare-bones implementation.  It relies on a
terminal that supports 256 colors.  RGB colors get remapped to the 6x6x6
color cube.  If your terminal supports full RGB coloring (QTerminal and
modern xterm, for example),  you can set

PDC_VT=RGB
export PDC_VT

This will eventually be expanded so one can explain to PDCurses the
capabilities of the terminal,  e.g.,

PDC_VT=RGB BOLD ITALIC BLINK

to say that the underlying terminal supports built-in bold,  italic,  and
blinking text.  (And/or the code may dig around in the terminfo database,
which would be the Right Way.)

Arrow keys and some function keys are recognized (see the `tbl` array
in `pdckey.c`).  Some mouse input is recognized.

Test routines have been run on rxvt,  xterm,  and QTerminal only,  on
one Xubuntu box,  plus some testing on TrueOS.

Distribution Status
-------------------

The files in this directory are released to the Public Domain.

