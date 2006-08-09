################################################################################
#
# Watcom WMAKE Makefile for PDCurses library - DOS/4GW Watcom C/C++ 10.6+
#
# Usage: wmake -f [path\]wccdos4g.mak [DEBUG=Y] [target]
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

!include $(PDCURSES_HOME)\version.mif

osdir		= $(PDCURSES_HOME)\dos

CC		= wcc386
TARGET		= dos4g

!ifeq DEBUG Y
CFLAGS  	= /d2 /DPDCDEBUG
LDFLAGS 	= D W A op q sys $(TARGET)
!else
CFLAGS  	= /oneatx
LDFLAGS 	= op q sys $(TARGET)
!endif

CPPFLAGS	= /i=$(PDCURSES_HOME)
CCFLAGS		= /bt=$(TARGET) /zq /wx /mf $(CFLAGS) $(CPPFLAGS)

LIBEXE		= wlib /q /n /t

!include $(PDCURSES_HOME)\watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @$(osdir)\wccdos.lrf

PLATFORM1	= Watcom C++ 32-bit DOS
PLATFORM2	= Open Watcom 1.3 for 32-bit DOS
ARCNAME		= pdc$(VER)32w

!include $(PDCURSES_HOME)\makedist.mif
