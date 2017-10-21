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

#if UINT_MAX >= 0xffffffffful
# define irq0_ticks()	(getdosmemdword(0x46c))
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
# define irq0_ticks()	(getdosmemdword(0x46c))  /* FIXME */
#endif

static void do_idle(void)
{
    PDCREGS regs;

    regs.W.ax = 0x1680;
    PDCINT(0x2f, regs);
    PDCINT(0x28, regs);
}

#define MAX_TICK	0x1800b0ul	/* no. of IRQ 0 clock ticks per day;
					   BIOS counter (0:0x46c) will go up
					   to MAX_TICK - 1 before wrapping to
					   0 at midnight */
#define MS_PER_DAY	86400000ul	/* no. of milliseconds in a day */

void PDC_napms(int ms)
{
    unsigned long goal, start, current;

    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

#if INT_MAX > MS_PER_DAY / 2
    /* If `int' is 32-bit, we might be asked to "nap" for more than one day,
       in which case the system timer might wrap around at least twice, and
       that will be tricky to handle as is.  Slice the "nap" into half-day
       portions.  */
    while (ms > MS_PER_DAY / 2)
    {
        PDC_napms (MS_PER_DAY / 2);
        ms -= MS_PER_DAY / 2;
    }
#endif

    if (ms < 0)
        return;

    /* Scale the millisecond count by MAX_TICK / MS_PER_DAY.  The scaling
       done here is not very precise, but what is more important is
       preventing integer overflow.

       The approximation 67 / 3680 can be obtained by considering the
       convergents (mathworld.wolfram.com/Convergent.html) of MAX_TICK /
       MS_PER_DAY 's continued fraction representation.  */
#if MS_PER_DAY / 2 <= ULONG_MAX / 67ul
# define MS_TO_TICKS(x)	((x) * 67ul / 3680ul)
#else
# error "unpossible!"
#endif
    goal = MS_TO_TICKS(ms);

    if (!goal)
        goal++;

    start = irq0_ticks();
    goal += start;

    if (goal >= MAX_TICK)
    {
        /* We expect to cross over midnight!  Wait for the clock tick count
           to wrap around, then wait out the remaining ticks.  */
        goal -= MAX_TICK;

        while (irq0_ticks() == start)
            do_idle();

        while (irq0_ticks() > start)
            do_idle();

        start = 0;
    }

    while (goal > (current = irq0_ticks()))
    {
        if (current < start)
        {
            /* If the BIOS time somehow gets reset under us (ugh!), then
               restart (what is left of) the nap with `current' as the new
               starting time.  Remember to adjust the goal time
               accordingly!  */
            goal -= start - current;
            start = current;
        }

        do_idle();
    }
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
