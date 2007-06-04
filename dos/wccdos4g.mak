#########################################################################
#
# Watcom WMAKE Makefile for PDCurses library - DOS/4GW Watcom C/C++ 10.6+
#
# Usage: wmake -f [path\]wccdos4g.mak [DEBUG=Y] [target]
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

osdir		= $(PDCURSES_HOME)\dos

CC		= wcc386
TARGET		= dos4g

CFLAGS		= /bt=$(TARGET) /zq /wx /mf /i=$(PDCURSES_HOME)

!ifeq DEBUG Y
CFLAGS  	+= /d2 /DPDCDEBUG
LDFLAGS 	= D W A op q sys $(TARGET)
!else
CFLAGS  	+= /oneatx
LDFLAGS 	= op q sys $(TARGET)
!endif

LIBEXE		= wlib /q /n /t

!include $(PDCURSES_HOME)\watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	%write wccdos.lrf $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @wccdos.lrf
	-del wccdos.lrf
	-copy $(LIBCURSES) panel.lib

PLATFORM1	= Watcom C++ 32-bit DOS
PLATFORM2	= Open Watcom 1.6 for 32-bit DOS
ARCNAME		= pdc$(VER)32w

!include $(PDCURSES_HOME)\makedist.mif
