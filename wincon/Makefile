# GNU MAKE Makefile for PDCurses library - Windows console GCC
#
# Usage: make [-f path\Makefile] [DEBUG=Y] [DLL=Y] [WIDE=Y] [UTF8=Y] [tgt]
#
# where tgt can be any of:
# [all|demos|pdcurses.a|testcurs.exe...]

O = o
E = .exe

ifeq ($(OS),Windows_NT)
	RM = cmd /c del
else
	RM = rm -f
endif

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

osdir		= $(PDCURSES_SRCDIR)/wincon

PDCURSES_WIN_H	= $(osdir)/pdcwin.h

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CFLAGS += -I$(PDCURSES_SRCDIR)

ifeq ($(WIDE),Y)
	CFLAGS += -DPDC_WIDE
endif

ifeq ($(UTF8),Y)
	CFLAGS += -DPDC_FORCE_UTF8
endif

LINK		= gcc

ifeq ($(DLL),Y)
	CFLAGS += -DPDC_DLL_BUILD
	LIBEXE = gcc
	LIBFLAGS = -Wl,--out-implib,pdcurses.a -shared -o
	LIBCURSES = pdcurses.dll
	LIBDEPS = $(LIBOBJS) $(PDCOBJS)
	CLEAN = $(LIBCURSES) *.a
else
	LIBEXE = ar
	LIBFLAGS = rcv
	LIBCURSES = pdcurses.a
	LIBDEPS = $(LIBOBJS) $(PDCOBJS)
	CLEAN = *.a
endif

.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(LIBCURSES)

clean:
	-$(RM) *.o
	-$(RM) *.exe
	-$(RM) $(CLEAN)

demos:	$(DEMOS)
ifneq ($(DEBUG),Y)
	strip *.exe
endif

$(LIBCURSES) : $(LIBDEPS)
	$(LIBEXE) $(LIBFLAGS) $@ $?

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_WIN_H)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
panel.o : $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) $<

firework.exe ozdemo.exe rain.exe testcurs.exe worm.exe xmas.exe \
ptest.exe: %.exe: $(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $< $(LIBCURSES)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

PLATFORM1 = GCC Wincon
PLATFORM2 = GCC for Windows console
ARCNAME = pdc$(VER)_gcc_wincon

include $(PDCURSES_SRCDIR)/makedist.mif
