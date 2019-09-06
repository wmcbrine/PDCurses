/* Public Domain Curses */

#include <limits.h>
#include "pdcdos.h"

void PDC_beep(void)
{
    PDCREGS regs;

    PDC_LOG(("PDC_beep() - called\n"));

    regs.W.ax = 0x0e07;       /* Write ^G in TTY fashion */
    regs.W.bx = 0;
    PDCINT(0x10, regs);
}

#if UINT_MAX >= 0xfffffffful
# define irq0_ticks()  (getdosmemdword(0x46c))
/* For 16-bit platforms, we expect that the program will need _two_ memory
   read instructions to read the tick count.  Between the two instructions,
   if we do not turn off interrupts, an IRQ 0 might intervene and update the
   tick count with a carry over from the lower half to the upper half ---
   and our read count will be bogus.  */
#elif defined __TURBOC__
static unsigned long irq0_ticks(void)
{
    unsigned long t;
    disable();
    t = getdosmemdword(0x46c);
    enable();
    return t;
}
#elif defined __WATCOMC__
static unsigned long irq0_ticks(void)
{
    unsigned long t;
    _disable();
    t = getdosmemdword(0x46c);
    _enable();
    return t;
}
#else
# define irq0_ticks()  (getdosmemdword(0x46c))  /* FIXME */
#endif

static void do_idle(void)
{
    PDCREGS regs;

    regs.W.ax = 0x1680;
    PDCINT(0x2f, regs);
    PDCINT(0x28, regs);
}

#define MAX_TICK       0x1800b0L       /* no. of IRQ 0 clock ticks per day;
                                          BIOS counter (0:0x46c) will go up
                                          to MAX_TICK - 1 before wrapping to
                                          0 at midnight */
#define MS_PER_DAY     86400000L       /* no. of milliseconds in a day */

/* 1080 seconds = 18 minutes = 1/80 day is exactly 19663 ticks.
If asked to nap for longer than 1080000 milliseconds,  we take
one or more 18-minute naps.  This avoids wraparound issues and
the integer overflows that would result for ms > MAX_INT / 859
(about 42 minutes).  */

#define MAX_NAP_SPAN      (MS_PER_DAY / 80ul)

void PDC_napmsl( long ms)
{
    long ticks_to_wait, tick0;

    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    while (ms > MAX_NAP_SPAN)
    {
        PDC_napmsl( MAX_NAP_SPAN);
        ms -= MAX_NAP_SPAN;
    }

    if (ms < 0)
        return;

    /* Scale the millisecond count by MAX_TICK / MS_PER_DAY.  We actually
       scale by 859/47181,  which is correct to within four parts per
       billion and avoids the need for floating-point math.   We have to
       round to the nearest integer tick anyway and don't know where we
       started within a tick,  so this error can be accepted.

       Here,  we assume that we start (on average) halfway through a tick,
       but will end almost exactly when the 'goal' tick begins.  So the
       rounding (on average) will work out correctly.       */

    ticks_to_wait = (ms * 859L) / 47181L + 1L;
    tick0 = irq0_ticks();

    for( ;;)
    {
        long t = irq0_ticks() - tick0;

        if( t < 0)           /* midnight rollover */
            t += MAX_TICK;
        if( t >= ticks_to_wait)
            break;
        do_idle();
    }
}

void PDC_napms(int ms)
{
   PDC_napmsl( (long)ms);
}

const char *PDC_sysname(void)
{
    return "DOS";
}

PDC_version_info PDC_version = { PDC_PORT_DOS,
          PDC_VER_MAJOR, PDC_VER_MINOR, PDC_VER_CHANGE,
          sizeof( chtype),
               /* note that thus far,  'wide' and 'UTF8' versions exist */
               /* only for SDL2, X11,  Win32,  and Win32a;  elsewhere, */
               /* these will be FALSE */
#ifdef PDC_WIDE
          TRUE,
#else
          FALSE,
#endif
#ifdef PDC_FORCE_UTF8
          TRUE,
#else
          FALSE,
#endif
          };

#ifdef __DJGPP__

unsigned char getdosmembyte(int offset)
{
    unsigned char b;

    dosmemget(offset, sizeof(unsigned char), &b);
    return b;
}

unsigned short getdosmemword(int offset)
{
    unsigned short w;

    dosmemget(offset, sizeof(unsigned short), &w);
    return w;
}

unsigned long getdosmemdword(int offset)
{
    unsigned long dw;

    dosmemget(offset, sizeof(unsigned long), &dw);
    return dw;
}

void setdosmembyte(int offset, unsigned char b)
{
    dosmemput(&b, sizeof(unsigned char), offset);
}

void setdosmemword(int offset, unsigned short w)
{
    dosmemput(&w, sizeof(unsigned short), offset);
}

#endif

#if defined(__WATCOMC__) && defined(__386__)

void PDC_dpmi_int(int vector, pdc_dpmi_regs *rmregs)
{
    union REGPACK regs = {0};

    rmregs->w.ss = 0;
    rmregs->w.sp = 0;
    rmregs->w.flags = 0;

    regs.w.ax = 0x300;
    regs.h.bl = vector;
    regs.x.edi = FP_OFF(rmregs);
    regs.x.es = FP_SEG(rmregs);

    intr(0x31, &regs);
}

#endif
