/* Public Domain Curses */

#include "pdcdos.h"

/* ACS definitions originally by jshumate@wrdis01.robins.af.mil -- these
   match code page 437 and compatible pages (CP850, CP852, etc.) */

#ifdef CHTYPE_LONG
   #define USE_UNICODE_ACS_CHARS 0
   #include "acs_defs.h"
#endif

#ifdef __PACIFIC__
void movedata(unsigned sseg, unsigned soff, unsigned dseg,
              unsigned doff, unsigned n)
{
    far char *src = MK_FP(sseg, soff);
    far char *dst = MK_FP(dseg, doff);

    while (n--)
        *dst++ = *src++;
}
#endif

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    PDCREGS regs;

    PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

    regs.h.ah = 0x02;
    regs.h.bh = 0;
    regs.h.dh = (unsigned char) row;
    regs.h.dl = (unsigned char) col;
    PDCINT(0x10, regs);
}

/* update the given physical line to look like the corresponding line in
   curscr */

/* NOTE:  the original indexing into pdc_atrtab[] relied on three or five
   attribute bits in 'chtype' being adjacent to the color bits.  Such is
   not the case for 64-bit chtypes (CHTYPE_LONG == 2),  so we have to do
   additional bit-fiddling for that situation.  Code is similar in Win32
   and DOS flavors.  (BJG) */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    int j;

    PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

    if (pdc_direct_video)
    {
#if SMALL || MEDIUM
# ifndef __PACIFIC__
        struct SREGS segregs;
# endif
        int ds;
#endif
        /* this should be enough for the maximum width of a screen */

        struct {unsigned char text, attr;} temp_line[256];

        /* replace the attribute part of the chtype with the actual
           color value for each chtype in the line */

        for (j = 0; j < len; j++)
        {
            chtype ch = srcp[j];

#if defined( CHTYPE_LONG) && (CHTYPE_LONG >= 2)
            temp_line[j].attr = pdc_atrtab[((ch >> PDC_ATTR_SHIFT) & 0x1f)
                     | (((ch >> PDC_COLOR_SHIFT) & 0xff) << 5)];
#else
            temp_line[j].attr = pdc_atrtab[ch >> PDC_ATTR_SHIFT];
#endif

#ifdef CHTYPE_LONG
            if (ch & A_ALTCHARSET && !(ch & 0xff80))
                ch = acs_map[ch & 0x7f];
#endif
            temp_line[j].text = ch & 0xff;
        }

#ifdef __DJGPP__
        dosmemput(temp_line, len * 2,
                  (unsigned long)_FAR_POINTER(pdc_video_seg,
                  pdc_video_ofs + (lineno * curscr->_maxx + x) * 2));
#else
# if SMALL || MEDIUM
#  ifdef __PACIFIC__
        ds = FP_SEG((void far *) temp_line);
#  else
        segread(&segregs);
        ds = segregs.ds;
#  endif
        movedata(ds, (int)temp_line, pdc_video_seg,
                 pdc_video_ofs + (lineno * curscr->_maxx + x) * 2, len * 2);
# else
        memcpy((void *)_FAR_POINTER(pdc_video_seg,
               pdc_video_ofs + (lineno * curscr->_maxx + x) * 2),
               temp_line, len * 2);
# endif
#endif

    }
    else
        for (j = 0; j < len;)
        {
            PDCREGS regs;
            unsigned short count = 1;
            chtype ch = srcp[j];

            while ((j + count < len) && (ch == srcp[j + count]))
                count++;

            PDC_gotoyx(lineno, j + x);

            regs.h.ah = 0x09;
            regs.W.bx = pdc_atrtab[ch >> PDC_ATTR_SHIFT];
            regs.W.cx = count;
#ifdef CHTYPE_LONG
            if (ch & A_ALTCHARSET && !(ch & 0xff80))
                ch = acs_map[ch & 0x7f];
#endif
            regs.h.al = (unsigned char) (ch & 0xff);

            PDCINT(0x10, regs);

            j += count;
        }
}
