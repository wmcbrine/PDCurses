#include <curspriv.h>
#include "pdcplan9.h"

#ifdef CHTYPE_LONG
# define A(x) ((chtype)x | A_ALTCHARSET)
chtype acs_map[128] = {
	A(0), A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9),
	A(10), A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18),
	A(19), A(20), A(21), A(22), A(23), A(24), A(25), A(26), A(27),
	A(28), A(29), A(30), A(31), ' ', '!', '"', '#', '$', '%', '&',
	'\'', '(', ')', '*',

	0x2192, 0x2190, 0x2191, 0x2193,

	'/',

	0x2588,

	'1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
	'>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

	0x25c6, 0x2592,

	'b', 'c', 'd', 'e',

	0x00ba, 0x00b1, 0x0023, 0x0023, 0x2518, 0x2510, 0x250c, 0x2514,
	0x253c, 'o',
	'p', 0x2500, 'r', 's', 0x2524, 0x251c, 0x2534, 0x252c, 0x2502,
	0x2264, 0x2265, 0x03c0, 0x2260, 0x00a3, 0x2022,

	A(127)
};

# undef A
#endif


void PDC_gotoyx(int y, int x)
{
	chtype *ch;

	PDC_LOG(("PDC_gotoyx() - called: y %d x %d\n", y, x));
	p9setcur(SP->cursrow, SP->curscol, y, x, SP->visibility);
}


void PDC_transform_line(int y, int x, int len, const chtype * srcp)
{
	int i;

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));
	for (i = 0; i < len; i++)
	{
		p9putc(y, x + i, srcp[i]);
	}
}
