Summary: Public Domain Curses
Name: PDCurses
Version: 2.6
Release: 2
Copyright: Some portions 1995-2003 Mark Hessling
Distribution: Public Domain and LGPL
Vendor: Mark Hessling
Packager: Mark Hessling
Group: Development/Languages
Source: ftp://prdownloads.sourceforge.net/pdcurses/PDCurses-2.6.tar.gz
URL: http://pdcurses.sourceforge.net

%description
PDCurses is an implementation of the curses library for X11. It provides
the ability for existing text-mode curses programs to be re-built as
native X11 applications with very little modification.
PDCurses for X11 is also known as XCurses.
For more information on PDCurses, visit http://pdcurses.sourceforge.net

%prep

%setup

%build
./configure
make

%install
make install

%files
/usr/lib/libXCurses.a
/usr/lib/libXpanel.a
/usr/lib/libXCurses.so
/usr/include/xcurses.h
/usr/include/xpanel.h

