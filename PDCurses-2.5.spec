Summary: Public Domain Curses
Name: PDCurses
Version: 2.5
Release: 1
Copyright: LGPL
Group: Development/Languages
Source: ftp://ftp.lightlink.com/pub/hessling/PDCurses/PDCurses-2.5.tar.gz
%description
PDCurses is an implementation of the curses library for X11. It provides
the ability for existing text-mode curses programs to be re-built as
native X11 applications with very little modification.
PDCurses for X11 is also known as XCurses.
For more information on PDCurses, visit http://www.lightlink.com/hessling/
%prep
%setup

%build
./configure
make

%install
make install

%files
/usr/local/lib/libXCurses.a
/usr/local/lib/libpanel.a
/usr/local/lib/libXCurses.so
/usr/local/include/xcurses.h
/usr/local/include/xpanel.h

