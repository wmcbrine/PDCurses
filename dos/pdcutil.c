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

#define MAX_TICK       0x1800b0ul      /* no. of IRQ 0 clock ticks per day;
                                          BIOS counter (0:0x46c) will go up
                                          to MAX_TICK - 1 before wrapping to
                                          0 at midnight */
#define MS_PER_DAY     86400000ul      /* no. of milliseconds in a day */

/* 1080 seconds = 18 minutes = 1/80 day = 19663 ticks,  exactly.
If asked to nap for longer than 1080000 milliseconds,  we take one
or more 18-minute naps.  This avoids wraparound issues and possible
integer overflows if ms > MAX_INT / 67.  */

#define MAX_NAP_SPAN      (MS_PER_DAY / 80ul)

void PDC_napmsl( long ms)
{
    unsigned long goal;
    long ticks_remaining;

    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    while (ms > MAX_NAP_SPAN)
    {
        PDC_napmsl( MAX_NAP_SPAN);
        ms -= MAX_NAP_SPAN;
    }

    if (ms < 0)
        return;

    /* Scale the millisecond count by MAX_TICK / MS_PER_DAY.  We actually
       scale by 67/3860,  resulting in a maximum error of under 1/20 tick,
       or about two parts in a million.  Since we have to round to the
       nearest integer tick anyway,  this can be accepted.

       Here,  we assume that we start (on average) halfway through a tick,
       but will end almost exactly when the 'goal' tick begins.  So the
       rounding (on average) will work out correctly.

       The approximation 67 / 3680 can be obtained by considering the
       convergents (mathworld.wolfram.com/Convergent.html) of
       MAX_TICK / MS_PER_DAY's continued fraction representation. */

    goal = ((unsigned long)ms * 67ul) / 3680ul + 1ul;
    goal += irq0_ticks();

    do
    {
        ticks_remaining = (long)goal - (long)irq0_ticks();
        while( ticks_remaining > MAX_TICK / 2)
           ticks_remaining -= MAX_TICK;   /* account for midnight rollover */
        while( ticks_remaining < -MAX_TICK / 2)
           ticks_remaining += MAX_TICK;
        do_idle();
    }
    while( ticks_remaining > 0);
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
