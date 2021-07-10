#include <curspriv.h>
#include "pdcplan9.h"

int PDC_get_columns(void)
{
	int i;

	i = p9getcols();
	PDC_LOG(("PDC_get_columns() returns: %d\n", i));
	return i;
}


int PDC_get_cursor_mode(void)
{
	PDC_LOG(("PDC_get_cursor_mode() - called\n"));
	return 0;
}


int PDC_get_rows(void)
{
	int i;

	i = p9getrows();
	PDC_LOG(("PDC_get_rows() - returns: %d\n", i));
	return i;
}
