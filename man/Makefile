# Makefile for PDCurses manext program.

all: manual

manual: PDCurses.md

PDCurses.md: manext intro.md x11.md sdl.md ../curses.h
	cat intro.md > PDCurses.md
	echo PDCurses Definitions and Variables >> PDCurses.md
	echo ================================== >> PDCurses.md
	./manext ../curses.h >> PDCurses.md
	echo PDCurses Functions >> PDCurses.md
	echo ================== >> PDCurses.md
	./manext ../pdcurses/*.c >> PDCurses.md
	./manext ../x11/*.c >> PDCurses.md
	cat x11.md >> PDCurses.md
	echo >> PDCurses.md
	echo >> PDCurses.md
	echo \
-------------------------------------------------------------------------- \
>> PDCurses.md
	echo >> PDCurses.md
	cat sdl.md >> PDCurses.md

manext: manext.c

install:
	echo Does nothing at the moment

clean:
	-rm -rf *.o manext PDCurses.md

distclean: clean

mostlyclean: clean

realclean: distclean
