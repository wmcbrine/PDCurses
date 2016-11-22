# GNU MAKE Makefile for PDCurses library - WIN32 MinGW GCC
#
# Usage: make -f [path\]mingwin32.mak [DEBUG=Y] [DLL=Y] [WIDE=Y] [UTF8=Y] [tgt]
#
# where tgt can be any of:
# [all|demos|pdcurses.a|testcurs.exe...]

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

# For cross-compiling from Linux,  use _linux_w64 to compile 64-bit Windows
# code,  or _linux_w32 to compile 32-bit Windows code.

ifdef _linux_w64
	PREFIX  = x86_64-w64-mingw32-
	CAT = cat
endif

ifdef _linux_w32
	PREFIX  = i686-w64-mingw32-
	CAT = cat
endif

# For either of the above cross-compiles,  we use 'cat' and 'cp;  in Windows,
# the commands should be 'type' and 'copy',  and the slash goes backwards.

ifeq ($(CAT),cat)
	CP =  cp
	BASEDEF      = $(PDCURSES_SRCDIR)/exp-base.def
	WIDEDEF      = $(PDCURSES_SRCDIR)/exp-wide.def
	DELETE   = -rm
else
	CAT = type
	CP  = copy
	BASEDEF      = $(PDCURSES_SRCDIR)\exp-base.def
	WIDEDEF      = $(PDCURSES_SRCDIR)\exp-wide.def
	DELETE = -del
endif

osdir      = $(PDCURSES_SRCDIR)/win32

PDCURSES_WIN_H   = $(osdir)/pdcwin.h

CC      = $(PREFIX)gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O4 -Wall
	LDFLAGS =
endif

CFLAGS += -I$(PDCURSES_SRCDIR)

ifdef CHTYPE_32
	CFLAGS += -DCHTYPE_32
endif

ifdef CHTYPE_16
	CFLAGS += -DCHTYPE_16
endif

DEFDEPS      = $(BASEDEF)

ifeq ($(WIDE),Y)
	CFLAGS += -DPDC_WIDE
	DEFDEPS += $(WIDEDEF)
	DEFFILE      = pdcursew.def
else
	DEFFILE      = pdcurses.def
endif

ifeq ($(UTF8),Y)
	CFLAGS += -DPDC_FORCE_UTF8
endif

LINK      = $(PREFIX)gcc

ifeq ($(DLL),Y)
	CFLAGS += -DPDC_DLL_BUILD
	LIBEXE = $(PREFIX)gcc $(DEFFILE)
	LIBFLAGS = -Wl,--out-implib,pdcurses.a -shared -o
	LIBCURSES = pdcurses.dll
	LIBDEPS = $(LIBOBJS) $(PDCOBJS) $(DEFFILE)
	CLEAN = $(LIBCURSES) *.a $(DEFFILE)
else
	LIBEXE = $(PREFIX)ar
ifeq ($(PREFIX),)
	LIBFLAGS = rcv
else
	LIBFLAGS   = rv
endif
	LIBCURSES = pdcurses.a
	LIBDEPS = $(LIBOBJS) $(PDCOBJS)
	CLEAN = *.a
endif

.PHONY: all libs clean demos dist

all:   libs demos

libs:   $(LIBCURSES)

clean:
	$(DELETE) *.o
	$(DELETE) *.exe
	$(DELETE) *.dll
	$(DELETE) *.def
	$(DELETE) $(CLEAN)

demos:   $(DEMOS)
	strip *.exe

$(DEFFILE): $(DEFDEPS)
	echo LIBRARY pdcurses > $@
	echo EXPORTS >> $@
	$(CAT) $(BASEDEF) >> $@
ifeq ($(WIDE),Y)
	$(CAT) $(WIDEDEF) >> $@
endif

$(LIBCURSES) : $(LIBDEPS)
	$(LIBEXE) $(LIBFLAGS) $@ $?
	$(CP) pdcurses.a panel.a

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_WIN_H)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
panel.o : $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) $<

firework.exe newdemo.exe newtest.exe ptest.exe rain.exe testcurs.exe  \
worm.exe xmas.exe: %.exe: $(demodir)/%.c
	$(CC) $(CFLAGS) -mwindows -o$@ $< $(LIBCURSES)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

PLATFORM1 = MinGW Win32
PLATFORM2 = MinGW for Win32
ARCNAME = pdc$(VER)_ming_w32

include $(PDCURSES_SRCDIR)/makedist.mif
