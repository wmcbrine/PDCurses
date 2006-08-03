################################################################################
#
# Watcom WMAKE Makefile for PDCurses library - OS/2 Open Watcom 1.1+
#
# Usage: wmake -f [path\]wccos2.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|panel.lib|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, and/or edit the
# lines below; for example, "set PDCURSES_SRCDIR=c:\pdcurses".
#
################################################################################
PDCURSES_HOME	= $(%PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################

!include $(PDCURSES_HOME)\version

osdir		= $(PDCURSES_HOME)\os2

CC		= wcc386
TARGET		= os2v2

!ifeq DEBUG Y
CFLAGS  	= /d2 /DPDCDEBUG
LDFLAGS 	= D A op q sys $(TARGET)
!else
CFLAGS  	= /oneatx
LDFLAGS 	= op q sys $(TARGET)
!endif

CPPFLAGS	= /i=$(PDCURSES_HOME)
CCFLAGS 	= /bt=$(TARGET) /wx /s /zq $(CFLAGS) $(CPPFLAGS)

LIBEXE		= wlib /q /n /b /c /t

PLATFORM	= OS/2
ARCNAME		= pdc$(VER)_wcc_os2

!include $(PDCURSES_HOME)\wccinc

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)
