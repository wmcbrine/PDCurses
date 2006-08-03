################################################################################
#
# GNU MAKE Makefile for PDCurses library - WIN32 Cygnus GCC
#
# Usage: make -f [path\]gccwin32.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, or edit the line
# below; for example, "export PDCURSES_SRCDIR=/cygdrive/c/pdcurses". 
#
################################################################################
PDCURSES_HOME	= $(PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################

include $(PDCURSES_HOME)/version

PDCURSES_CURSES_H	= $(PDCURSES_HOME)/curses.h
PDCURSES_CURSPRIV_H	= $(PDCURSES_HOME)/curspriv.h
PDCURSES_HEADERS	= $(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		= $(PDCURSES_HOME)/panel.h
TERM_HEADER		= $(PDCURSES_HOME)/term.h

srcdir		= $(PDCURSES_HOME)/pdcurses
osdir		= $(PDCURSES_HOME)/win32
pandir		= $(PDCURSES_HOME)/panel
demodir		= $(PDCURSES_HOME)/demos

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CPPFLAGS	= -I$(PDCURSES_HOME) #-DPDC_WIDE

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)
DEMOS		= testcurs.exe newdemo.exe xmas.exe tuidemo.exe \
firework.exe ptest.exe rain.exe worm.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-rm -f *.o
	-rm -f *.a
	-rm -f *.exe

demos:	$(DEMOS)
	strip *.exe

#------------------------------------------------------------------------

LIBOBJS = addch.o addchstr.o addstr.o attr.o beep.o bkgd.o border.o \
clear.o color.o delch.o deleteln.o getch.o getstr.o getyx.o inch.o \
inchstr.o initscr.o inopts.o insch.o insstr.o instr.o kernel.o mouse.o \
move.o outopts.o overlay.o pad.o printw.o refresh.o scanw.o scr_dump.o \
scroll.o slk.o termattr.o terminfo.o touch.o util.o window.o pdcdebug.o \
pdcwin.o

PDCOBJS = pdcclip.o pdcdisp.o pdcgetsc.o pdckbd.o pdcscrn.o pdcsetsc.o \
pdcutil.o

PANOBJS = panel.o

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PANOBJS) : $(PANEL_HEADER)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CCFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CCFLAGS) $<

$(PANOBJS) : %.o: $(pandir)/%.c
	$(CC) -c $(CCFLAGS) $<

#------------------------------------------------------------------------

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe: \
%.exe: $(demodir)/%.c
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

ptest.exe: $(demodir)/ptest.c $(PANEL_HEADER) $(LIBPANEL)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES) $(LIBPANEL)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o tuidemo.exe tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o $@ $<

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
