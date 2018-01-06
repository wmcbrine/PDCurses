# Watcom WMAKE Makefile for PDCurses library - Win32 Open Watcom 1.8+
#
# Usage: wmake -f [win32/]wccwin32.mak [DEBUG=Y] [WIDE=Y] [UTF8=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

RM = del

!ifdef %PDCURSES_SRCDIR
PDCURSES_SRCDIR	= $(%PDCURSES_SRCDIR)
!else
PDCURSES_SRCDIR	= ..
!endif

!include $(PDCURSES_SRCDIR)/version.mif

osdir		= $(PDCURSES_SRCDIR)/win32

CC		= wcc386
TARGET		= nt

CFLAGS		= -ei -zq -wx -i=$(PDCURSES_SRCDIR)

!ifeq DEBUG Y
CFLAGS		+= -d2 -DPDCDEBUG
LDFLAGS		= D W A op q sys $(TARGET)
!else
CFLAGS		+= -oneatx
LDFLAGS		= op q sys $(TARGET)
!endif

!ifeq WIDE Y
CFLAGS		+= -DPDC_WIDE
!endif

!ifeq UTF8 Y
CFLAGS		+= -DPDC_FORCE_UTF8
!endif

LIBEXE		= wlib -q -n -t

!include $(PDCURSES_SRCDIR)/watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)

PLATFORM1	= Watcom C++ Win32
PLATFORM2	= Open Watcom 2.0 for Win32
ARCNAME		= pdc$(VER)_wcc_w32

!include $(PDCURSES_SRCDIR)/makedist.mif
