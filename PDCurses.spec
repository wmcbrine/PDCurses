# $Id: PDCurses.spec,v 1.13 2007/04/14 13:26:41 wmcbrine Exp $

%define ver 31
%define verdot 3.1
%define base /usr

Summary: Public Domain Curses for X11
Name: PDCurses
Version: %verdot
Release: 1
Copyright: Public Domain
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
URL: http://pdcurses.sourceforge.net
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Prefix: %base

%description
PDCurses for X11 is an implementation of the curses library that lets 
you build text-mode curses programs as native X11 applications. For more 
information, visit http://pdcurses.sourceforge.net/

%prep
%setup -q

%build
./configure --prefix=%{base}
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
%doc README HISTORY maintain.er
%{base}/bin/xcurses-config
%{base}/lib/libXCurses.a
%{base}/lib/libXpanel.a
%{base}/lib/libXCurses.so
%{base}/include/xcurses.h
%{base}/include/xpanel.h
%{base}/include/xcurses/curses.h
%{base}/include/xcurses/curspriv.h
%{base}/include/xcurses/panel.h
%{base}/include/xcurses/term.h
