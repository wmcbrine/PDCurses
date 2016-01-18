# $Id: PDCurses.spec,v 1.6 2013/07/23 01:35:31 mark Exp $
%define debug_package %{nil}
%define base /usr

Summary: Public Domain Curses for X11
Name: PDCurses
Version: %{myversion}
Release: 1
License: Public Domain
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
URL: http://pdcurses.org
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Prefix: %base
BuildRequires: libXaw-devel

%package devel
Summary: Public Domain Curses for X11 Development Files
Group: Development/Libraries
Requires: %{name}%{?_isa} = %{version}-%{release}

%description
PDCurses for X11 is an implementation of the curses library that lets
you build text-mode curses programs as native X11 applications.
This package contains the runtime library.

For more information, visit http://pdcurses.org

%description devel
PDCurses for X11 is an implementation of the curses library that lets
you build text-mode curses programs as native X11 applications.
This package contains the development files.

For more information, visit http://pdcurses.org

%prep
%setup -q

%build
cd x11
./configure --prefix=%{base} --libdir=%{_libdir} --enable-libdir-change=no
make

%install
cd x11
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root)
%{_libdir}/libXCurses.so.%{version}
%{_libdir}/libXpanel.so.%{version}

%files devel
%defattr(-,root,root)
%{_bindir}/xcurses-config
%{_libdir}/libXCurses.a
%{_libdir}/libXpanel.a
%{_libdir}/libXCurses.so
%{_libdir}/libXCurses.so.3
%{_libdir}/libXpanel.so
%{_libdir}/libXpanel.so.3
%{_includedir}/xcurses.h
%{_includedir}/xpanel.h
%{_includedir}/xcurses/curses.h
%{_includedir}/xcurses/panel.h
%{_includedir}/xcurses/term.h

%post
ldconfig %{_libdir}

%postun
ldconfig %{_libdir}

%post devel
ldconfig %{_libdir}

%postun devel
ldconfig %{_libdir}
