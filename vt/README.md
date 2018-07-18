PDCurses for VT
================

This directory contains source code to support PDCurses using a mix
of VT-100,  VT-200,  and ANSI escape sequences to set colors,  position
the cursor,  etc.  Note that the name is misleading;  it uses an olio of
control sequences from

https://www.gnu.org/software/screen/manual/html_node/Control-Sequences.html

which may,  or may not,  work on your terminal (I've tested urxvt,  xterm,
and QTerminal).

It assumes that 256 colors are available,  but it can use full RGB on
terminals that support it.

This is a first pass;  it mostly works,  but some work remains to be done
(see 'Caveats' below).

Building
--------

In Linux,  run `make` or `make WIDE=Y`.  In *BSD,  use `gmake` or `gmake WIDE=Y`.

In theory,  this could be tweaked to work in Windows 10,  which now has
some VT emulation.  I do Windows as little as possible,  so don't rely on
me for that part.

Caveats
-------

As currently set up,  this is a bare-bones implementation.  It relies on a
terminal that supports at least 256 colors.  RGB colors get remapped to the
6x6x6 color cube.  If your terminal supports full RGB coloring (QTerminal
and modern xterm, for example),  you can set

`PDC_VT=RGB`

`export PDC_VT`

This will eventually be expanded so one can explain to PDCurses the
capabilities of the terminal,  e.g.,

`PDC_VT=RGB BOLD ITALIC BLINK`

to say that the underlying terminal supports built-in bold,  italic,  and
blinking text.  (And/or the code may dig around in the terminfo database,
as ncurses does.  That would be the Right Way of doing it.  Or at the
very least,  look at the TERM and COLORTERM environment variables.)

Arrow keys and some function keys are recognized (see the `tbl` array
in `pdckey.c`).  Some mouse input is recognized.  Shift,  Ctrl,  and Alt
function keys and arrows are (mostly) not correctly identified;  I've
not figured out how those keys are supposed to be detected yet.  Or if
they can be.

Test routines have been run on rxvt,  xterm,  and QTerminal only,  on
one Xubuntu box,  plus some testing on TrueOS.

Distribution Status
-------------------

The files in this directory are released to the Public Domain.

