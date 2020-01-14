#define PACKED_RGB uint32_t

#define Get_BValue( rgb) ((int)( (rgb) >> 16))
#define Get_GValue( rgb) ((int)( (rgb) >> 8) & 0xff)
#define Get_RValue( rgb) ((int)((rgb) & 0xff))

   /* If the following is TRUE,  only a limited subset of control
   codes will actually work.  Happens in older Windows and DOS.  */
extern bool PDC_is_ansi;
