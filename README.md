Welcome to PDCurses!
====================

Public Domain Curses, aka PDCurses, is an implementation of X/Open
curses for multiple platforms. Relative to the 'official' PDCurses
project,  this has a variety of improvements,  adding a Windows GUI
and a VT-like console "flavor" of Curses and expanding the
capabilities of the other flavors (SDL1,  SDL2,  X11, Win32 console,
OS/2, and DOS).  Some details and screen shots at

https://www.projectpluto.com/win32a.htm

Much of this was done during a long period in which 'official'
PDCurses was inactive.  It is hoped that the changes in this fork
can be merged into the 'official' PDCurses,  and a pull request has
been made.  But the split is looking increasingly permanent.

Legal Stuff
-----------

The core package is in the public domain, but small portions of PDCurses
are subject to copyright under various licenses.  Each directory
contains a README file, with a section titled "Distribution Status"
which describes the status of the files in that directory.

If you use PDCurses in an application, an acknowledgement would be
appreciated, but is not mandatory. If you make corrections or
enhancements to PDCurses, please forward them to the current maintainer
for the benefit of other users.

This software is provided AS IS with NO WARRANTY whatsoever.


Ports
-----

PDCurses has been ported to DOS, OS/2, Win32, X11 and SDL1 and SDL2,
and this version adds a Windows graphical and VT-like console
flavor. A directory containing the port-specific source files exists
for each of these platforms.

Build instructions are in the README file for each platform:

-  [DOS](dos/README.md) for use on DOS
-  [OS/2](os2/README.md) for use on OS/2
-  [SDL 1.x](sdl1/README.md) for use as separate SDL version 1 window
-  [SDL 2.x](sdl2/README.md) for use as separate SDL version 2 window
-  [wincon](wincon/README.md) (formerly win32) for use on Windows Console
-  [WinGUI](wingui/README.md) for use on Windows Graphics Mode
-  [X11](x11/README.md) (also called XCurses) for use as separate X11 window
-  [VT](vt/README.md) for use on terminal

Distribution Status
-------------------

All files in this directory except configure, config.guess and
config.sub are released to the Public Domain. config.guess and
config.sub are under the GPL; configure is under a free license
described within it.

Build Status
-------------------

PDCurses is automatically build in multiple environments:

[![Build status Travis-CI](https://api.travis-ci.com/Bill-Gray/PDCurses.svg?branch=master)](https://travis-ci.com/Bill-Gray/PDCurses)
, also providing artifacts: [![Build status Appveyor](https://ci.appveyor.com/api/projects/status/github/Bill-Gray/PDCurses?branch=master&svg=true)](https://ci.appveyor.com/project/Bill-Gray/PDCurses)


Maintainer
----------

William McBrine <wmcbrine@gmail.com>  ("official" PDCurses)

Bill Gray
p&#x202e;&ocirc;&#xe7;.&ouml;tulp&#x165;c&eacute;j&ocirc;&#x159;p&#x40;ot&uacute;l&#x202c;m
(this fork)
