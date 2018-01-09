# Makefile for PDCurses library for SDL

O = o

ifeq ($(OS),Windows_NT)
	E = .exe
	CC = gcc
	RM = cmd /c del

	SFLAGS = -Ic:/sdl2-2.0.7/include
	SLIBS = -Lc:/sdl2-2.0.7/lib/x64 -lSDL2main -lSDL2

	TFLAGS = -Ic:/sdl2_ttf-2.0.14/include
	TLIBS =-Lc:/sdl2_ttf-2.0.14/lib/x64 -lSDL2_ttf

	DEMOFLAGS = -mwindows
else
	RM = rm -f

	SFLAGS = $(shell sdl2-config --cflags)
	SLIBS = $(shell sdl2-config --libs)

	TLIBS = -lSDL2_ttf
endif

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR	= ..
endif

include $(PDCURSES_SRCDIR)/libobjs.mif

osdir		= $(PDCURSES_SRCDIR)/sdl2

PDCURSES_SDL_H	= $(osdir)/pdcsdl.h

# If your system doesn't have these, remove the defines here
SFLAGS		+= -DHAVE_VSNPRINTF -DHAVE_VSSCANF

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG -fPIC
else
	CFLAGS  += -O2 -Wall -fPIC
endif

ifeq ($(WIDE),Y)
	CFLAGS += -DPDC_WIDE $(TFLAGS)
	SLIBS  += $(TLIBS)
endif

ifeq ($(UTF8),Y)
	CFLAGS += -DPDC_FORCE_UTF8
endif

BUILD		= $(CC) $(CFLAGS) -I$(PDCURSES_SRCDIR)

LINK		= $(CC)
LDFLAGS		= $(LIBCURSES) $(SLIBS)
RANLIB		= ranlib
LIBCURSES	= libpdcurses.a

DEMOS		+= sdltest$(E)

.PHONY: all libs clean demos install

all:	libs demos

libs:	$(LIBCURSES)

clean:
	-$(RM) *.o trace $(LIBCURSES) $(DEMOS)

demos:	$(DEMOS)
ifneq ($(DEBUG),Y)
	strip $(DEMOS)
endif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	ar rv $@ $?
	-$(RANLIB) $@

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_SDL_H)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
tui.o tuidemo.o : $(PDCURSES_CURSES_H)
terminfo.o: $(TERM_HEADER)
panel.o ptest$(E): $(PANEL_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(BUILD) $(SFLAGS) -c $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(BUILD) $(SFLAGS) -c $<

firework$(E): $(demodir)/firework.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

ozdemo$(E): $(demodir)/ozdemo.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

ptest$(E): $(demodir)/ptest.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

rain$(E): $(demodir)/rain.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

testcurs$(E): $(demodir)/testcurs.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

tuidemo$(E): tuidemo.o tui.o
	$(LINK) tui.o tuidemo.o -o $@ $(LDFLAGS)

worm$(E): $(demodir)/worm.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

xmas$(E): $(demodir)/xmas.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

sdltest$(E): $(osdir)/sdltest.c
	$(BUILD) $(SFLAGS) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

tui.o: $(demodir)/tui.c $(demodir)/tui.h
	$(BUILD) -c $(DEMOFLAGS) $(demodir)/tui.c

tuidemo.o: $(demodir)/tuidemo.c
	$(BUILD) -c $(DEMOFLAGS) $(demodir)/tuidemo.c

ncurses_testdir = $(HOME)/ncurses-5.9/test
include $(demodir)/nctests.mif
