################################################################################
#
# GNU MAKE (3.79.1) Makefile for PDCurses library - DOS DJGPP V2.0+
#
# Usage: make -f [path\]gccdos.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|libs|demos|dist|pdcurses.a|panel.a|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, or edit the line 
# below; for example, "set PDCURSES_SRCDIR=c:\pdcurses".
#
################################################################################
PDCURSES_HOME	= $(PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################

O = o

include $(PDCURSES_HOME)/version.mif
include $(PDCURSES_HOME)/libobjs.mif

osdir		= $(PDCURSES_HOME)/dos

PDCURSES_DOS_H	= $(osdir)/pdcdos.h

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CPPFLAGS	= -I$(PDCURSES_HOME) -D_NAIVE_DOS_REGS -funsigned-char

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

################################################################################
.PHONY: all libs clean demos dist

all:	$(PDCLIBS) $(DEMOS)

libs:	$(PDCLIBS)

clean:
	-del *.o
	-del $(LIBCURSES)
	-del $(LIBPANEL)
	-del *.exe

demos:	$(DEMOS)

#------------------------------------------------------------------------

PDCOBJS = pdcclip.o pdcdisp.o pdcgetsc.o pdckbd.o pdcscrn.o \
pdcsetsc.o pdcutil.o pdcgo32.o

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_DOS_H)
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
	strip $@

ptest.exe: $(demodir)/ptest.c $(PANEL_HEADER) $(LIBPANEL)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES) $(LIBPANEL)
	strip $@

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)
	strip $@

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o$@ $<
	$(CC) -c $(CCFLAGS) -I$(demodir) -o$@ $<
	$(CC) -c $(CCFLAGS) -I$(demodir) -o$@ $<

#------------------------------------------------------------------------

PLATFORM1 = DJGPP 2.03
PLATFORM2 = DJGPP 2.03 for DOS
ARCNAME = pdc$(VER)djg

include $(PDCURSES_HOME)/makedist.mif
