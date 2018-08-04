
extern int PDC_rows, PDC_cols;

int PDC_get_columns(void)
{
   return( PDC_cols);
}

int PDC_get_cursor_mode(void)
{
   return( 0);    /* no cursor mode setting yet */
}

int PDC_get_rows(void)
{
   return( PDC_rows);
}
