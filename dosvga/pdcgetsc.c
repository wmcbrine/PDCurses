/* PDCurses */

#include "pdcdos.h"

#include <stdlib.h>

/* return width of screen/viewport */

int PDC_get_columns(void)
{
    int cols;

    PDC_LOG(("PDC_get_columns() - called\n"));

    cols = PDC_state.video_width / PDC_state.font_width;

    PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

    return cols;
}

/* get the cursor size/shape */

int PDC_get_cursor_mode(void)
{
    int start = PDC_state.font_height * 3 / 4;
    int end   = PDC_state.font_height - 1;

    PDC_LOG(("PDC_get_cursor_mode() - called\n"));

    return (start << 8) | end;
}

/* return number of screen rows */

int PDC_get_rows(void)
{
    int rows;

    PDC_LOG(("PDC_get_rows() - called\n"));

    rows = PDC_state.video_height / PDC_state.font_height;

    PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

    return rows;
}
