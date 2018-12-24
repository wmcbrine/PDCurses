#define PACKED_RGB uint32_t
#ifndef DIVROUND
#define DIVROUND( A, B) ((A + B / 2) / B)
#endif
#define PACK_RGB( R, G, B) (((uint32_t)(R) << 16) | ((uint32_t)(G) << 8) \
                      | (uint32_t)(B))

#define Get_RValue( rgb) ((int)( (rgb) >> 16))
#define Get_GValue( rgb) ((int)( (rgb) >> 8) & 0xff)
#define Get_BValue( rgb) ((int)((rgb) & 0xff))

int PDC_pair_content( short pair, short *fg, short *bg);

   /* If the following is TRUE,  only a limited subset of control
   codes will actually work.  Happens in older Windows and DOS.  */
extern bool PDC_is_ansi;
