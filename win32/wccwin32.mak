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

PLATFORM	= Win32
ARCNAME		= pdc$(VER)_wcc_w32

!include $(PDCURSES_HOME)\wccinc

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)
