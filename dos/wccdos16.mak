# Watcom WMAKE Makefile for PDCurses library - DOS (16 bit) Watcom C/C++ 10.6+
#
# Usage: wmake -f [path\]wccdos16.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

# Change the memory MODEL here, if desired
MODEL		= l

!ifdef %PDCURSES_SRCDIR
PDCURSES_SRCDIR	= $(%PDCURSES_SRCDIR)
!else
PDCURSES_SRCDIR	= ..
!endif

!include $(PDCURSES_SRCDIR)/version.mif

!ifndef CROSS_COMPILE
!ifeq %SHELL /bin/bash
# assume we are cross-compiling
CROSS_COMPILE	= Y
!endif
!ifeq %SHELL /bin/sh
CROSS_COMPILE	= Y
!endif
!ifeq %SHELL /bin/csh
CROSS_COMPILE	= Y
!endif
!ifeq %SHELL /bin/dash
CROSS_COMPILE	= Y
!endif
!endif

!ifeq CROSS_COMPILE Y
osdir		= $(PDCURSES_SRCDIR)/dos
# OpenWatcom README strongly recommends setting WATCOM environment variable...
!ifdef %WATCOM
watcomdir	= $(%WATCOM)
!else
watcomdir	= "`which wcc | xargs realpath | xargs dirname`"/..
!endif
!else
osdir		= $(PDCURSES_SRCDIR)\dos
!endif

CC		= wcc
TARGET		= dos

!ifeq CROSS_COMPILE Y
CFLAGS		= -bt=$(TARGET) -zq -wx -m$(MODEL) -i=$(PDCURSES_SRCDIR)
CFLAGS		+= -i=$(watcomdir)/h
!else
CFLAGS		= /bt=$(TARGET) /zq /wx /m$(MODEL) /i=$(PDCURSES_SRCDIR)
!endif

!ifeq DEBUG Y
!ifeq CROSS_COMPILE Y
CFLAGS  	+= -d2 -DPDCDEBUG
!else
CFLAGS  	+= /d2 /DPDCDEBUG
!endif
LDFLAGS 	= D W A op q sys $(TARGET)
!else
!ifeq CROSS_COMPILE Y
CFLAGS  	+= -oneatx
LDFLAGS		= op q sys $(TARGET)
LDFLAGS 	+= libp $(watcomdir)/lib286/dos\;$(watcomdir)/lib286
!else
CFLAGS  	+= /oneatx
LDFLAGS 	= op q sys $(TARGET)
!endif
!endif

!ifeq CROSS_COMPILE Y
LIBEXE		= wlib -q -n -t
!else
LIBEXE		= wlib /q /n /t
!endif

!include $(PDCURSES_SRCDIR)/watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	%write wccdos.lrf $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @wccdos.lrf
!ifeq CROSS_COMPILE Y
	rm wccdos.lrf
	cp $(LIBCURSES) panel.lib
!else
	-del wccdos.lrf
	-copy $(LIBCURSES) panel.lib
!endif

PLATFORM1	= Watcom C++ 16-bit DOS
PLATFORM2	= Open Watcom 1.6 for 16-bit DOS
ARCNAME		= pdc$(VER)16w

!include $(PDCURSES_SRCDIR)/makedist.mif
