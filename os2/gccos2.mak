#########################################################################
#
# GNU MAKE Makefile for PDCurses library - OS/2 emx 0.9c+
#
# Usage: make -f [path\]gccos2.mak [DEBUG=Y] [EMXVIDEO=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|testcurs.exe...]
#
# The EMXVIDEO option compiles with the emx video library, which
# enables a PDCurses program to run under OS/2 and DOS.
#
#########################################################################

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

osdir		= $(PDCURSES_SRCDIR)/os2

PDCURSES_OS2_H	= $(osdir)/pdcos2.h

CC		= gcc

CFLAGS = -I$(PDCURSES_SRCDIR) -c -Wall

ifeq ($(EMXVIDEO),Y)
	CFLAGS += -DEMXVIDEO
	CCLIBS = -lvideo
	BINDFLAGS = -acm
else
	CCLIBS =
	BINDFLAGS = 
endif

ifeq ($(DEBUG),Y)
	CFLAGS  += -g -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  += -O2
	LDFLAGS =
endif

DLLTARGET	= pdcurses.dll
DLLFLAGS 	= -Zdll -Zcrtdll -Zomf

LINK		= gcc
EMXBIND		= emxbind 
EMXOMF		= emxomf

LIBEXE		= ar
LIBFLAGS	= rcv

ifeq ($(DLL),Y)
	CFLAGS += -Zdll -Zcrtdll -Zomf
	LDFLAGS += -Zlinker /PM:VIO -Zomf -Zcrtdll
	LIBCURSES = pdcurses.lib
	PDCLIBS = $(DLLTARGET)
	EXEPOST =
	TUIPOST =
	CLEAN = *.dll *.lib
else
	LIBCURSES = pdcurses.a
	PDCLIBS = $(LIBCURSES)
	EXEPOST = $(EMXBIND) $* $(BINDFLAGS)
	TUIPOST = $(EMXBIND) tuidemo $(BINDFLAGS)
	CLEAN = *.a testcurs newdemo xmas tuidemo firework ptest rain worm
endif

#########################################################################
.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(PDCLIBS)

clean:
	-del *.o
	-del *.exe
	-del $(CLEAN)

demos:	$(DEMOS)

#------------------------------------------------------------------------

DEMOOBJS = testcurs.o newdemo.o xmas.o tui.o tuidemo.o firework.o \
ptest.o rain.o worm.o

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $?
	-copy $(LIBCURSES) panel.a

$(DLLTARGET): $(LIBOBJS) $(PDCOBJS)
	$(LINK) $(DLLFLAGS) -o $(DLLTARGET) $? $(osdir)\pdcurses.def
#	lxlite $(DLLTARGET)
	emximp -o $(LIBCURSES) $(osdir)\pdcurses.def

$(LIBOBJS) $(PDCOBJS) $(DEMOOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_OS2_H)
$(DEMOS) : $(LIBCURSES)
panel.o ptest.o: $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) -o$@ $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) -o$@ $<

#------------------------------------------------------------------------

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe \
ptest.exe: %.exe: %.o
	$(LINK) $(LDFLAGS) -o $* $< $(LIBCURSES) $(CCLIBS)
	$(EXEPOST)

tuidemo.exe:	tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o tuidemo $? $(LIBCURSES) $(CCLIBS)
	$(TUIPOST)

firework.o newdemo.o ptest.o rain.o testcurs.o worm.o xmas.o: %.o: \
$(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $<

tui.o: $(demodir)\tui.c $(demodir)\tui.h
	$(CC) $(CFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)\tuidemo.c
	$(CC) $(CFLAGS) -I$(demodir) -o $@ $<

PLATFORM1 = EMX OS/2
PLATFORM2 = EMX 0.9d for OS/2
ARCNAME = pdc$(VER)_emx_os2

include $(PDCURSES_SRCDIR)/makedist.mif
