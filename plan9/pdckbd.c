#include <curspriv.h>
#include "pdcplan9.h"


unsigned long pdc_key_modifiers = 0L;


bool PDC_check_key(void)
{
	PDC_LOG(("PDC_check_key() - called\n"));
	return p9ecan();
}


int PDC_get_key(void)
{
	PDC_LOG(("PDC_check_key() - called\n"));
	return p9eget();
}


void PDC_flushinp(void)
{
	PDC_LOG(("PDC_flushinp() - called\n"));
}


int PDC_modifiers_set(void)
{
	PDC_LOG(("PDC_modifiers_set() - called\n"));
	return OK;
}


int PDC_mouse_set(void)
{
	PDC_LOG(("PDC_mouse_set() - called\n"));
	return OK;
}


void PDC_set_keyboard_binary(bool)
{
	PDC_LOG(("PDC_mouse_set() - called\n"));
}


unsigned long PDC_get_input_fd(void)
{
	PDC_LOG(("PDC_get_input_fd() - called\n"));
	return -1;
}
