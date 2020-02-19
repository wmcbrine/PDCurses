/* PDCurses */

#include "pdcdos.h"

#include <stdlib.h>

/* return width of screen/viewport */

int PDC_get_columns(void)
{
    int cols;

    PDC_LOG(("PDC_get_columns() - called\n"));

    cols = PDC_state.video_width / 8;

    PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

    return cols;
}

/* get the cursor size/shape */

int PDC_get_cursor_mode(void)
{
    PDC_LOG(("PDC_get_cursor_mode() - called\n"));

    int start = _FONT16*3/4;
    int end   = _FONT16-1;

    return (start << 4) | end;
}

/* return number of screen rows */

int PDC_get_rows(void)
{
    int rows;

    PDC_LOG(("PDC_get_rows() - called\n"));

    rows = PDC_state.video_height / _FONT16;

    PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

    return rows;
}
