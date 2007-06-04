#########################################################################
#
# Watcom WMAKE Makefile for PDCurses library - Win32 Watcom C/C++ 10.6+
#
# Usage: wmake -f [win32\]wccwin32.mak [DEBUG=Y] [WIDE=Y] [UTF8=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]
#
#########################################################################

!ifdef %PDCURSES_SRCDIR
PDCURSES_HOME	= $(%PDCURSES_SRCDIR)
!else
PDCURSES_HOME	= ..
!endif

!include $(PDCURSES_HOME)\version.mif

osdir		= $(PDCURSES_HOME)\win32

CC		= wcc386
TARGET		= nt

CFLAGS		= /ei /zq /wx /i=$(PDCURSES_HOME)

!ifeq DEBUG Y
CFLAGS		+= /d2 /DPDCDEBUG
LDFLAGS		= D W A op q sys $(TARGET)
!else
CFLAGS		+= /oneatx
LDFLAGS		= op q sys $(TARGET)
!endif

!ifeq WIDE Y
CFLAGS		+= /DPDC_WIDE
!endif

!ifeq UTF8 Y
CFLAGS		+= /DPDC_FORCE_UTF8
!endif

LIBEXE		= wlib /q /n /t

!include $(PDCURSES_HOME)\watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)
	-copy $(LIBCURSES) panel.lib

PLATFORM1	= Watcom C++ Win32
PLATFORM2	= Open Watcom 1.6 for Win32
ARCNAME		= pdc$(VER)_wcc_w32

!include $(PDCURSES_HOME)\makedist.mif
