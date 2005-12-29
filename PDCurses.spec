# $Id: PDCurses.spec,v 1.7 2005/12/29 03:05:45 wmcbrine Exp $

%define ver 27
%define verdot 2.7
%define base /usr

Summary: Public Domain Curses for X11
Name: PDCurses
Version: %verdot
Release: 1
Copyright: LGPL
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
URL: http://pdcurses.sourceforge.net
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Prefix: %base

%description
PDCurses for X11, also known as XCurses, is an implementation of the 
curses library that lets you rebuild existing text-mode curses programs 
as native X11 applications, with very little modification. For more 
information, visit http://pdcurses.sourceforge.net

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
%doc README readme.%{ver} maintain.er
%{base}/bin/xcurses-config
%{base}/lib/libXCurses.a
%{base}/lib/libXpanel.a
%{base}/lib/libXCurses.so
%{base}/include/xcurses.h
%{base}/include/xpanel.h
