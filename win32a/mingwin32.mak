# GNU MAKE Makefile for PDCurses library - WIN32 MinGW GCC
#
# Usage: make -f [path\]mingwin32.mak [DEBUG=Y] [DLL=Y] [WIDE=Y] [UTF8=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|testcurs.exe...]

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

uname_S := $(shell uname -s 2>/dev/null)

CAT = cat
PREFIX =
PATH_SEP = /
CP = cp
DELETE = rm -f

# It appears we have three cases:  we're running in Cygwin/MSYS;  or we're
# running in command.com on Windows with MinGW;  or we're on Linux or BSD
# or similar system,  cross-compiling with MinGW.

ifneq (,$(findstring CYGWIN,$(uname_S)))
	#  Insert Cygwin-specific changes here
	ON_WINDOWS = 1
endif
ifneq (,$(findstring MINGW32_NT,$(uname_S)))
	#  Insert MINGW32-specific changes here
	ON_WINDOWS = 1
endif
ifneq (,$(findstring MINGW64_NT,$(uname_S)))
	#  Insert MINGW64-specific changes here
	ON_WINDOWS = 1
endif

ifeq ($(uname_S),)
	CAT = type
	PATH_SEP = \\
	CP = -copy
	DELETE = -del
	ON_WINDOWS = 1
endif

# If we aren't on Windows,  assume MinGW on a Linux-like host
# Only decision is:  are we doing a 64-bit compile (_w64 defined)?

ifndef ON_WINDOWS
	ifdef _w64
	   PREFIX  = x86_64-w64-mingw32-
	else
	   PREFIX  = i686-w64-mingw32-
	endif
endif

osdir	   = $(PDCURSES_SRCDIR)/win32a

PDCURSES_WIN_H	= $(osdir)/pdcwin.h

CC	= $(PREFIX)gcc

AR	= $(PREFIX)ar
STRIP	= $(PREFIX)strip

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O4 -Wall -pedantic
	LDFLAGS =
endif

CFLAGS += -I$(PDCURSES_SRCDIR)

ifdef CHTYPE_32
	CFLAGS += -DCHTYPE_32
endif

ifdef CHTYPE_16
	CFLAGS += -DCHTYPE_16
endif

ifeq ($(WIDE),Y)
	CFLAGS += -DPDC_WIDE
endif

ifeq ($(UTF8),Y)
	CFLAGS += -DPDC_FORCE_UTF8
endif

LINK	   = $(CC)

ifeq ($(DLL),Y)
	CFLAGS += -DPDC_DLL_BUILD
	LIBEXE = $(CC)
	LIBFLAGS = -Wl,--out-implib,pdcurses.a -shared -o
	LIBCURSES = pdcurses.dll
	LIBDEPS = $(LIBOBJS) $(PDCOBJS)
	LIBSADDED = -lgdi32 -lcomdlg32
	EXELIBS =
	CLEAN = $(LIBCURSES) *.a
else
	LIBEXE = $(AR)
ifeq ($(PREFIX),)
	LIBFLAGS = rcv
else
	LIBFLAGS = rv
endif
	LIBCURSES = pdcurses.a
	LIBDEPS = $(LIBOBJS) $(PDCOBJS)
	LIBSADDED =
	EXELIBS = -lgdi32 -lcomdlg32
	CLEAN = *.a
endif

.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(LIBCURSES)

clean:
	$(DELETE) *.o
	$(DELETE) *.exe
	$(DELETE) *.dll
	$(DELETE) $(CLEAN)

demos:	$(DEMOS)
ifneq ($(DEBUG),Y)
	$(STRIP) *.exe
endif

$(LIBCURSES) : $(LIBDEPS)
	$(LIBEXE) $(LIBFLAGS) $@ $? $(LIBSADDED)
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
version.exe worm.exe xmas.exe: %.exe: $(demodir)/%.c
	$(CC) $(CFLAGS) -mwindows -o$@ $< $(LIBCURSES) $(EXELIBS)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -mwindows -o$@ tuidemo.o tui.o $(LIBCURSES) $(EXELIBS)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

PLATFORM1 = MinGW Win32
PLATFORM2 = MinGW for Win32
ARCNAME = pdc$(VER)_ming_w32

include $(PDCURSES_SRCDIR)/makedist.mif
