
/* Definitions of latin1 character set for XCurses */

/*
 *  Latin 1
 *  Byte 3 = 0
*/
#ifdef XK_LATIN1       

/* keycode              keypad ?        normal         shifted  control alt */
/* ------------------------------------------------------------------------ */
 {XK_nobreakspace,      FALSE,          0x0A0,          0x0A0,  0,      0},
 {XK_exclamdown,        FALSE,          0x0A1,          0x0A1,  0,      0},
 {XK_cent,              FALSE,          0x0A2,          0x0A2,  0,      0},
 {XK_sterling,          FALSE,          0x0A3,          0x0A3,  0,      0},
 {XK_currency,          FALSE,          0x0A4,          0x0A4,  0,      0},
 {XK_yen,               FALSE,          0x0A5,          0x0A5,  0,      0},
 {XK_brokenbar,         FALSE,          0x0A6,          0x0A6,  0,      0},
 {XK_section,           FALSE,          0x0A7,          0x0A7,  0,      0},
 {XK_diaeresis,         FALSE,          0x0A8,          0x0A8,  0,      0},
 {XK_copyright,         FALSE,          0x0A9,          0x0A9,  0,      0},
 {XK_ordfeminine,       FALSE,          0x0AA,          0x0AA,  0,      0},
 {XK_guillemotleft,     FALSE,          0x0AB,          0x0AB,  0,      0},
 {XK_notsign,           FALSE,          0x0AC,          0x0AC,  0,      0},
 {XK_hyphen,            FALSE,          0x0AD,          0x0AD,  0,      0},
 {XK_registered,        FALSE,          0x0AE,          0x0AE,  0,      0},
 {XK_macron,            FALSE,          0x0AF,          0x0AF,  0,      0},
 {XK_degree,            FALSE,          0x0B0,          0x0B0,  0,      0},
 {XK_plusminus,         FALSE,          0x0B1,          0x0B1,  0,      0},
 {XK_twosuperior,       FALSE,          0x0B2,          0x0B2,  0,      0},
 {XK_threesuperior,     FALSE,          0x0B3,          0x0B3,  0,      0},
 {XK_acute,             FALSE,          0x0B4,          0x0B4,  0,      0},
 {XK_mu,                FALSE,          0x0B5,          0x0B5,  0,      0},
 {XK_paragraph,         FALSE,          0x0B6,          0x0B6,  0,      0},
 {XK_periodcentered,    FALSE,          0x0B7,          0x0B7,  0,      0},
 {XK_cedilla,           FALSE,          0x0B8,          0x0B8,  0,      0},
 {XK_onesuperior,       FALSE,          0x0B9,          0x0B9,  0,      0},
 {XK_masculine,         FALSE,          0x0BA,          0x0BA,  0,      0},
 {XK_guillemotright,    FALSE,          0x0BB,          0x0BB,  0,      0},
 {XK_onequarter,        FALSE,          0x0BC,          0x0BC,  0,      0},
 {XK_onehalf,           FALSE,          0x0BD,          0x0BD,  0,      0},
 {XK_threequarters,     FALSE,          0x0BE,          0x0BE,  0,      0},
 {XK_questiondown,      FALSE,          0x0BF,          0x0BF,  0,      0},
/*  -- Uppercases -- */
 {XK_Agrave,            FALSE,          0x0C0,          0x0C0,  0,      0},
 {XK_Aacute,            FALSE,          0x0C1,          0x0C1,  0,      0},
 {XK_Acircumflex,       FALSE,          0x0C2,          0x0C2,  0,      0},
 {XK_Atilde,            FALSE,          0x0C3,          0x0C3,  0,      0},
 {XK_Adiaeresis,        FALSE,          0x0C4,          0x0C4,  0,      0},
 {XK_Aring,             FALSE,          0x0C5,          0x0C5,  0,      0},
 {XK_AE,                FALSE,          0x0C6,          0x0C6,  0,      0},
 /* */
 {XK_Ccedilla,          FALSE,          0x0C7,          0x0C7,  0,      0},
 /* */
 {XK_Egrave,            FALSE,          0x0C8,          0x0C8,  0,      0},
 {XK_Eacute,            FALSE,          0x0C9,          0x0C9,  0,      0},
 {XK_Ecircumflex,       FALSE,          0x0CA,          0x0CA,  0,      0},
 {XK_Ediaeresis,        FALSE,          0x0CB,          0x0CB,  0,      0},
 /* */
 {XK_Igrave,            FALSE,          0x0CC,          0x0CC,  0,      0},
 {XK_Iacute,            FALSE,          0x0CD,          0x0CD,  0,      0},
 {XK_Icircumflex,       FALSE,          0x0CE,          0x0CE,  0,      0},
 {XK_Idiaeresis,        FALSE,          0x0CF,          0x0CF,  0,      0},
 /* */
 {XK_ETH,               FALSE,          0x0D0,          0x0D0,  0,      0},
 {XK_Eth,               FALSE,          0x0D0,          0x0D0,  0,      0},
 {XK_Ntilde,            FALSE,          0x0D1,          0x0D1,  0,      0},
 /* */
 {XK_Ograve,            FALSE,          0x0D2,          0x0D2,  0,      0},
 {XK_Oacute,            FALSE,          0x0D3,          0x0D3,  0,      0},
 {XK_Ocircumflex,       FALSE,          0x0D4,          0x0D4,  0,      0},
 {XK_Otilde,            FALSE,          0x0D5,          0x0D5,  0,      0},
 {XK_Odiaeresis,        FALSE,          0x0D6,          0x0D6,  0,      0},
 {XK_multiply,          FALSE,          0x0D7,          0x0D7,  0,      0},
 {XK_Ooblique,          FALSE,          0x0D8,          0x0D8,  0,      0},
 /* */
 {XK_Ugrave,            FALSE,          0x0D9,          0x0D9,  0,      0},
 {XK_Uacute,            FALSE,          0x0DA,          0x0DA,  0,      0},
 {XK_Ucircumflex,       FALSE,          0x0DB,          0x0DB,  0,      0},
 {XK_Udiaeresis,        FALSE,          0x0DC,          0x0DC,  0,      0},
 /* */
 {XK_Yacute,            FALSE,          0x0DD,          0x0DD,  0,      0},
 {XK_THORN,             FALSE,          0x0DE,          0x0DE,  0,      0},
 {XK_Thorn,             FALSE,          0x0DE,          0x0DE,  0,      0},
 {XK_ssharp,            FALSE,          0x0DF,          0x0DF,  0,      0},
/*  -- Lowercases -- */
 {XK_agrave,            FALSE,          0x0E0,          0x0E0,  0,      0},
 {XK_aacute,            FALSE,          0x0E1,          0x0E1,  0,      0},
 {XK_acircumflex,       FALSE,          0x0E2,          0x0E2,  0,      0},
 {XK_atilde,            FALSE,          0x0E3,          0x0E3,  0,      0},
 {XK_adiaeresis,        FALSE,          0x0E4,          0x0E4,  0,      0},
 {XK_aring,             FALSE,          0x0E5,          0x0E5,  0,      0},
 {XK_ae,                FALSE,          0x0E6,          0x0E6,  0,      0},
 /* */
 {XK_ccedilla,          FALSE,          0x0E7,          0x0E7,  0,      0},
 /* */
 {XK_egrave,            FALSE,          0x0E8,          0x0E8,  0,      0},
 {XK_eacute,            FALSE,          0x0E9,          0x0E9,  0,      0},
 {XK_ecircumflex,       FALSE,          0x0EA,          0x0EA,  0,      0},
 {XK_ediaeresis,        FALSE,          0x0EB,          0x0EB,  0,      0},
 /* */
 {XK_igrave,            FALSE,          0x0EC,          0x0EC,  0,      0},
 {XK_iacute,            FALSE,          0x0ED,          0x0ED,  0,      0},
 {XK_icircumflex,       FALSE,          0x0EE,          0x0EE,  0,      0},
 {XK_idiaeresis,        FALSE,          0x0EF,          0x0EF,  0,      0},
 /* */
 {XK_eth,               FALSE,          0x0F0,          0x0F0,  0,      0},
 {XK_ntilde,            FALSE,          0x0F1,          0x0F1,  0,      0},
 /* */
 {XK_ograve,            FALSE,          0x0F2,          0x0F2,  0,      0},
 {XK_oacute,            FALSE,          0x0F3,          0x0F3,  0,      0},
 {XK_ocircumflex,       FALSE,          0x0F4,          0x0F4,  0,      0},
 {XK_otilde,            FALSE,          0x0F5,          0x0F5,  0,      0},
 {XK_odiaeresis,        FALSE,          0x0F6,          0x0F6,  0,      0},
 {XK_division,          FALSE,          0x0F7,          0x0F7,  0,      0},
 {XK_oslash,            FALSE,          0x0F8,          0x0F8,  0,      0},
 /* */
 {XK_ugrave,            FALSE,          0x0F9,          0x0F9,  0,      0},
 {XK_uacute,            FALSE,          0x0FA,          0x0FA,  0,      0},
 {XK_ucircumflex,       FALSE,          0x0FB,          0x0FB,  0,      0},
 {XK_udiaeresis,        FALSE,          0x0FC,          0x0FC,  0,      0},
 /* */
 {XK_yacute,            FALSE,          0x0FD,          0x0FD,  0,      0},
 {XK_thorn,             FALSE,          0x0FE,          0x0FE,  0,      0},
 {XK_ydiaeresis,        FALSE,          0x0FF,          0x0FF,  0,      0},

#endif /* XK_LATIN1 */                    
