#define PACKED_RGB uint32_t

#define Get_BValue( rgb) ((int)( (rgb) >> 16))
#define Get_GValue( rgb) ((int)( (rgb) >> 8) & 0xff)
#define Get_RValue( rgb) ((int)((rgb) & 0xff))

   /* If the following is TRUE,  only a limited subset of control codes
    will actually work.  Happens in older Windows, DOS, Linux console. */
extern int PDC_is_ansi;

#ifdef PDC_WIDE
   #if !defined( UNICODE)
      # define UNICODE
   #endif
   #if !defined( _UNICODE)
      # define _UNICODE
   #endif
#endif

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE 1   /* kill nonsense warnings */
#endif
