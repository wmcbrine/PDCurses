/* Public Domain Curses */

#include <curspriv.h>
#include <term.h>

/*man-start**************************************************************

  Name:                                                         delphicompat

  Synopsis:
        int          DC_GetLines(void);
        int          DC_GetCols(void);
        WINDOW       *DC_GetStdscr(void);
        WINDOW       *DC_GetCurscr(void);
        void         DC_SetCurscr(WINDOW *);
        SCREEN       *DC_GetSP(void);
        void         DC_SetSP(SCREEN *);
        MOUSE_STATUS DC_GetMouse_status(void);
        void         DC_SetMouse_status(MOUSE_STATUS);
        int          DC_GetColors(void);
        int          DC_GetColor_Pairs(void);
        int          DC_GetTabsize(void);
        void         DC_SetTabsize(int);
        chtype       *DC_GetAcs_map(void);
        char         *DC_GetTtytype(void);
        TERMINAL     *DC_GetCur_Term(void);

  Description:
        DC_GetLines() is used to retrieve the LINES variable.
        
        DC_GetCols() is used to retrieve the COLS variable.
        
        DC_GetStdscr() is used to retrieve the stdscr variable.
        
        DC_GetCurscr() and DC_SetCurscr() are used to set and retrieve the curscr variable.
        
        DC_GetSP() and DC_SetSP() are used to set and retrieve the SP variable.
        
        DC_GetMouse_status() and DC_SetMouse_status() are used to set and retrieve the Mouse_Status variable.
        
        DC_GetColors() is used to retrieve the COLORS variable.
        
        DC_GetColor_Pairs() is used to retrieve the COLOR_PAIRS variable.
        
        DC_GetTabsize() and DC_SetTabsize() are used to set and retrieve the TABSIZE variable.
        
        DC_GetAcs_map() is used to retrieve the acs_map variable.
        
        DC_GetTtytype() is used to retrieve the ttytype variable.
        
        DC_GetCur_Term() is used to retrieve the current terminal.

  Portability                                X/Open    BSD    SYS V
        DC_GetLines                             Y       Y       Y
        DC_GetCols                              Y       Y       Y
        DC_GetStdscr                            Y       Y       Y
        DC_GetCurscr                            Y       Y       Y
        DC_SetCurscr                            Y       Y       Y
        DC_GetSP                                Y       Y       Y
        DC_SetSP                                Y       Y       Y
        DC_GetMouse_status                      Y       Y       Y
        DC_SetMouse_status                      Y       Y       Y
        DC_GetColors                            Y       Y       Y
        DC_GetColor_Pairs                       Y       Y       Y
        DC_GetTabsize                           Y       Y       Y
        DC_SetTabsize                           Y       Y       Y
        DC_GetAcs_map                           Y       Y       Y
        DC_GetTtytype                           Y       Y       Y
        DC_GetCur_Term                          Y       Y       Y

**man-end****************************************************************/

int DC_GetLines(void)
{
    return LINES;
}

int DC_GetCols(void)
{
    return COLS;
}

WINDOW *DC_GetStdscr(void)
{
    return stdscr;
}

WINDOW *DC_GetCurscr(void)
{
    return curscr;
}

void DC_SetCurscr(WINDOW *val)
{
    curscr = val;
}

SCREEN *DC_GetSP(void)
{
    return SP;
}

void DC_SetSP(SCREEN *val)
{
    SP = val;
}

MOUSE_STATUS DC_GetMouse_status(void)
{
    return Mouse_status;
}

void DC_SetMouse_status(MOUSE_STATUS val)
{
    Mouse_status = val;
}

int DC_GetColors(void)
{
    return COLORS;
}

int DC_GetColor_Pairs(void)
{
    return COLOR_PAIRS;
}

int DC_GetTabsize(void)
{
    return TABSIZE;
}

void DC_SetTabsize(int val)
{
    TABSIZE = val;
}

chtype *DC_GetAcs_map(void)
{
    return acs_map;
}

char *DC_GetTtytype(void)
{
    return ttytype;
}

TERMINAL *DC_GetCur_Term(void)
{
    return cur_term;
}
