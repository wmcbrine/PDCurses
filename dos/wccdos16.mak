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

osdir		= $(PDCURSES_SRCDIR)/dos
# Open Watcom README strongly recommends setting WATCOM environment variable...
!ifeq CROSS_COMPILE Y
!ifdef %WATCOM
watcomdir	= $(%WATCOM)
!else
watcomdir	= "`which wcc | xargs realpath | xargs dirname`"/..
!endif
!endif

CC		= wcc
TARGET		= dos

CFLAGS		= -bt=$(TARGET) -zq -wx -m$(MODEL) -i=$(PDCURSES_SRCDIR)
# the README also recommends setting INCLUDE; if absent, we need an extra -i=
!ifndef %INCLUDE
CFLAGS		+= -i=$(watcomdir)/h
!endif

!ifeq DEBUG Y
CFLAGS  	+= -d2 -DPDCDEBUG
LDFLAGS 	= D W A op q sys $(TARGET)
!else
CFLAGS  	+= -oneatx
LDFLAGS		= op q sys $(TARGET)
!ifeq CROSS_COMPILE Y
LDFLAGS 	+= libp $(watcomdir)/lib286/dos\;$(watcomdir)/lib286
!endif
!endif

LIBEXE		= wlib -q -n -t

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
