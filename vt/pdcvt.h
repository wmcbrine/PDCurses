#define COLORREF int32_t
#ifndef DIVROUND
#define DIVROUND( A, B) ((A + B / 2) / B)
#endif
#define RGB( R, G, B) (((int32_t)(R) << 16) | ((int32_t)(G) << 8) \
                      | (int32_t)(B))

#define GetRValue( rgb) ((int)( (rgb) >> 16))
#define GetGValue( rgb) ((int)( (rgb) >> 8) & 0xff)
#define GetBValue( rgb) ((int)((rgb) & 0xff))

int PDC_pair_content( short pair, short *fg, short *bg);
