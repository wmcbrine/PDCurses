PDCurses for VT
================

This directory contains source code to support PDCurses using a mix of
xterm,  VT-100,  VT-200,  and ANSI escape sequences to set colors,  position
the cursor,  etc.  Note that the name is misleading;  it uses an olio of
control sequences from

https://www.gnu.org/software/screen/manual/html_node/Control-Sequences.html

which may,  or may not,  work on your terminal.  I've tested it in urxvt,
xterm, and QTerminal on Linux and FreeBSD,  and in `cmd` on Win10 and (with
NANSI.COM or NANSI.SYS)  on Windows ME.  MS-DOS (again with NANSI) and
Linux console modes 'sort of' work (no mouse and the colors need work).

It assumes that 256 colors are available (16 in NANSI mode),  but it can use
full RGB on terminals that support it.

Building
--------

In GNU/Linux,  run `make` or `make WIDE=Y`.  You can add `-w64` or `-w32`
to cross-compile 64-bit or 32-bit Windows executables,  using MinGW64.

In *BSD,  use `gmake` or `gmake WIDE=Y`.  Cross-compiling to Windows
should be possible there as well.

For DOS/Windows,  makefiles for Borland,  Digital Mars,  MSVC, and
OpenWATCOM are provided,  but are at the 'it works on my machine' stage,
and haven't really been thoroughly vetted by others yet.

Caveats
-------

As currently set up,  this is a bare-bones implementation.  It relies on a
terminal that supports at least 256 colors.  RGB colors get remapped to the
6x6x6 color cube.  If the `COLORTERM` environment variable is set to
`truecolor`,  you'll get true color.  If not,  you can tell PDCurses you
really do have true color by setting

`PDC_VT=RGB`

`export PDC_VT`

Note that you can explain to PDCurses more capabilities of the terminal,  e.g.,

`PDC_VT=RGB UNDERLINE BLINK DIM STANDOUT`

to say that the underlying terminal supports true-color,  underlined,
blinking,  dimmed,  and 'standout' text.  (The Right Thing to Do here
would be to dig around in the terminfo database,  as ncurses does,  both
to know the control sequences to use and the actual capabilities of
the terminal.)

Arrow keys and some function keys are recognized (see the `tbl` array
in `pdckey.c`).  Some mouse input is recognized.  Shift,  Ctrl,  and Alt
function keys and arrows are (mostly) not correctly identified;  I've
not figured out how those keys are supposed to be detected yet.  Or if
they can be.  None of the 'extended' keys found on some keyboards,  such
as Browser Back/Forward,  Search,  Refresh,  Stop,  etc.,  are detected,
on any platform.

Clipboard functions are currently completely absent on this platform.  I
expect to be able to add clipboard functions for Windows by recycling
code from the Windows GUI and console flavors,  and have clipboard
access code for X11 that is not yet included here.

Distribution Status
-------------------

The files in this directory are released to the Public Domain.

