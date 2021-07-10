#include <curspriv.h>
#include <sys/time.h>
#include "pdcplan9.h"


void PDC_beep(void)
{
	PDC_LOG(("PDC_beep() - called\n"));
}


void PDC_napms(int ms)
{
	PDC_LOG(("PDC_napms() - called\n"));
	p9napms(ms);
}


const char *PDC_sysname(void)
{
	PDC_LOG(("PDC_sysname() - called\n"));

	return "PLAN9";
}
