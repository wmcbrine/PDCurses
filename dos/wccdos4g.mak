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

!include $(PDCURSES_HOME)\version

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

!include $(PDCURSES_HOME)\wccinc

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @$(osdir)\wccdos.lrf

dist: .symbolic $(PDCLIBS)
	echo PDCurses $(VERDOT) for Watcom C++ 32bit DOS > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Open Watcom 1.3 for 32 bit DOS. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	echo $(PDCURSES_HOME)\README > flist
	echo $(PDCURSES_HOME)\HISTORY >> flist
	echo $(PDCURSES_HOME)\maintain.er >> flist
	echo $(PDCURSES_HOME)\curses.h >> flist
	echo $(PDCURSES_HOME)\curspriv.h >> flist
	echo $(PDCURSES_HOME)\panel.h >> flist
	echo $(PDCURSES_HOME)\term.h >> flist
	echo $(LIBCURSES) >> flist
	echo $(LIBPANEL) >> flist
	echo file_id.diz >> flist
	zip -9jX pdc$(VER)32w -@ <flist
	del flist
	del file_id.diz
