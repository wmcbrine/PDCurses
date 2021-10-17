#include <curspriv.h>
#include "pdcplan9.h"


int PDC_curs_set(int vis)
{
	int rvis;
	chtype *ch;

	PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", vis));
	rvis = SP->visibility;
	p9setcur(SP->cursrow, SP->curscol, SP->cursrow, SP->curscol, vis);
	SP->visibility = vis;

	return rvis;
}


void PDC_set_title(const char *title)
{
	PDC_LOG(("PDC_set_title() - called:<%s>\n", title));
	p9setlabel(title);
}


int PDC_set_blink(bool blinkon)
{
	PDC_LOG(("PDC_set_title() - called:<%d>\n", blinkon));
	if (SP->color_started)
		COLORS = 16;

	return blinkon ? ERR : OK;
}

int PDC_set_bold(bool boldon)
{
    if (!SP)
        return ERR;

#ifdef PDC_WIDE
    if (boldon)
        SP->termattrs |= A_BOLD;
    else
        SP->termattrs &= ~A_BOLD;

    return OK;
#else
    return boldon ? ERR : OK;
#endif
}

