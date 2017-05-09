# GNU MAKE Makefile for PDCurses demos with ncurses
#
# Usage: make [tgt] [WIDE=Y]
#
# where tgt can be any of:
# [all|demos|testcurs]...

ifeq ($(WIDE),Y)
 CFLAGS = -Wall -O3 -D_XOPEN_SOURCE_EXTENDED -DHAVE_NCURSESW
else
 CFLAGS = -Wall -O3 -D_XOPEN_SOURCE_EXTENDED
endif

LIBCURSES = -lncursesw
demodir = ../demos

DEMOS    = firework newdemo newtest rain testcurs worm xmas

all: $(DEMOS)

$(DEMOS) :  %: $(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $< $(LIBCURSES)

clean:
	rm -f $(DEMOS)
