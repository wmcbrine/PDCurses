################################################################################
#
# GNU MAKE Makefile for PDCurses library - WIN32 Cygnus GCC
#
# Usage: make -f [path\]gccwin32.mak [DEBUG=Y] [WIDE=Y] [UTF8=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, or edit the line
# below; for example, "export PDCURSES_SRCDIR=c:/pdcurses". 
#
################################################################################
PDCURSES_HOME	= $(PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################

O = o

include $(PDCURSES_HOME)/version.mif
include $(PDCURSES_HOME)/libobjs.mif

osdir		= $(PDCURSES_HOME)/win32

PDCURSES_WIN_H	= $(osdir)/pdcwin.h

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CFLAGS += -I$(PDCURSES_HOME)

ifeq ($(WIDE),Y)
	CFLAGS += -DPDC_WIDE
endif

ifeq ($(UTF8),Y)
	CFLAGS += -DPDC_FORCE_UTF8
endif

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

################################################################################
.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(PDCLIBS)

clean:
	-rm -f *.o
	-rm -f *.a
	-rm -f *.exe

demos:	$(DEMOS)
	strip *.exe

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_WIN_H)
$(PANOBJS) : $(PANEL_HEADER)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)

terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) $<

$(PANOBJS) : %.o: $(pandir)/%.c
	$(CC) -c $(CFLAGS) $<

#------------------------------------------------------------------------

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe: \
%.exe: $(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $< $(LIBCURSES)

ptest.exe: $(demodir)/ptest.c $(PANEL_HEADER) $(LIBPANEL)
	$(CC) $(CFLAGS) -o$@ $< $(LIBCURSES) $(LIBPANEL)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

#------------------------------------------------------------------------

dist: $(PDCLIBS)
	echo PDCurses $(VERDOT) for Cygnus Win32 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Cygnus GCC for Win32. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_cyg_w32 \
	$(PDCURSES_HOME)/README $(PDCURSES_HOME)/HISTORY \
	$(PDCURSES_HOME)/maintain.er \
	$(PDCURSES_HOME)/curses.h $(PDCURSES_HOME)/curspriv.h \
	$(PDCURSES_HOME)/panel.h $(PDCURSES_HOME)/term.h \
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	rm file_id.diz
