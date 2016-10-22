/* Public Domain Curses */

#include "pdcdos.h"

/* ACS definitions originally by jshumate@wrdis01.robins.af.mil -- these
   match code page 437 and compatible pages (CP850, CP852, etc.) */

#ifdef CHTYPE_LONG

#include "acs_defs.h"

# define A(x) ((chtype)x | A_ALTCHARSET)

/* This assumes use of code page 437,  or some compatible pages
such as CP850 or 852. */

#define USE_UNICODE_ACS_CHARS 0

chtype acs_map[128] =
{
   A_ORDINAL, O_ORDINAL, INVERTED_QUESTION_MARK,                /* 0 1 2 */
   REVERSED_NOT_SIGN, NOT_SIGN, VULGAR_HALF,                    /* 3 4 5 */
   VULGAR_QUARTER, INVERTED_EXCLAMATION_MARK,                   /* 6 7   */
   LEFT_ANGLE_QUOTE_MARK, RIGHT_ANGLE_QUOTE_MARK,               /* 8 9   */
   UPPER_HALF_INTEGRAL_SIGN, LOWER_HALF_INTEGRAL_SIGN,          /* 10 11 */
   SUPERSCRIPT_N, CENTERED_SQUARE, PESETA_SIGN, F_WITH_HOOK,    /* 12-15 */
   RIGHT_SINGLE_DOWN_DOUBLE, RIGHT_SINGLE_UP_DOUBLE,            /* 16 17 */
   DOWN_DOUBLE_LEFT_SINGLE, UP_DOUBLE_LEFT_SINGLE,              /* 18 19 */
   VERTICAL_DOUBLE_RIGHT_SINGLE, VERTICAL_DOUBLE_LEFT_SINGLE,   /* 20 21 */
   HORIZONTAL_SINGLE_DOWN_DOUBLE, HORIZONTAL_SINGLE_UP_DOUBLE,  /* 22 23 */
   HORIZONTAL_SINGLE_VERTICAL_DOUBLE,                           /* 24    */
   RIGHT_DOUBLE_DOWN_SINGLE, RIGHT_DOUBLE_UP_SINGLE,            /* 25 26 */
   LEFT_DOUBLE_DOWN_SINGLE, UP_SINGLE_LEFT_DOUBLE,              /* 27 28 */
   VERTICAL_SINGLE_RIGHT_DOUBLE, VERTICAL_SINGLE_LEFT_DOUBLE,   /* 29 30 */
   HORIZONTAL_DOUBLE_DOWN_SINGLE, HORIZONTAL_DOUBLE_UP_SINGLE,  /* 31 ' ' */
   HORIZONTAL_DOUBLE_VERTICAL_SINGLE,                           /* !     */

    '"', '#', '$', '%', '&', '\'', '(',
    ')', '*',

   RIGHT_ARROW, LEFT_ARROW, UP_ARROW, DOWN_ARROW,                   /* +,-.  */
   '/', FULL_BLOCK,                                                 /* /0    */
   CENT_SIGN, YEN_SIGN, PESETA_SIGN, VULGAR_HALF, VULGAR_QUARTER,   /* 12345 */
   LEFT_ANGLE_QUOTE_MARK, RIGHT_ANGLE_QUOTE_MARK, DARK_SHADE,       /* 678 */
   BOX_DBL_DOWN_RIGHT, BOX_DBL_UP_RIGHT,                            /* 9: */
   BOX_DBL_DOWN_LEFT, BOX_DBL_UP_LEFT,                              /* ;< */
   BOX_DBL_TEE_LEFT, BOX_DBL_TEE_RIGHT,                             /* => */
   BOX_DBL_TEE_UP, BOX_DBL_TEE_DOWN,                                /* ?@ */
   BOX_DBL_HORIZONTAL, BOX_DBL_VERTICAL,                            /* AB */
   BOX_DBL_CROSS, SUPERSCRIPT_2,                                    /* CD */
   ALPHA, BETA, GAMMA, UPPERCASE_SIGMA, LOWERCASE_SIGMA,         /* EFGHI */
   MU, TAU, UPPERCASE_PHI, THETA, OMEGA, DELTA, INFINITY,       /* JKLMNOP */
   LOWERCASE_PHI, EPSILON, INTERSECTION, TRIPLE_BAR,            /* QRST */
   DIVISION_SIGN, APPROXIMATELY_EQUALS_SIGN, SMALL_BULLET,      /* UVW */
   SQUARE_ROOT, UPPER_HALF_BLOCK, LOWER_HALF_BLOCK,             /* XYZ */
   LEFT_HALF_BLOCK, RIGHT_HALF_BLOCK,                           /* [\ */

   ']', '^', '_',
    DIAMOND, MEDIUM_SHADE,          /* `a */

    'b', 'c', 'd', 'e',

   DEGREE_SIGN, PLUS_OR_MINUS, LIGHT_SHADE, SPLAT,          /* fghi */
   LEFT_UP, DOWN_LEFT, RIGHT_DOWN, UP_RIGHT,                /* jklm */
   HORIZONTAL_VERTICAL, HORIZ_SCAN_LINE_1, HORIZ_SCAN_LINE_3,  /* nop */
   HORIZONTAL, HORIZ_SCAN_LINE_7, HORIZ_SCAN_LINE_9,        /* qrs */
   RIGHT_VERTICAL, VERTICAL_LEFT, UP_HORIZONTAL,            /* tuv */
   DOWN_HORIZONTAL, VERTICAL, LESSER_THAN_OR_EQUAL_TO,      /* wxy */
   GREATER_THAN_OR_EQUAL_TO, PI, NOT_EQUALS_SIGN,           /* z{| */
   STERLING_SIGN, LARGE_BULLET,                             /* }~  */

    A(127)
};

# undef A

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
