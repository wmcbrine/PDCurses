/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#define CURSES_LIBRARY 1
#define INCLUDE_WINDOWS_H
#include <curses.h>

RCSID("$Id: pdckbd.c,v 1.53 2006/07/30 22:00:25 wmcbrine Exp $");

#define ACTUAL_MOUSE_MOVED	  (Actual_Mouse_status.changes & 8)
#define ACTUAL_BUTTON_STATUS(x)   (Actual_Mouse_status.button[(x) - 1])

#define TEMP_MOUSE_X_POS	  (Temp_Mouse_status.x)
#define TEMP_MOUSE_Y_POS	  (Temp_Mouse_status.y)
#define TEMP_BUTTON_STATUS(x)	  (Temp_Mouse_status.button[(x) - 1])

#define KEY_STATE TRUE
#define MS_MOUSE_MOVED 1

/* MingW32 header files are missing the following in some versions
   of the compiler */

#ifndef DOUBLE_CLICK
# define DOUBLE_CLICK 0x0002
#endif

#ifndef MOUSE_WHEELED
# define MOUSE_WHEELED 0x0004
#endif

#ifndef FROM_LEFT_1ST_BUTTON_PRESSED
# define FROM_LEFT_1ST_BUTTON_PRESSED 0x0001
#endif

#ifndef FROM_LEFT_2ND_BUTTON_PRESSED
# define FROM_LEFT_2ND_BUTTON_PRESSED 0x0004
#endif

#ifndef RIGHTMOST_BUTTON_PRESSED
# define RIGHTMOST_BUTTON_PRESSED 0x0002
#endif

/* These variables are used to store information about the next
   Input Event. */

static INPUT_RECORD save_ip;
unsigned long pdc_key_modifiers = 0L;

extern HANDLE hConIn;

#ifdef PDC_THREAD_BUILD
extern HANDLE hPipeRead;
extern HANDLE hPipeWrite;
extern HANDLE hSemKeyCount;
#else
static int keyCount = 0;
#endif

static void win32_getch(void);
static int win32_kbhit(int);

/************************************************************************
 *    Table for key code translation of function keys in keypad mode	*  
 *    These values are for strict IBM keyboard compatibles only		*
 ************************************************************************/

typedef struct
{
	int normal;
	int shift;
	int control;
	int alt;
	int extended;
} KPTAB;

static KPTAB kptab[] =
{
   {0,		0,	   0,		0,	    0	}, /* 0  */
   {0,		0,	   0,		0,	    0	}, /* 1   VK_LBUTTON */
   {0,		0,	   0,		0,	    0	}, /* 2   VK_RBUTTON */
   {0,		0,	   0,		0,	    0	}, /* 3   VK_CANCEL  */
   {0,		0,	   0,		0,	    0	}, /* 4   VK_MBUTTON */
   {0,		0,	   0,		0,	    0	}, /* 5   */
   {0,		0,	   0,		0,	    0	}, /* 6   */
   {0,		0,	   0,		0,	    0	}, /* 7   */
   {0x08,	0x08,	   0x7F,	ALT_BKSP,   0	}, /* 8   VK_BACK    */
   {0x09,	KEY_BTAB,  CTL_TAB,	ALT_TAB,    999 }, /* 9   VK_TAB     */
   {0,		0,	   0,		0,	    0	}, /* 10  */
   {0,		0,	   0,		0,	    0	}, /* 11  */
   {KEY_B2,	0x35,	   CTL_PAD5,	ALT_PAD5,   0	}, /* 12  VK_CLEAR   */
   {0x0D,	0x0D,	   CTL_ENTER,	ALT_ENTER,  1	}, /* 13  VK_RETURN  */
   {0,		0,	   0,		0,	    0	}, /* 14  */
   {0,		0,	   0,		0,	    0	}, /* 15  */
   {0,		0,	   0,		0,	    0	}, /* 16  VK_SHIFT   HANDLED SEPARATELY */
   {0,		0,	   0,		0,	    0	}, /* 17  VK_CONTROL HANDLED SEPARATELY */
   {0,		0,	   0,		0,	    0	}, /* 18  VK_MENU    HANDLED SEPARATELY */
   {0,		0,	   0,		0,	    0	}, /* 19  VK_PAUSE   */
   {0,		0,	   0,		0,	    0	}, /* 20  VK_CAPITAL HANDLED SEPARATELY */
   {0,		0,	   0,		0,	    0	}, /* 21  VK_HANGUL  */
   {0,		0,	   0,		0,	    0	}, /* 22  */
   {0,		0,	   0,		0,	    0	}, /* 23  VK_JUNJA   */
   {0,		0,	   0,		0,	    0	}, /* 24  VK_FINAL   */
   {0,		0,	   0,		0,	    0	}, /* 25  VK_HANJA   */
   {0,		0,	   0,		0,	    0	}, /* 26  */
   {0x1B,	0x1B,	   0x1B,	ALT_ESC,    0	}, /* 27  VK_ESCAPE  */
   {0,		0,	   0,		0,	    0	}, /* 28  VK_CONVERT */
   {0,		0,	   0,		0,	    0	}, /* 29  VK_NONCONVERT */
   {0,		0,	   0,		0,	    0	}, /* 30  VK_ACCEPT  */
   {0,		0,	   0,		0,	    0	}, /* 31  VK_MODECHANGE */
   {0x20,	0x20,	   0x20,	0x20,	    0	}, /* 32  VK_SPACE   */
   {KEY_A3,	0x39,	   CTL_PAD9,	ALT_PAD9,   3	}, /* 33  VK_PRIOR   */
   {KEY_C3,	0x33,	   CTL_PAD3,	ALT_PAD3,   4	}, /* 34  VK_NEXT    */
   {KEY_C1,	0x31,	   CTL_PAD1,	ALT_PAD1,   5	}, /* 35  VK_END     */
   {KEY_A1,	0x37,	   CTL_PAD7,	ALT_PAD7,   6	}, /* 36  VK_HOME    */
   {KEY_B1,	0x34,	   CTL_PAD4,	ALT_PAD4,   7	}, /* 37  VK_LEFT    */
   {KEY_A2,	0x38,	   CTL_PAD8,	ALT_PAD8,   8	}, /* 38  VK_UP      */
   {KEY_B3,	0x36,	   CTL_PAD6,	ALT_PAD6,   9	}, /* 39  VK_RIGHT   */
   {KEY_C2,	0x32,	   CTL_PAD2,	ALT_PAD2,   10	}, /* 40  VK_DOWN    */
   {0,		0,	   0,		0,	    0	}, /* 41  VK_SELECT  */
   {0,		0,	   0,		0,	    0	}, /* 42  VK_PRINT   */
   {0,		0,	   0,		0,	    0	}, /* 43  VK_EXECUTE */
   {0,		0,	   0,		0,	    0	}, /* 44  VK_SNAPSHOT*/
   {PAD0,	0x30,	   CTL_PAD0,	ALT_PAD0,   11	}, /* 45  VK_INSERT  */
   {PADSTOP,	0x2E,	   CTL_PADSTOP, ALT_PADSTOP,12	}, /* 46  VK_DELETE  */
   {0,		0,	   0,		0,	    0	}, /* 47  VK_HELP    */
   {0x30,	0x29,	   0,		ALT_0,	    0	}, /* 48  */
   {0x31,	0x21,	   0,		ALT_1,	    0	}, /* 49  */
   {0x32,	0x40,	   0,		ALT_2,	    0	}, /* 50  */
   {0x33,	0x23,	   0,		ALT_3,	    0	}, /* 51  */
   {0x34,	0x24,	   0,		ALT_4,	    0	}, /* 52  */
   {0x35,	0x25,	   0,		ALT_5,	    0	}, /* 53  */
   {0x36,	0x5E,	   0,		ALT_6,	    0	}, /* 54  */
   {0x37,	0x26,	   0,		ALT_7,	    0	}, /* 55  */
   {0x38,	0x2A,	   0,		ALT_8,	    0	}, /* 56  */
   {0x39,	0x28,	   0,		ALT_9,	    0	}, /* 57  */
   {0,		0,	   0,		0,	    0	}, /* 58  */
   {0,		0,	   0,		0,	    0	}, /* 59  */
   {0,		0,	   0,		0,	    0	}, /* 60  */
   {0,		0,	   0,		0,	    0	}, /* 61  */
   {0,		0,	   0,		0,	    0	}, /* 62  */
   {0,		0,	   0,		0,	    0	}, /* 63  */
   {0,		0,	   0,		0,	    0	}, /* 64  */
   {0x61,	0x41,	   0x01,	ALT_A,	    0	}, /* 65  */
   {0x62,	0x42,	   0x02,	ALT_B,	    0	}, /* 66  */
   {0x63,	0x43,	   0x03,	ALT_C,	    0	}, /* 67  */
   {0x64,	0x44,	   0x04,	ALT_D,	    0	}, /* 68  */
   {0x65,	0x45,	   0x05,	ALT_E,	    0	}, /* 69  */
   {0x66,	0x46,	   0x06,	ALT_F,	    0	}, /* 70  */
   {0x67,	0x47,	   0x07,	ALT_G,	    0	}, /* 71  */
   {0x68,	0x48,	   0x08,	ALT_H,	    0	}, /* 72  */
   {0x69,	0x49,	   0x09,	ALT_I,	    0	}, /* 73  */
   {0x6A,	0x4A,	   0x0A,	ALT_J,	    0	}, /* 74  */
   {0x6B,	0x4B,	   0x0B,	ALT_K,	    0	}, /* 75  */
   {0x6C,	0x4C,	   0x0C,	ALT_L,	    0	}, /* 76  */
   {0x6D,	0x4D,	   0x0D,	ALT_M,	    0	}, /* 77  */
   {0x6E,	0x4E,	   0x0E,	ALT_N,	    0	}, /* 78  */
   {0x6F,	0x4F,	   0x0F,	ALT_O,	    0	}, /* 79  */
   {0x70,	0x50,	   0x10,	ALT_P,	    0	}, /* 80  */
   {0x71,	0x51,	   0x11,	ALT_Q,	    0	}, /* 81  */
   {0x72,	0x52,	   0x12,	ALT_R,	    0	}, /* 82  */
   {0x73,	0x53,	   0x13,	ALT_S,	    0	}, /* 83  */
   {0x74,	0x54,	   0x14,	ALT_T,	    0	}, /* 84  */
   {0x75,	0x55,	   0x15,	ALT_U,	    0	}, /* 85  */
   {0x76,	0x56,	   0x16,	ALT_V,	    0	}, /* 86  */
   {0x77,	0x57,	   0x17,	ALT_W,	    0	}, /* 87  */
   {0x78,	0x58,	   0x18,	ALT_X,	    0	}, /* 88  */
   {0x79,	0x59,	   0x19,	ALT_Y,	    0	}, /* 89  */
   {0x7A,	0x5A,	   0x1A,	ALT_Z,	    0	}, /* 90  */
   {0,		0,	   0,		0,	    0	}, /* 91  VK_LWIN    */
   {0,		0,	   0,		0,	    0	}, /* 92  VK_RWIN    */
   {0,		0,	   0,		0,	    0	}, /* 93  VK_APPS    */
   {0,		0,	   0,		0,	    0	}, /* 94  */
   {0,		0,	   0,		0,	    0	}, /* 95  */
   {0x30,	0,	   CTL_PAD0,	ALT_PAD0,   0	}, /* 96  VK_NUMPAD0 */
   {0x31,	0,	   CTL_PAD1,	ALT_PAD1,   0	}, /* 97  VK_NUMPAD1 */
   {0x32,	0,	   CTL_PAD2,	ALT_PAD2,   0	}, /* 98  VK_NUMPAD2 */
   {0x33,	0,	   CTL_PAD3,	ALT_PAD3,   0	}, /* 99  VK_NUMPAD3 */
   {0x34,	0,	   CTL_PAD4,	ALT_PAD4,   0	}, /* 100 VK_NUMPAD4 */
   {0x35,	0,	   CTL_PAD5,	ALT_PAD5,   0	}, /* 101 VK_NUMPAD5 */
   {0x36,	0,	   CTL_PAD6,	ALT_PAD6,   0	}, /* 102 VK_NUMPAD6 */
   {0x37,	0,	   CTL_PAD7,	ALT_PAD7,   0	}, /* 103 VK_NUMPAD7 */
   {0x38,	0,	   CTL_PAD8,	ALT_PAD8,   0	}, /* 104 VK_NUMPAD8 */
   {0x39,	0,	   CTL_PAD9,	ALT_PAD9,   0	}, /* 105 VK_NUMPAD9 */
   {PADSTAR,   SHF_PADSTAR,CTL_PADSTAR, ALT_PADSTAR,999 }, /* 106 VK_MULTIPLY*/
   {PADPLUS,   SHF_PADPLUS,CTL_PADPLUS, ALT_PADPLUS,999 }, /* 107 VK_ADD     */
   {0,		0,	   0,		0,	    0	}, /* 108 VK_SEPARATOR     */
   {PADMINUS, SHF_PADMINUS,CTL_PADMINUS,ALT_PADMINUS,999}, /* 109 VK_SUBTRACT*/
   {0x2E,	0,	   CTL_PADSTOP, ALT_PADSTOP,0	}, /* 110 VK_DECIMAL */
   {PADSLASH,  SHF_PADSLASH,CTL_PADSLASH,ALT_PADSLASH,2 }, /* 111 VK_DIVIDE  */
   {KEY_F(1),	KEY_F(13), KEY_F(25),	KEY_F(37),  0	}, /* 112 VK_F1      */
   {KEY_F(2),	KEY_F(14), KEY_F(26),	KEY_F(38),  0	}, /* 113 VK_F2      */
   {KEY_F(3),	KEY_F(15), KEY_F(27),	KEY_F(39),  0	}, /* 114 VK_F3      */
   {KEY_F(4),	KEY_F(16), KEY_F(28),	KEY_F(40),  0	}, /* 115 VK_F4      */
   {KEY_F(5),	KEY_F(17), KEY_F(29),	KEY_F(41),  0	}, /* 116 VK_F5      */
   {KEY_F(6),	KEY_F(18), KEY_F(30),	KEY_F(42),  0	}, /* 117 VK_F6      */
   {KEY_F(7),	KEY_F(19), KEY_F(31),	KEY_F(43),  0	}, /* 118 VK_F7      */
   {KEY_F(8),	KEY_F(20), KEY_F(32),	KEY_F(44),  0	}, /* 119 VK_F8      */
   {KEY_F(9),	KEY_F(21), KEY_F(33),	KEY_F(45),  0	}, /* 120 VK_F9      */
   {KEY_F(10),	KEY_F(22), KEY_F(34),	KEY_F(46),  0	}, /* 121 VK_F10     */
   {KEY_F(11),	KEY_F(23), KEY_F(35),	KEY_F(47),  0	}, /* 122 VK_F11     */
   {KEY_F(12),	KEY_F(24), KEY_F(36),	KEY_F(48),  0	}, /* 123 VK_F12     */
   {0,		0,	   0,		0,	    0	}, /* 124 VK_F13     */
   {0,		0,	   0,		0,	    0	}, /* 125 VK_F14     */
   {0,		0,	   0,		0,	    0	}, /* 126 VK_F15     */
   {0,		0,	   0,		0,	    0	}, /* 127 VK_F16     */
   {0,		0,	   0,		0,	    0	}, /* 128 VK_F17     */
   {0,		0,	   0,		0,	    0	}, /* 129 VK_F18     */
   {0,		0,	   0,		0,	    0	}, /* 130 VK_F19     */
   {0,		0,	   0,		0,	    0	}, /* 131 VK_F20     */
   {0,		0,	   0,		0,	    0	}, /* 132 VK_F21     */
   {0,		0,	   0,		0,	    0	}, /* 133 VK_F22     */
   {0,		0,	   0,		0,	    0	}, /* 134 VK_F23     */
   {0,		0,	   0,		0,	    0	}, /* 135 VK_F24     */
   {0,		0,	   0,		0,	    0	}, /* 136 */
   {0,		0,	   0,		0,	    0	}, /* 137 */
   {0,		0,	   0,		0,	    0	}, /* 138 */
   {0,		0,	   0,		0,	    0	}, /* 139 */
   {0,		0,	   0,		0,	    0	}, /* 140 */
   {0,		0,	   0,		0,	    0	}, /* 141 */
   {0,		0,	   0,		0,	    0	}, /* 142 */
   {0,		0,	   0,		0,	    0	}, /* 143 */
   {0,		0,	   0,		0,	    0	}, /* 144 VK_NUMLOCK HANDLED SEPARATELY */
   {0,		0,	   0,		0,	    0	}, /* 145 VK_SCROLL  HANDLED SEPARATELY */
   {0,		0,	   0,		0,	    0	}, /* 146 */
   {0,		0,	   0,		0,	    0	}, /* 147 */
   {0,		0,	   0,		0,	    0	}, /* 148 */
   {0,		0,	   0,		0,	    0	}, /* 149 */
   {0,		0,	   0,		0,	    0	}, /* 150 */
   {0,		0,	   0,		0,	    0	}, /* 151 */
   {0,		0,	   0,		0,	    0	}, /* 152 */
   {0,		0,	   0,		0,	    0	}, /* 153 */
   {0,		0,	   0,		0,	    0	}, /* 154 */
   {0,		0,	   0,		0,	    0	}, /* 155 */
   {0,		0,	   0,		0,	    0	}, /* 156 */
   {0,		0,	   0,		0,	    0	}, /* 157 */
   {0,		0,	   0,		0,	    0	}, /* 158 */
   {0,		0,	   0,		0,	    0	}, /* 159 */
   {0,		0,	   0,		0,	    0	}, /* 160 VK_LSHIFT  */
   {0,		0,	   0,		0,	    0	}, /* 161 VK_RSHIFT  */
   {0,		0,	   0,		0,	    0	}, /* 162 VK_LCONTROL*/
   {0,		0,	   0,		0,	    0	}, /* 163 VK_RCONTROL*/
   {0,		0,	   0,		0,	    0	}, /* 164 VK_LMENU   */
   {0,		0,	   0,		0,	    0	}, /* 165 VK_RMENU   */
   {0,		0,	   0,		0,	    0	}, /* 166 */
   {0,		0,	   0,		0,	    0	}, /* 167 */
   {0,		0,	   0,		0,	    0	}, /* 168 */
   {0,		0,	   0,		0,	    0	}, /* 169 */
   {0,		0,	   0,		0,	    0	}, /* 170 */
   {0,		0,	   0,		0,	    0	}, /* 171 */
   {0,		0,	   0,		0,	    0	}, /* 172 */
   {0,		0,	   0,		0,	    0	}, /* 173 */
   {0,		0,	   0,		0,	    0	}, /* 174 */
   {0,		0,	   0,		0,	    0	}, /* 175 */
   {0,		0,	   0,		0,	    0	}, /* 176 */
   {0,		0,	   0,		0,	    0	}, /* 177 */
   {0,		0,	   0,		0,	    0	}, /* 178 */
   {0,		0,	   0,		0,	    0	}, /* 179 */
   {0,		0,	   0,		0,	    0	}, /* 180 */
   {0,		0,	   0,		0,	    0	}, /* 181 */
   {0,		0,	   0,		0,	    0	}, /* 182 */
   {0,		0,	   0,		0,	    0	}, /* 183 */
   {0,		0,	   0,		0,	    0	}, /* 184 */
   {0,		0,	   0,		0,	    0	}, /* 185 */
   {0,		0,	   0,		0,	    0	}, /* 186 */
   {0,		0,	   0,		0,	    0	}, /* 187 */
   {0,		0,	   0,		0,	    0	}, /* 188 */
   {0,		0,	   0,		0,	    0	}, /* 189 */
   {0,		0,	   0,		0,	    0	}, /* 190 */
   {0,		0,	   0,		0,	    0	}, /* 191 */
   {0,		0,	   0,		0,	    0	}, /* 192 */
   {0,		0,	   0,		0,	    0	}, /* 193 */
   {0,		0,	   0,		0,	    0	}, /* 194 */
   {0,		0,	   0,		0,	    0	}, /* 195 */
   {0,		0,	   0,		0,	    0	}, /* 196 */
   {0,		0,	   0,		0,	    0	}, /* 197 */
   {0,		0,	   0,		0,	    0	}, /* 198 */
   {0,		0,	   0,		0,	    0	}, /* 199 */
   {0,		0,	   0,		0,	    0	}, /* 200 */
   {0,		0,	   0,		0,	    0	}, /* 201 */
   {0,		0,	   0,		0,	    0	}, /* 202 */
   {0,		0,	   0,		0,	    0	}, /* 203 */
   {0,		0,	   0,		0,	    0	}, /* 204 */
   {0,		0,	   0,		0,	    0	}, /* 205 */
   {0,		0,	   0,		0,	    0	}, /* 206 */
   {0,		0,	   0,		0,	    0	}, /* 207 */
   {0,		0,	   0,		0,	    0	}, /* 208 */
   {0,		0,	   0,		0,	    0	}, /* 209 */
   {0,		0,	   0,		0,	    0	}, /* 210 */
   {0,		0,	   0,		0,	    0	}, /* 211 */
   {0,		0,	   0,		0,	    0	}, /* 212 */
   {0,		0,	   0,		0,	    0	}, /* 213 */
   {0,		0,	   0,		0,	    0	}, /* 214 */
   {0,		0,	   0,		0,	    0	}, /* 215 */
   {0,		0,	   0,		0,	    0	}, /* 216 */
   {0,		0,	   0,		0,	    0	}, /* 217 */
   {0,		0,	   0,		0,	    0	}, /* 218 */
   {0x5B,	0x7B,	   0x1B,	ALT_LBRACKET,0	}, /* 219 */
   {0x5C,	0x7C,	   0x1C,	ALT_BSLASH, 0	}, /* 220 */
   {0x5D,	0x7D,	   0x1D,	ALT_RBRACKET,0	}, /* 221 */
   {0,		0,	   0x27,	ALT_FQUOTE, 0	}, /* 222 */
   {0,		0,	   0,		0,	    0	}, /* 223 */
   {0,		0,	   0,		0,	    0	}, /* 224 */
   {0,		0,	   0,		0,	    0	}  /* 225 */
};

static KPTAB ext_kptab[] =
{
   {0,		0,		0,		0,	    }, /* MUST BE EMPTY */
   {PADENTER,	SHF_PADENTER,	CTL_PADENTER,	ALT_PADENTER}, /* 13 */
   {PADSLASH,	SHF_PADSLASH,	CTL_PADSLASH,	ALT_PADSLASH}, /* 111 */
   {KEY_PPAGE,	KEY_SPREVIOUS,	CTL_PGUP,	ALT_PGUP    }, /* 33 */
   {KEY_NPAGE,	KEY_SNEXT,	CTL_PGDN,	ALT_PGDN    }, /* 34 */
   {KEY_END,	KEY_SEND,	CTL_END,	ALT_END     }, /* 35 */
   {KEY_HOME,	KEY_SHOME,	CTL_HOME,	ALT_HOME    }, /* 36 */
   {KEY_LEFT,	KEY_SLEFT,	CTL_LEFT,	ALT_LEFT    }, /* 37 */
   {KEY_UP,	KEY_SUP,	CTL_UP,		ALT_UP	    }, /* 38 */
   {KEY_RIGHT,	KEY_SRIGHT,	CTL_RIGHT,	ALT_RIGHT   }, /* 39 */
   {KEY_DOWN,	KEY_SDOWN,	CTL_DOWN,	ALT_DOWN    }, /* 40 */
   {KEY_IC,	KEY_SIC,	CTL_INS,	ALT_INS     }, /* 45 */
   {KEY_DC,	KEY_SDC,	CTL_DEL,	ALT_DEL     }, /* 46 */
   {PADSLASH,	SHF_PADSLASH,	CTL_PADSLASH,	ALT_PADSLASH}, /* 191 */
};

/* End of kptab[] */

MOUSE_STATUS Trapped_Mouse_status;

/*man-start**************************************************************

  PDC_get_input_fd() - Get file descriptor used for PDCurses input

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will return the file descriptor that PDCurses reads
	its input from. It can be used for WaitForMulitpleObjects().

  PDCurses Return Value:
	Returns a HANDLE.

  Portability:
	PDCurses  int PDC_get_input_fd(void);

**man-end****************************************************************/

unsigned long PDC_get_input_fd(void)
{
	PDC_LOG(("PDC_get_input_fd() - called\n"));

#ifdef PDC_THREAD_BUILD
	return (unsigned long)hSemKeyCount;
#else
	return 0L;
#endif
}

/*man-start**************************************************************

  PDC_check_bios_key()  - Check BIOS key data area for input

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will check the BIOS for any indication that
	keystrokes are pending.

  PDCurses Return Value:
	Returns 1 if a keyboard character is available, 0 otherwise.

  Portability:
	PDCurses  bool PDC_check_bios_key(void);

**man-end****************************************************************/

bool PDC_check_bios_key(void)
{
	return win32_kbhit(0);
}

/* processKeyEvent returns -1 if the key in save_ip should be ignored 
   for some reasons. Otherwise the keycode is returned which should be 
   returned by PDC_get_bios_code. save_ip MUST BE A KEY_EVENT!

   The Unicode support has been disabled. See below for the reason.
   CTRL-ALT support has been disabled, when is it emitted plainly?  */

static int processKeyEvent(void)
{
	CHAR ascii = save_ip.Event.KeyEvent.uChar.AsciiChar;
#if 0
	WCHAR unicode = save_ip.Event.KeyEvent.uChar.UnicodeChar;
#endif
	WORD vk = save_ip.Event.KeyEvent.wVirtualKeyCode;
	DWORD state = save_ip.Event.KeyEvent.dwControlKeyState;

	unsigned long local_key_modifiers = 0L;
	int idx;
	BOOL enhanced;

	pdc_key_modifiers = 0L;

	/* Must calculate the key modifiers so that Alt keys work! Save 
	   the key modifiers if required. Do this first to allow to 
	   detect e.g. a pressed CTRL key after a hit of NUMLOCK. */

	if (state & LEFT_ALT_PRESSED || state & RIGHT_ALT_PRESSED)
		local_key_modifiers |= PDC_KEY_MODIFIER_ALT;

	if (state & SHIFT_PRESSED)
		local_key_modifiers |= PDC_KEY_MODIFIER_SHIFT;

	if (state & LEFT_CTRL_PRESSED || state & RIGHT_CTRL_PRESSED)
		local_key_modifiers |= PDC_KEY_MODIFIER_CONTROL;

	if (state & NUMLOCK_ON)
		local_key_modifiers |= PDC_KEY_MODIFIER_NUMLOCK;

	pdc_key_modifiers = SP->save_key_modifiers ? local_key_modifiers : 0;


	/* Handle modifier keys hit by themselves */

	switch (vk)
	{
	case VK_SHIFT: /* shift */
		if (!SP->return_key_modifiers)
			return -1;

		return KEY_SHIFT_R;

	case VK_CONTROL: /* control */
		if (!SP->return_key_modifiers)
			return -1;

		return (state & LEFT_CTRL_PRESSED) ?
			KEY_CONTROL_L : KEY_CONTROL_R;

	case VK_MENU: /* alt */
		if (!SP->return_key_modifiers)
			return -1;

		return (state & LEFT_ALT_PRESSED) ?
			KEY_ALT_L : KEY_ALT_R;
	}

	/* The system may emit Ascii or Unicode characters depending on 
	   whether ReadConsoleInputA or ReadConsoleInputW is used. We 
	   use ReadConsoleInputA in all cases currently, so we just 
	   check Ascii. Unicode characters are very hard to implement, 
	   because our "special keys" like KEY_F(1) is one of the 
	   unicode range.

	   Now the ridiculous part of the processing. Normally, if ascii 
	   != 0 then the system did the translation successfully. But 
	   this is not true for LEFT_ALT (different to RIGHT_ALT). In 
	   case of LEFT_ALT we get we get ascii != 0. So check for this 
	   first. */

	if ((ascii != 0) && (((state & LEFT_ALT_PRESSED) == 0) ||
	    (state & RIGHT_ALT_PRESSED)))
	{
		/* This code should catch all keys returning a printable 
		   character. Characters above 0x7F should be returned 
		   as positive codes. But if'ndef NUMKEYPAD we have to 
		   return extended keycodes for keypad codes. Test for 
		   it and don't return an ascii code in case. */

#ifndef NUMKEYPAD
		if (kptab[vk].extended == 0)
#endif
			return (unsigned char) ascii;
	}

	/* This case happens if a functional key has been entered. */

	if ((state & ENHANCED_KEY) && (kptab[vk].extended != 999))
	{
		enhanced = TRUE;
		idx = kptab[vk].extended;
	}
	else
	{
		enhanced = FALSE;
		idx = vk;
	}

	if (state & SHIFT_PRESSED)
		return enhanced ? ext_kptab[idx].shift : kptab[idx].shift;

	if (state & LEFT_CTRL_PRESSED || state & RIGHT_CTRL_PRESSED)
		return enhanced ? ext_kptab[idx].control : kptab[idx].control;

	if (state & LEFT_ALT_PRESSED || state & RIGHT_ALT_PRESSED)
		return enhanced ? ext_kptab[idx].alt : kptab[idx].alt;

	return enhanced ? ext_kptab[idx].normal : kptab[idx].normal;
}


/*man-start**************************************************************

  PDC_get_bios_key() - Returns the next key available from the BIOS.

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the next key code struck at the keyboard. If the low 8
	bits are 0, the upper bits contain the extended character
	code. If bit 0-7 are non-zero, the upper bits = 0.

  PDCurses Return Value:
	See above.

  Portability:
	PDCurses  int PDC_get_bios_key(void);

**man-end****************************************************************/

int PDC_get_bios_key(void)
{
	MOUSE_STATUS Temp_Mouse_status;
	static int last_button_no = 0;
	static MOUSE_STATUS Actual_Mouse_status;
	int button_no = 0;
	bool trap_mouse = FALSE;
	int retval;

	PDC_LOG(("PDC_get_bios_key() - called\n"));

	while (1)
	{
	    win32_getch();

	    switch (save_ip.EventType)
	    {
	    case KEY_EVENT:
		retval = processKeyEvent();
		if (retval == -1)	/* ignore key? */
			continue;

		return retval;

	    case MOUSE_EVENT:
		memset((char*)&Temp_Mouse_status, 0, sizeof(MOUSE_STATUS));

		/* Wheel has been scrolled */

		if (save_ip.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
		{
		    if (save_ip.Event.MouseEvent.dwButtonState & 0xFF000000)
			Temp_Mouse_status.changes = PDC_MOUSE_WHEEL_DOWN;
		    else
			Temp_Mouse_status.changes = PDC_MOUSE_WHEEL_UP;
		}

		/* button press, release or double click */

		else if (save_ip.Event.MouseEvent.dwEventFlags == 0 ||
			 save_ip.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)
		{
		    /* Check for Left-most button - always button 1 */

		    if (save_ip.Event.MouseEvent.dwButtonState &
			FROM_LEFT_1ST_BUTTON_PRESSED && 
			!(ACTUAL_BUTTON_STATUS(1) & BUTTON_RELEASED))
		    {
			button_no = 1;

			TEMP_BUTTON_STATUS(button_no) = 
			    (save_ip.Event.MouseEvent.dwEventFlags) ?
			    BUTTON_DOUBLE_CLICKED : BUTTON_PRESSED;

			if (TEMP_BUTTON_STATUS(button_no) == 
			    BUTTON_PRESSED && (SP->_trap_mbe) &
			    BUTTON1_PRESSED)
				trap_mouse = TRUE;

			if (TEMP_BUTTON_STATUS(button_no) == 
			    BUTTON_DOUBLE_CLICKED && (SP->_trap_mbe) & 
			    BUTTON1_DOUBLE_CLICKED)
				trap_mouse = TRUE;

			break;
		    }

		    /* Check for Right-most button - always button 3 */

		    if (save_ip.Event.MouseEvent.dwButtonState & 
			RIGHTMOST_BUTTON_PRESSED && 
			!(ACTUAL_BUTTON_STATUS(3) & BUTTON_RELEASED))
		    {
			button_no = 3;

			TEMP_BUTTON_STATUS(button_no) = 
			    (save_ip.Event.MouseEvent.dwEventFlags) ? 
			    BUTTON_DOUBLE_CLICKED : BUTTON_PRESSED;

			if (TEMP_BUTTON_STATUS(button_no) == 
			    BUTTON_PRESSED && (SP->_trap_mbe) & 
			    BUTTON3_PRESSED)
				trap_mouse = TRUE;

			if (TEMP_BUTTON_STATUS(button_no) == 
			    BUTTON_DOUBLE_CLICKED && (SP->_trap_mbe) & 
			    BUTTON3_DOUBLE_CLICKED)
				trap_mouse = TRUE;

			break;
		    }

		    /* To get here we have a button release event or 
		       another button press while a current button is 
		       pressed. The latter, we throw away. We have to 
		       use the information from the previous mouse event 
		       to determine which button was released. */

		    if (last_button_no == 1 && (ACTUAL_BUTTON_STATUS(1) &
			BUTTON_PRESSED || ACTUAL_BUTTON_STATUS(1) & 
			BUTTON_DOUBLE_CLICKED || ACTUAL_MOUSE_MOVED) &&
			!(save_ip.Event.MouseEvent.dwButtonState & 
			FROM_LEFT_1ST_BUTTON_PRESSED))
		    {
			button_no = 1;
			TEMP_BUTTON_STATUS(button_no) = BUTTON_RELEASED;

			if ((SP->_trap_mbe) & BUTTON1_RELEASED)
				trap_mouse = TRUE;
			break;
		    }

		    /* Check for Right-most button - always button 3 */

		    if (last_button_no == 3 && (ACTUAL_BUTTON_STATUS(3) &
			BUTTON_PRESSED || ACTUAL_BUTTON_STATUS(3) & 
			BUTTON_DOUBLE_CLICKED || ACTUAL_MOUSE_MOVED) &&
			!(save_ip.Event.MouseEvent.dwButtonState & 
			RIGHTMOST_BUTTON_PRESSED))
		    {
			button_no = 3;
			TEMP_BUTTON_STATUS(button_no) = BUTTON_RELEASED;

			if ((SP->_trap_mbe) & BUTTON3_RELEASED)
				trap_mouse = TRUE;
			break;
		    }

		    /* Check for Middle button - button 2 only for 3 
		       button mice */

		    if (save_ip.Event.MouseEvent.dwButtonState & 
			FROM_LEFT_2ND_BUTTON_PRESSED &&
			!(ACTUAL_BUTTON_STATUS(2) & BUTTON_RELEASED))
		    {
			button_no = 2;
			TEMP_BUTTON_STATUS(button_no) = 
			    (save_ip.Event.MouseEvent.dwEventFlags) ? 
			    BUTTON_DOUBLE_CLICKED : BUTTON_PRESSED;

			if (TEMP_BUTTON_STATUS(button_no) == BUTTON_PRESSED
			    && (SP->_trap_mbe) & BUTTON2_PRESSED)
				trap_mouse = TRUE;

			if (TEMP_BUTTON_STATUS(button_no) == 
			    BUTTON_DOUBLE_CLICKED && (SP->_trap_mbe) & 
			    BUTTON2_DOUBLE_CLICKED)
				trap_mouse = TRUE;

			break;
		    }

		    if (last_button_no == 2 && (ACTUAL_BUTTON_STATUS(2) &
			BUTTON_PRESSED || ACTUAL_BUTTON_STATUS(2) & 
			BUTTON_DOUBLE_CLICKED || ACTUAL_MOUSE_MOVED) &&
			!(save_ip.Event.MouseEvent.dwButtonState & 
			FROM_LEFT_2ND_BUTTON_PRESSED))
		    {
			button_no = 2;
			TEMP_BUTTON_STATUS(button_no) = BUTTON_RELEASED;

			if ((SP->_trap_mbe) & BUTTON2_RELEASED)
				trap_mouse = TRUE;
			break;
		    }

		    /* If we get here, then we don't know how to handle 
		       the event, so dispose of it */

		    break;
		}
		else	/* button motion event */
		{
		    Temp_Mouse_status.changes |= PDC_MOUSE_MOVED;
		    button_no = last_button_no;

		    if ((button_no == 1 && SP->_trap_mbe & BUTTON1_MOVED)
		     || (button_no == 2 && SP->_trap_mbe & BUTTON2_MOVED)
		     || (button_no == 3 && SP->_trap_mbe & BUTTON3_MOVED))
			trap_mouse = TRUE;

		    break;
		}

		break;

	    case WINDOW_BUFFER_SIZE_EVENT:
		return -1;
	    }

	    if (button_no != 0)
	    {
		int key;

		/* We have a button action, rather than a mouse movement 
		   or wheel action */

		TEMP_MOUSE_X_POS = save_ip.Event.MouseEvent.dwMousePosition.X;
		TEMP_MOUSE_Y_POS = save_ip.Event.MouseEvent.dwMousePosition.Y;

		/* First thing is to check if the mouse has been clicked 
		   on a slk area. If the return value is > 0 (indicating 
		   the label number), return with the KEY_F(key) value. 
		   Only call this if we have set trap_mouse to TRUE above. */

		if (trap_mouse && (key = PDC_mouse_in_slk(TEMP_MOUSE_Y_POS,
		    TEMP_MOUSE_X_POS)) != 0)
			return KEY_F(key);

		Temp_Mouse_status.changes |= 1 << (button_no - 1);

		if (save_ip.Event.MouseEvent.dwControlKeyState &
		    SHIFT_PRESSED)
			TEMP_BUTTON_STATUS(button_no) |= BUTTON_SHIFT;

		if (save_ip.Event.MouseEvent.dwControlKeyState & 
		    LEFT_CTRL_PRESSED)
			TEMP_BUTTON_STATUS(button_no) |= BUTTON_CONTROL;

		if (save_ip.Event.MouseEvent.dwControlKeyState & 
		    RIGHT_CTRL_PRESSED)
			TEMP_BUTTON_STATUS(button_no) |= BUTTON_CONTROL;

		if (save_ip.Event.MouseEvent.dwControlKeyState & 
		    RIGHT_ALT_PRESSED)
			TEMP_BUTTON_STATUS(button_no) |= BUTTON_ALT;

		if (save_ip.Event.MouseEvent.dwControlKeyState & 
		    LEFT_ALT_PRESSED)
			TEMP_BUTTON_STATUS(button_no) |= BUTTON_ALT;

		last_button_no = button_no;

		/* We now have the current mouse status information
		   for the last Mouse event.  We need to save this in
		   Actual_Mouse_status so we can use that when comparing
		   against the next mouse event. We also need to
		   determine if we need to set Trapped_Mouse_status
		   based on the settings in SP->_trap_mbe. */

		memcpy((char*)&Actual_Mouse_status,
			(char*)&Temp_Mouse_status, sizeof(MOUSE_STATUS));

		if (trap_mouse)
			 break;
	    }

	    if ((Temp_Mouse_status.changes & PDC_MOUSE_WHEEL_DOWN ||
		Temp_Mouse_status.changes & PDC_MOUSE_WHEEL_UP) &&
		SP->_trap_mbe & MOUSE_WHEEL_SCROLL)
	    {
		TEMP_MOUSE_X_POS = -1;
		TEMP_MOUSE_Y_POS = -1;
		break;
	    }
	}

	/* To get here we have a mouse event that has been trapped by 
	   the user. Save it in the Trapped_Mouse_status structure. */

	memcpy((char*)&Trapped_Mouse_status, (char*)&Temp_Mouse_status, 
		sizeof(MOUSE_STATUS));

	return KEY_MOUSE;
}

/*man-start**************************************************************

  PDC_get_ctrl_break()  - return OS control break state

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the current OS Control Break Check state.

  PDCurses Return Value:
	This function returns TRUE if the Control Break Check is 
	enabled; otherwise, FALSE is returned.

  Portability:
	PDCurses  bool PDC_get_ctrl_break(void);

**man-end****************************************************************/

bool PDC_get_ctrl_break(void)
{
	PDC_LOG(("PDC_get_ctrl_break() - called\n"));

	return FALSE;
}

/*man-start**************************************************************

  PDC_set_ctrl_break()  - Enables/Disables the host OS BREAK key check.

  PDCurses Description:
	This is a private PDCurses routine.

	Enables/Disables the host OS BREAK key check. If the supplied 
	setting is TRUE, this enables CTRL/C and CTRL/BREAK to abort the 
	process. If FALSE, CTRL/C and CTRL/BREAK are ignored.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_set_ctrl_break(bool setting);

**man-end****************************************************************/

int PDC_set_ctrl_break(bool setting)
{
	PDC_LOG(("PDC_set_ctrl_break() - called\n"));

	return OK;
}

/*man-start**************************************************************

  PDC_validchar() - validate/translate passed character

  PDCurses Description:
	This is a private PDCurses function.

	Checks that 'c' is a valid character, and if so returns it,
	with function key translation applied if 'w' has keypad mode
	set.  If char is invalid, returns -1.

  PDCurses Return Value:
	This function returns -1 if the passed character is invalid, or
	the WINDOW *_getch_win_ is NULL, or _getch_win_'s keypad is not 
	active.

	Otherwise, this function returns the PDCurses equivalent of the
	passed character.  See the function key and key macros in
	<curses.h>.

  Portability:
	PDCurses  int PDC_validchar(int c);

**man-end****************************************************************/

int PDC_validchar(int c)
{
	PDC_LOG(("PDC_validchar() - called\n"));

	/* skip special keys if !keypad mode */

	if ((_getch_win_ == (WINDOW *)NULL) ||
	    ((unsigned int)c >= 256 && !_getch_win_->_use_keypad))
		c = -1;

	PDC_LOG(("PDC_validchar() - returned: %x\n", c));

	return c;
}

/* GetInterestingEvent returns 0 if *ip doesn't contain an event which
   should be passed back to the user. This function filters "useless"
   events.

   The function returns the number of events waiting. This may be > 1
   if the repeation of real keys pressed so far are > 1.

   Keyboard: Returns 0 on NUMLOCK, CAPSLOCK, SCROLLLOCK.

	     Returns 1 for SHIFT, ALT, CTRL only if no other key has been
	     pressed in between; these are returned on keyup in opposite 
	     to normal keys. The overall flags for processing of SHIFT, ALT,
	     CTRL (SP->return_key_modifiers) must have been set.

	     FGC: CHANGED BEHAVIOUR: In previous version SHIFT etc had a
	     chance to be returned on the first keydown, too. This was a 
	     bug, because return_key_modifiers were 0.

	     Normal keys are returned on keydown only. The number of 
	     repetitions are returned. Dead keys (diacritics) are 
	     omitted. See below for a description.

	     The keypad entering of special keys is not supported. This 
	     feature can be built in by replacing "#ifdef NUMPAD_CHARS" 
	     with an intelligent code.

   Mouse:    Returns > 0 only if SP->_trap_mbe is set. MOUSE_MOVE without
	     a pressed mouse key are ignored.

   THIS FUNCTION IS NOT THREAD-SAFE. NEVER USE MORE THREADS THAN TO
   USE THIS FUNCTION. STATIC VARIABLES ARE USED HERE.
*/

static int GetInterestingEvent(INPUT_RECORD *ip)
{
	int numKeys = 0, vk;
	static int save_press;
	static unsigned numpadChar = 0;

#ifdef PDCDEBUG
# ifdef PDC_THREAD_BUILD
#  define PDC_DEBUG_THREADING1 "-->"
#  define PDC_DEBUG_THREADING2 "THREADING"
# else
#  define PDC_DEBUG_THREADING1 ""
#  define PDC_DEBUG_THREADING2 ""
# endif
	char *ptr = "";

	PDC_LOG(("%sGetInterestingEvent(%s) - called\n",
		PDC_DEBUG_THREADING1, PDC_DEBUG_THREADING2));

# define PTR(x) ptr = x
#else
# define PTR(x)
#endif

	switch(ip->EventType)
	{
	case KEY_EVENT:
	    vk = ip->Event.KeyEvent.wVirtualKeyCode;

	    /* throw away some modifiers */

	    if (vk == VK_CAPITAL || vk == VK_NUMLOCK || vk == VK_SCROLL)
	    {
		PTR("KEY MODIFIERS");

		numpadChar = 0;
		save_press = 0;
		break;
	    }

	    if (ip->Event.KeyEvent.bKeyDown == FALSE)
	    {
		/* key up, the following check for VK_??? is paranoid 
		   hopefully */

		if ((vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU)
		    && vk == save_press && SP->return_key_modifiers)
		{
		    PTR("KEYUP WANTED");

		    /* Fall through and return this key. Still have to 
		       check the dead key condition. */

		    /* always limited */

		    ip->Event.KeyEvent.wRepeatCount = 1;
		}
		else if (vk == VK_MENU && numpadChar)
		{
		    ip->Event.KeyEvent.uChar.AsciiChar =
			(CHAR) (unsigned char) numpadChar;

		    ip->Event.KeyEvent.dwControlKeyState &= 
			~LEFT_ALT_PRESSED;

		    ip->Event.KeyEvent.wRepeatCount = 1; /* always limited */

		    /* change ALT to something else */

		    ip->Event.KeyEvent.wVirtualKeyCode = VK_NUMPAD0;

		    numpadChar = 0;
		}
		else
		{
		    PTR("KEYUP IGNORED");
		    break;		/* throw away other KeyUp events */
		}
	    }
	    else
	    {
		if (vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU)
		{
		    /* These keys are returned on keyup only. */

		    save_press = (SP->return_key_modifiers) ? vk : 0;
		    numpadChar = 0;

		    PTR("KEYDOWN SAVED");
		    break;	/* throw away key press */
		}

#ifdef NUMPAD_CHARS
		if ((ip->Event.KeyEvent.dwControlKeyState & 
		    (LEFT_ALT_PRESSED | ENHANCED_KEY)) == LEFT_ALT_PRESSED)
		{
		    switch (vk)
		    {
		    case VK_CLEAR:
			vk = VK_NUMPAD5;
			break;
		    case VK_PRIOR:
			vk = VK_NUMPAD9;
			break;
		    case VK_NEXT:
			vk = VK_NUMPAD3;
			break;
		    case VK_END:
			vk = VK_NUMPAD1;
			break;
		    case VK_HOME:
			vk = VK_NUMPAD7;
			break;
		    case VK_LEFT:
			vk = VK_NUMPAD4;
			break;
		    case VK_UP:
			vk = VK_NUMPAD8;
			break;
		    case VK_RIGHT:
			vk = VK_NUMPAD6;
			break;
		    case VK_DOWN:
			vk = VK_NUMPAD2;
			break;
		    case VK_INSERT:
			vk = VK_NUMPAD0;
		    }
               
		}

		if ((vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) &&
                    ip->Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED)
		{
		    PTR("NUMPAD ALTERNATE INPUT");

		    numpadChar *= 10;
		    numpadChar += vk - VK_NUMPAD0;
		    break;
		}
		else
		    numpadChar = 0;

#else /* NUMPAD_CHARS */
		numpadChar = 0;
#endif
	    }

	    save_press = 0;

	    /* Check for diacritics. These are dead keys. Some locale 
	       have modified characters like umlaut-a, which is an "a" 
	       with two dots on it. In some locales you have to press a 
	       special key (the dead key) immediately followed by the 
	       "a" to get a composed umlaut-a. The special key may have 
	       a normal meaning with different modifiers. */

	    if (ip->Event.KeyEvent.uChar.AsciiChar == 0 &&
		(MapVirtualKey(ip->Event.KeyEvent.wVirtualKeyCode, 2) &
		0x80000000))
	    {
		PTR("DIACRITIC IGNORED");
		break;		/* Diacritic characters, ignore them */
	    }

	    PTR("KEY WANTED");

	    numKeys = ip->Event.KeyEvent.wRepeatCount;
	    break;

	case MOUSE_EVENT:
	    /* If we aren't trapping mouse events, then the "keyboard" 
	       hasn't been hit. Fix from stepheng@clearspeed.com */

	    if (!SP->_trap_mbe)
	    {
		PTR("MOUSE - NOT TRAPPED");
		break;
	    }

	    if (ip->Event.MouseEvent.dwEventFlags == MS_MOUSE_MOVED
		&& ip->Event.MouseEvent.dwButtonState == 0)
	    {
		PTR("MOUSE MOVE IGNORED");
		break;		/* throw away plain MOUSE_MOVE events */
	    }

	    PTR("MOUSE MOVE WANTED");
	    numKeys = 1;
	    break;

	default:
	    PTR("UNKNOWN");
	    break;
	}

#undef PTR

	PDC_LOG(("%sGetInterestingEvent(%s) - returning: numKeys %d "
		"type %d: %s\n", PDC_DEBUG_THREADING1, 
		PDC_DEBUG_THREADING2, numKeys, ip->EventType, ptr));

	return numKeys;
}

#ifdef PDC_THREAD_BUILD

static int win32_kbhit(int timeout)
{
	DWORD read = 0, avail = 0, unread = 0;
	INPUT_RECORD ip;

	PDC_LOG(("win32_kbhit(THREADING) - called: timeout %d\n", 
		timeout));

	if (WaitForSingleObject(hSemKeyCount, timeout) != WAIT_OBJECT_0)
		return FALSE;

	if (timeout == INFINITE)
	{
		ReadFile(hPipeRead, &save_ip, sizeof(INPUT_RECORD), 
			&read, NULL);
		return TRUE;
	}
	else
	{
		if (PeekNamedPipe(hPipeRead, &ip, sizeof(INPUT_RECORD), 
		    &read, &avail, &unread))
		{
			PDC_LOG(("win32_kbhit(THREADING) - maybe key on "
				"pipe. read %d avail %d unread %d\n",
				read, avail, unread));

			if (read == sizeof(INPUT_RECORD))
				return TRUE;
		}
	}

	return FALSE;
}

static void win32_getch(void)
{
	while (win32_kbhit(INFINITE) == FALSE)
	;
}

#else	/* PDC_THREAD_BUILD */

static int win32_kbhit(int timeout)
{
	INPUT_RECORD ip;
	DWORD read;
	int rc = FALSE;

	PDC_LOG(("win32_kbhit() - called: timeout %d keyCount %d\n",
		timeout, keyCount));

	if (keyCount > 0)
		return TRUE;

	if (WaitForSingleObject(hConIn, timeout) != WAIT_OBJECT_0)
		return FALSE;

	PeekConsoleInput(hConIn, &ip, 1, &read);
	if (read == 0)
		return FALSE;

	ReadConsoleInput(hConIn, &ip, 1, &read);

	keyCount = GetInterestingEvent(&ip);
	if (keyCount)
	{
		/* To get here a recognised event has occurred; save it 
		   and return TRUE */

		memcpy((char*)&save_ip, (char*)&ip, sizeof(INPUT_RECORD));
		rc = TRUE;
	}

	return rc;
}

static void win32_getch(void)
{
	while (win32_kbhit(INFINITE) == FALSE)
	;

	keyCount--;
}

#endif	/* PDC_THREAD_BUILD */

/*man-start**************************************************************

  PDC_flushinp()		- Low-level input flush

  PDCurses Description:
	This is a private PDCurses routine.

	Discards any pending keyboard and mouse input. Called by 
	flushinp().

  Portability:
	PDCurses  void PDC_flushinp(void);

**man-end****************************************************************/

void PDC_flushinp(void)
{
	PDC_LOG(("PDC_flushinp() - called\n"));

	FlushConsoleInputBuffer(hConIn);
}

#ifdef PDC_THREAD_BUILD

LONG InputThread(LPVOID lpThreadData)
{
	INPUT_RECORD ip;
	DWORD read;
	LONG prev;
	int num_keys, i;

	PDC_LOG(("-->InputThread() - called\n"));

	/* Create a semaphore on which the parent thread will wait */

	hSemKeyCount = CreateSemaphore(NULL, 0, 1024, NULL);
	if (hSemKeyCount != NULL)
	{
	    for (;;)
	    {
		ReadConsoleInput(hConIn, &ip, 1, &read);

		/* Now that this thread has woken up we need to check if 
		   the key/mouse event is relevant to us. If so, write 
		   it to the anonymous pipe. */

		PDC_LOG(("-->InputThread() - read %d character(s)\n",
			read));

		num_keys = GetInterestingEvent(&ip);

		PDC_LOG(("-->InputThread() - got %d interesting keys\n",
			num_keys));

		/* For each key written to the pipe, increment the 
		   semaphore */

		for (i = 0; i < num_keys; i++)
		    if (ReleaseSemaphore(hSemKeyCount, 1, &prev))
		    {
			PDC_LOG(("-->InputThread() - writing to pipe; "
				"sem incremented from %d\n", prev));

			/* If an error occured, we assume it is because 
			   the pipe broke; therefore the parent thread 
			   is shutting down; so will we. */

			if (!WriteFile(hPipeWrite, &ip, sizeof(INPUT_RECORD),
			    &read, NULL))
				/* break; */
				return 0;		/* ? */
		    }
	    }
	}

	PDC_LOG(("-->InputThread() - finished\n"));

	return 0;
}

#endif
