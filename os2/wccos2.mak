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
!include $(PDCURSES_HOME)\wccobjs

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

CPPFLAGS 	= /i=$(PDCURSES_HOME)

CCFLAGS 	= /bt=$(TARGET) /wx /s /zq $(CFLAGS) $(CPPFLAGS)

LINK		= wlink

LIBEXE		= wlib /q /n /b /c /t

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean
	-del *.obj
	-del *.lib
	-del *.exe
	-del *.err

demos:	$(DEMOS)

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $@ $(PANOBJS)

.c: $(srcdir);$(osdir);$(pandir);$(demodir)
.c.obj: .autodepend
	$(CC) $(CCFLAGS) $<

#------------------------------------------------------------------------

!include $(PDCURSES_HOME)\wccdems

dist: .symbolic $(PDCLIBS)
	echo PDCurses $(VERDOT) for Watcom C++ OS/2 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Open Watcom 1.3 for OS/2. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_wcc_os2 &
	$(PDCURSES_HOME)\README $(PDCURSES_HOME)\HISTORY &
	$(PDCURSES_HOME)\maintain.er &
	$(PDCURSES_HOME)\curses.h $(PDCURSES_HOME)\curspriv.h &
	$(PDCURSES_HOME)\panel.h $(PDCURSES_HOME)\term.h &
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	del file_id.diz
