Welcome to PDCurses!
====================

Public Domain Curses, aka PDCurses, is an implementation of X/Open
curses for multiple platforms. Relative to the 'official',  but
mostly inactive,  PDCurses project,  this has a variety of
improvements,  adding a Windows GUI "flavor" of Curses and
expanding the capabilities of the other flavors (SDL,  X11,
Win32 console,  OS/2,  and DOS).  Some details and screen shots at

http://www.projectpluto.com/win32a.htm

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

PDCurses has been ported to DOS, OS/2, Win32, X11 and SDL,  and
this version adds a Windows graphical flavor. A directory
containing the port-specific source files exists for each of
these platforms.

Build instructions are in the README file for each platform:

-  [DOS](dos/README.md)
-  [OS/2](os2/README.md)
-  [SDL](sdl1/README.md)
-  [SDL2](sdl2/README.md)
-  [Win32](win32/README.md)
-  [Win32a](win32a/README.md)
-  [X11](x11/README.md)

Distribution Status
-------------------

All files in this directory except configure, config.guess and
config.sub are released to the Public Domain. config.guess and
config.sub are under the GPL; configure is under a free license
described within it.


Maintainer
----------

William McBrine <wmcbrine@gmail.com>  ("official" PDCurses)

Bill Gray
p&#x202e;&ocirc;&#xe7;.&ouml;tulp&#x165;c&eacute;j&ocirc;&#x159;p&#x40;ot&uacute;l&#x202c;m
(this fork)
