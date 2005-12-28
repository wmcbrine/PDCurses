# $Id: PDCurses.spec,v 1.2 2005/12/28 21:12:46 wmcbrine Exp $
%define _sourcedir /home/wmcbrine
%define _builddir %{_tmppath}
%define _srcrpmdir %{_sourcedir}
%define _rpmdir %{_sourcedir}
%define prefix /usr

Summary: Public Domain Curses for X11
Name: PDCurses
Version: 2.7
Release: 1
Copyright: LGPL
Packager: William McBrine <wmcbrine@users.sf.net>
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
URL: http://pdcurses.sourceforge.net
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
PDCurses is an implementation of the curses library for X11. It provides
the ability for existing text-mode curses programs to be re-built as
native X11 applications with very little modification.
PDCurses for X11 is also known as XCurses.
For more information on PDCurses, visit http://pdcurses.sourceforge.net

%prep

%setup

%build
./configure --prefix=%{prefix}
make 

%install
make DESTDIR=$RPM_BUILD_ROOT install

%files
%{prefix}/lib/libXCurses.a
%{prefix}/lib/libXpanel.a
%{prefix}/lib/libXCurses.so
%{prefix}/include/xcurses.h
%{prefix}/include/xpanel.h
