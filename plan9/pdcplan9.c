#define _LOCK_EXTENSION
#define _QLOCK_EXTENSION
#define _PLAN9_SOURCE
#define _RESEARCH_SOURCE
#include <sys/types.h>
#include <lock.h>
#include <qlock.h>
#include <lib9.h>
#include <stdlib.h>
#include <string.h>
#include <bsd.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <utf.h>
#include <fmt.h>
#include <signal.h>
#include <select.h>
#include <sys/time.h>

#define border curborder
#include <curspriv.h>
#undef border
#include <draw.h>
#include <event.h>
#include <keyboard.h>
#include "pdcplan9.h"

enum {
	Ncolors = 18,
	Eresized = 3,
};

uint rgbacolors[Ncolors] = {
	0x000000FF,		/* black */
	0x0000FFFF,		/* blue */
	0x00AA00FF,		/* green */
	0x00AAAAFF,		/* cyan */
	0xAA0000FF,		/* red */
	0xAA00AAFF,		/* purple */
	0xFF5500FF,		/* brown */
	0x7F7F7FFF,		/* white */
	0x555555FF,		/* light black aka grey */
	0x5555FFFF,		/* light blue */
	0x55FF55FF,		/* light green */
	0x55FFFFFF,		/* light cyan */
	0xFF5555FF,		/* light red */
	0xFF55FFFF,		/* light purple */
	0xFFFF55FF,		/* light brown aka yellow */
	0xFFFFFFFF,		/* light grey aka white */
	0x9EEEEFF,
	0x9EE9EFF,
};

static Image *colors[Ncolors];

static void fatal(char *s)
{
	fprint(2, "curses: %s: %r\n", s);
	abort();
}


void p9napms(int ms)
{
	sleep(ms);
}


void p9setlabel(char *s)
{
	int fd;

	fd = open("/dev/label", 1);
	if (fd >= 0)
	{
		fprint(fd, "%s", s);
		close(fd);
	}
}


void p9resize(int nl, int nc)
{
	int fd;

	fd = open("/dev/wctl", 1);
	if (fd >= 0)
	{
		fprint(fd, "resize -dx %d -dy %d",
		       nc * stringwidth(font, "0") + 8, nl * font->height + 8);
		close(fd);
	}
}


int p9getrows(void)
{
	return Dy(screen->r) / font->height;
}


int p9getcols(void)
{
	return Dx(screen->r) / stringwidth(font, "0");
}


#define ms pdc_mouse_status	/* I don't like this var name */
static void setms(Mouse m)
{
	static uint clickmsec;
	MOUSE_STATUS oldms;
	int but, b, x, y;

	oldms = ms;
	memset(&ms, 0, sizeof(ms));
	ms.x = (m.xy.x - screen->r.min.x) / stringwidth(font, "0");
	ms.y = (m.xy.y - screen->r.min.y) / font->height;
	if (ms.x != oldms.x || ms.y != oldms.y)
		ms.changes |= PDC_MOUSE_MOVED;

	/*scroll wheel */
	if (m.buttons & (8 | 16))
	{
		if (m.buttons & 8)
			ms.changes |= PDC_MOUSE_WHEEL_UP;
		else
			ms.changes |= PDC_MOUSE_WHEEL_DOWN;
		SP->key_code = TRUE;
		return;
	}
	but = 0;
	if (m.buttons & 1)
		but = 1;
	else if (m.buttons & 2)
		but = 2;
	else if (m.buttons & 4)
		but = 3;

	if (but == 0)
		return;

	b = m.buttons;
	if (m.msec - clickmsec < 500)
	{
		ms.button[but - 1] = BUTTON_DOUBLE_CLICKED;
		x = m.xy.x;
		y = m.xy.y;
		/* stay here until something interesting happens */
		do
			m = emouse();
		while (m.buttons == b && abs(m.xy.x - x) < 3
		       && abs(m.xy.y - y) < 3);
	}
	if (m.buttons == b)
	{
		uint msec = m.msec;
		do
			m = emouse();
		while (m.buttons == b);
		if (m.msec - msec < 250)
			ms.button[but - 1] = BUTTON_CLICKED;
		else
			ms.button[but - 1] = BUTTON_PRESSED;
	}
	if (oldms.x == ms.x && oldms.y == ms.y)
	{
		if (m.msec - clickmsec < 500)
			ms.button[but - 1] = BUTTON_DOUBLE_CLICKED;
		else
			clickmsec = m.msec;
	}
	/* choording should be here */
	while (m.buttons == b)
		m = emouse();

	ms.changes |= 1 << (but - 1);
	SP->key_code = FALSE;
}
#undef ms


static int transk(Rune k)
{
	switch (k)
	{
	case Khome:
		return KEY_HOME;
	case Kup:
		return KEY_UP;
	case Kpgup:
		return KEY_PPAGE;
	case Kprint:
		return KEY_PRINT;
	case Kleft:
		return KEY_LEFT;
	case Kright:
		return KEY_RIGHT;
	case Kdown:
		return KEY_DOWN;
	case Kpgdown:
		return KEY_NPAGE;
	case Kins:
		return KEY_IC;
	case Kend:
		return KEY_END;
	default:
		return k;
	}
}

static int resized;

void eresized(int)
{
	if (getwindow(display, Refnone) < 0)
		fatal("can't reattach to window");

	draw(screen, screen->r, display->black, nil, ZP);
	resized = TRUE;
}


int p9ecan(void)
{
	return ecanread(Ekeyboard | Emouse);
}


int p9eget(void)
{
	char c;
	Event e;
	int x = 0;

	if (resized)
	{
		resized = FALSE;
		SP->resized = TRUE;
		x = KEY_RESIZE;
	}
	switch (event(&e))
	{
	case Emouse:
		setms(e.mouse);
		x = KEY_MOUSE;
	case Ekeyboard:
		if (e.kbdc == Kdel)
			raise(SIGINT);
		x = transk(e.kbdc);
	};
	return x;
}

void p9init(void)
{
	int i;

	if (initdraw(0, "/lib/font/bit/fixed/unicode.9x15.font", "curses") < 0)
		fatal("can't open display");

	draw(screen, screen->r, display->black, nil, ZP);
	einit(Ekeyboard | Emouse);
	for (i = 0; i < Ncolors; i++)
	{
		colors[i] =
		    allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1,
			       rgbacolors[i]);
		if (colors[i] == nil)
			fatal("can't allocate image");
	}
}

static Point getcur(int y, int x)
{
	Point p;

	p.x = screen->r.min.x + stringwidth(font, "0") * x;
	p.y = screen->r.min.y + font->height * y;

	return p;
}


void p9putc(int y, int x, chtype ch)
{
	chtype attr;
	int c, t;
	short fg, bg;
	Point p0, p1;
	Rune buf[1];

	attr = ch & A_ATTRIBUTES;
	if (pair_content(PAIR_NUMBER(attr), &fg, &bg) == ERR)
	{
		fg = COLOR_WHITE;
		bg = COLOR_BLACK;
	}

	fg |= attr & A_BOLD ? 8 : 0;
	bg |= attr & A_BLINK ? 8 : 0;
	if (attr & A_REVERSE)
	{
		t = fg;
		fg = bg;
		bg = t;
	}
	if (attr & A_ALTCHARSET && !(ch & 0xff80))
		ch = acs_map[ch & 0x7f];

	c = ch & A_CHARTEXT;

	buf[0] = c;
	p0 = getcur(y, x);
	runestringnbg(screen, p0, colors[fg], ZP, font, buf, 1, colors[bg], ZP);
	if (attr & A_UNDERLINE)
	{
		p1 = p0;
		p1.x += runestringnwidth(font, buf, 1);
		line(screen, p0, p1, Enddisc, Enddisc, 0, colors[fg], ZP);
	}
}


void p9setcur(int oy, int ox, int y, int x, int vis)
{
	Image *c;
	chtype *ch;
	Point p;

	ch = curscr->_y[oy] + ox;
	p9putc(oy, ox, *ch);
	if (vis == 0)
		return;

	if (vis == 1)
		c = colors[Ncolors - 2];
	else
		c = colors[Ncolors - 1];

	p = getcur(y, x);
	draw(screen,
	     Rect(p.x, p.y + font->height - 2, p.x + stringwidth(font, "0"),
		  p.y + font->height), c, nil, ZP);

	flushimage(display, 1);
}
