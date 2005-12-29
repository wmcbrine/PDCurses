# $Id: PDCurses.spec,v 1.5 2005/12/29 00:02:18 wmcbrine Exp $
%define prefix /usr

Summary: Public Domain Curses for X11
Name: PDCurses
Version: 2.7
Release: 1
Copyright: LGPL
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
URL: http://pdcurses.sourceforge.net
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires: /sbin/ldconfig

%description
PDCurses is an implementation of the curses library for X11. It provides
the ability for existing text-mode curses programs to be re-built as
native X11 applications with very little modification.
PDCurses for X11 is also known as XCurses.
For more information on PDCurses, visit http://pdcurses.sourceforge.net

%prep

%setup -q

%build
./configure --prefix=%{prefix}
make 

%install
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/%{name}-%{version}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{prefix}/bin/xcurses-config
%{prefix}/lib/libXCurses.a
%{prefix}/lib/libXpanel.a
%{prefix}/lib/libXCurses.so
%{prefix}/include/xcurses.h
%{prefix}/include/xpanel.h
