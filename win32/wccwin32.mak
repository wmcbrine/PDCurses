################################################################################
#
# Watcom WMAKE Makefile for PDCurses library - Win32 Watcom C/C++ 10.6+
#
# Usage: wmake -f [win32\]wccwin32.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|panel.lib|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, or edit the line
# below; for example, "set PDCURSES_SRCDIR=c:\pdcurses".
#
################################################################################
PDCURSES_HOME	= $(%PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################

!include $(PDCURSES_HOME)\version

osdir		= $(PDCURSES_HOME)\win32

CC		= wcc386
TARGET		= nt

!ifeq DEBUG Y
CFLAGS		= /d2 /DPDCDEBUG
LDFLAGS		= D W A op q sys $(TARGET)
!else
CFLAGS		= /oneatx
LDFLAGS		= op q sys $(TARGET)
!endif

CPPFLAGS	= /i=$(PDCURSES_HOME) #/DPDC_WIDE
CCFLAGS		= /ei /zq /wx $(CFLAGS) $(CPPFLAGS)

LIBEXE		= wlib /q /n /t

!include $(PDCURSES_HOME)\wccinc

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)

dist: .symbolic $(PDCLIBS)
	echo PDCurses $(VERDOT) for Watcom C++ Win32 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Open Watcom 1.3 for Win32. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_wcc_w32 &
	$(PDCURSES_HOME)\README $(PDCURSES_HOME)\HISTORY &
	$(PDCURSES_HOME)\maintain.er &
	$(PDCURSES_HOME)\curses.h $(PDCURSES_HOME)\curspriv.h &
	$(PDCURSES_HOME)\panel.h $(PDCURSES_HOME)\term.h &
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	del file_id.diz
