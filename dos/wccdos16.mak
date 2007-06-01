################################################################################
#
# Watcom WMAKE Makefile for PDCurses library - DOS (16 bit) Watcom C/C++ 10.6+
#
# Usage: wmake -f [path\]wccdos16.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, and/or edit the 
# lines below; for example, "set PDCURSES_SRCDIR=c:\pdcurses".
#
################################################################################
PDCURSES_HOME	= $(%PDCURSES_SRCDIR)
MODEL		= l
################################################################################
# Nothing below here should require changing.
################################################################################

!include $(PDCURSES_HOME)\version.mif

osdir		= $(PDCURSES_HOME)\dos

CC		= wcc
TARGET		= dos

CFLAGS		= /bt=$(TARGET) /zq /wx /m$(MODEL) /i=$(PDCURSES_HOME)

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

PLATFORM1	= Watcom C++ 16-bit DOS
PLATFORM2	= Open Watcom 1.6 for 16-bit DOS
ARCNAME		= pdc$(VER)16w

!include $(PDCURSES_HOME)\makedist.mif
