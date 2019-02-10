PDCurses for X11
================

This is a port of PDCurses for X11, aka XCurses.  It is designed to
allow existing curses programs to be re-compiled with PDCurses,
resulting in native X11 programs.


Building
--------

- Run "./configure". To build the wide-character version of the library,
  specify "--enable-widec" as a parameter. I recommend this option, but
  I haven't yet made it the default, for the sake of backwards
  compatibility.

  If your system is lacking in UTF-8 support, you can force the use of
  UTF-8 instead of the system locale via "--enable-force-utf8".

  If configure can't find your X include files or X libraries, you can
  specify the paths with the arguments "--x-includes=inc_path" and/or
  "--x-libraries=lib_path".

  By default, the library and demo programs are built with the optimizer
  switch -O2. You can turn this off, and turn on debugging (-g), by
  adding "--enable-debug" to the configure command.

- Run "make". This should build libXCurses. Add the target "demos" to
  build the sample programs.

- Optionally, run "make install". To avoid conflicts with any existing
  curses installations, copies of curses.h and panel.h are installed in
  (by default) /usr/local/include/xcurses.


Usage
-----

PDCurses for X11 uses the System V IPC shared memory facility, along
with sockets, to share data between the curses program and the child
process created to manage the X stuff.

When compiling your application, you need to include the \<curses.h\>
that comes with PDCurses. You also need to link your code with
libXCurses. You may need to link with the following libraries:

    Xaw Xmu Xt X11 SM ICE Xext Xpm

You can run "xcurses-config --libs" to show the link parameters for your
system. If using dynamic linking, on some systems, "-lXCurses" suffices.

By calling Xinitscr() rather than initscr(), you can pass your program
name and resource overrides to PDCurses. The program name is used as the
title of the X window, and for defining X resources specific to your
program.


Interaction with stdio
----------------------

Be aware that curses programs that expect to have a normal tty
underneath them will be very disappointed! Output directed to stdout
will go to the xterm that invoked the PDCurses application, or to the
console if not invoked directly from an xterm. Similarly, stdin will
expect its input from the same place as stdout.


X Resources
-----------

PDCurses for X11 recognizes the following resources:

### lines

Specifies the number of lines the "screen" will have. Directly equates
to LINES. There is no theoretical maximum. The minimum value must be 2.
Default: 24

### cols

Specifies the number of columns the "screen" will have. Directly equates
to COLS. There is no theoretical maximum. The minimum value must be 2.
Default: 80

### normalFont

The name of a fixed width font. Defaults:
- narrow: -misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1
- wide: -misc-fixed-medium-r-normal--20-200-75-75-c-100-iso10646-1

### italicFont

The name of a fixed width font to be used for characters with A_ITALIC
attributes. Must have the same cell size as normalFont. Defaults:
- narrow: -misc-fixed-medium-o-normal--13-120-75-75-c-70-iso8859-1
- wide: -misc-fixed-medium-o-normal--20-200-75-75-c-100-iso10646-1

### boldFont

The name of a fixed width font to be used for characters with A_BOLD
attributes. Must have the same cell size as normalFont. Defaults:
- narrow: -misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-1
- wide: -misc-fixed-bold-r-normal--20-200-75-75-c-100-iso10646-1

### pointer

The name of a valid pointer cursor. Default: xterm

### pointerForeColor

The foreground color of the pointer. Default: black

### pointerBackColor

The background color of the pointer. Default: white

### cursorColor

### textCursor

The alignment of the text cursor; horizontal or vertical. Default:
horizontal

### colorBlack

The color of the COLOR_BLACK attribute. Default: Black

### colorRed

The color of the COLOR_RED attribute. Default: red3

### colorGreen

The color of the COLOR_GREEN attribute. Default: green3

### colorYellow

The color of the COLOR_YELLOW attribute. Default: yellow3

### colorBlue

The color of the COLOR_BLUE attribute. Default: blue3

### colorMagenta

The color of the COLOR_MAGENTA attribute. Default: magenta3

### colorCyan

The color of the COLOR_CYAN attribute. Default: cyan3

### colorWhite

The color of the COLOR_WHITE attribute. Default: Grey

### colorBoldBlack

COLOR_BLACK combined with A_BOLD. Default: grey40

### colorBoldRed

COLOR_RED combined with A_BOLD. Default: red1

### colorBoldGreen

COLOR_GREEN combined with A_BOLD. Default: green1

### colorBoldYellow

COLOR_YELLOW combined with A_BOLD. Default: yellow1

### colorBoldBlue

COLOR_BLUE combined with A_BOLD. Default: blue1

### colorBoldMagenta

COLOR_MAGENTA combined with A_BOLD. Default: magenta1

### colorBoldCyan

COLOR_CYAN combined with A_BOLD. Default: cyan1

### colorBoldWhite

COLOR_WHITE combined with A_BOLD. Default: White

### bitmap

The name of a valid bitmap file of depth 1 (black and white) used for
the application's icon. The file is an X bitmap. Default: none

### pixmap

The name of a valid pixmap file of any depth supported by the window
manager (color) for the application's icon, The file is an X11 pixmap.
This resource overrides the "bitmap" resource. Default: a 32x32 or 64x64
pixmap depending on the window manager

### translations

Translations enable the user to customize the action that occurs when a
key, combination of keys, or a button is pressed.  The translations are
similar to those used by xterm.

Defaults:

    <Key>:        XCursesKeyPress()
    <KeyUp>:      XCursesKeyPress()
    <BtnDown>:    XCursesButton()
    <BtnUp>:      XCursesButton()
    <BtnMotion>:  XCursesButton()

The most useful action for KeyPress translations is string(). The
argument to the string() action can be either a string or a hex
representation of a character; e.g., string(0x1b) will send the ASCII
escape character to the application; string("[11~") will send [ 1 1 ~ ,
as separate keystrokes.

### shmmin

On most systems, there are two Unix kernel parameters that determine the
allowable size of a shared memory segment. These parameters are usually
something like SHMMIN and SHMMAX. To use shared memory, a program must
allocate a segment of shared memory that is between these two values.
Usually these values are like 1 for SHMMIN and some large number for
SHMMAX. Sometimes the Unix kernel is configured to have a value of
SHMMIN that is bigger than the size of one of the shared memory segments
that libXCurses uses. On these systems an error message like:

    Cannot allocate shared memory for SCREEN: Invalid argument

will result. To overcome this problem, this resource should be set to
the kernel value for SHMMIN. This ensures that a shared memory segment
will always be bigger than the kernel value for SHMMIN (and hopefully
less than SHMMAX!) Default: 0

### borderColor

The color of the border around the screen. Default: black

### borderWidth

The width in pixels of the border around the screen. Default: 0

### clickPeriod

The period (in milliseconds) between a button press and a button release
that determines if a click of a button has occurred. Default: 100

### doubleClickPeriod

The period (in milliseconds) between two button press events that
determines if a double click of a button has occurred. Default: 200


Using Resources
---------------

All applications have a top-level class name of "XCurses". If Xinitscr()
is used, it sets an application's top-level widget name. (Otherwise the
name defaults to "PDCurses".)

Examples for app-defaults or .Xdefaults:

    !
    ! resources for XCurses class of programs
    !
    XCurses*lines:  30
    XCurses*cols:   80
    XCurses*normalFont:     9x13
    XCurses*bitmap: /tmp/xcurses.xbm
    XCurses*pointer: top_left_arrow
    !
    ! resources for testcurs - XCurses
    !
    testcurs.colorRed:      orange
    testcurs.colorBlack:    midnightblue
    testcurs.lines: 25
    *testcurs.Translations: #override \n \
      <Key>F12:  string(0x1b) string("[11~") \n
    !
    ! resources for THE - XCurses
    !
    ! resources with the * wildcard can be overridden by a parameter passed
    ! to initscr()
    !
    the*normalFont: 9x15
    the*lines:      40
    the*cols:       86
    the*pointer:    xterm
    the*pointerForeColor: white
    the*pointerBackColor: black
    !
    ! resources with the . format can not be overridden by a parameter passed
    ! to Xinitscr()
    !
    the.bitmap:     /home/mark/the/the64.xbm
    the.pixmap:     /home/mark/the/the64.xpm

Resources may also be passed as parameters to the Xinitscr() function.
Parameters are strings in the form of switches; e.g., to set the color
"red" to "indianred", and the number of lines to 30, the string passed
to Xinitscr would be: "-colorRed indianred -lines 30"


Deprecated
----------

XCursesProgramName is no longer used. To set the program name, you must
use Xinitscr(), or PDC_set_title() to set just the window title.

The XCursesExit() function is now called automatically via atexit().
(Multiple calls to it are OK, so you don't need to remove it if you've
already added it for previous versions of PDCurses.)

XCURSES is no longer defined automatically, but need not be defined,
unless you want the X11-specific prototypes. (Normal curses programs
won't need it.)


Distribution Status
-------------------

As of April 13, 2006, the files in this directory are released to the
public domain, except for ScrollBox*, which are under essentially the
MIT X License; config.guess and config.sub, which are under the GPL; and
configure, which is under a free license described within it.


Acknowledgements
----------------

X11 port was provided by Mark Hessling <mark@rexx.org>
