/* Public Domain Curses */

#include <curspriv.h>

int PDC_get_cursor_mode(void)
{
    PDC_LOG(("PDC_get_cursor_mode() - called\n"));

    return SP->visibility;
}


int PDC_get_columns(void)
{
    extern int PDC_cols;

    PDC_LOG(("PDC_get_columns() - called\n"));
    return( PDC_cols);
}

int PDC_get_rows(void)
{
    extern int PDC_rows;

    PDC_LOG(("PDC_get_rows() - called\n"));
    return( PDC_rows);
}
