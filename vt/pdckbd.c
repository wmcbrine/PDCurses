#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef _WIN32
   #include <conio.h>
#else
   #include <sys/select.h>
   #include <unistd.h>
#endif
#include "curspriv.h"

/* Modified from the accepted answer at

https://stackoverflow.com/questions/33025599/move-the-cursor-in-a-c-program

_kbhit( ) returns -1 if no key has been hit,  and the keycode if one
has been hit.  You can just loop on it until the return value is >= 0.
Hitting a function or arrow or similar key will cause 27 (escape) to
be returned,  followed by cryptic codes that depend on what terminal
emulation is in place.

   Further info on VT100/ANSI control sequences is at

https://www.gnu.org/software/screen/manual/html_node/Control-Sequences.html
*/

extern bool PDC_resize_occurred;

static bool check_key( int *c)
{
    bool rval;
#ifndef _WIN32
    const int STDIN = 0;
    struct timeval timeout;
    fd_set rdset;

    if( PDC_resize_occurred)
       return( TRUE);
    FD_ZERO( &rdset);
    FD_SET( STDIN, &rdset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if( select( STDIN + 1, &rdset, NULL, NULL, &timeout) > 0)
#else
    if( kbhit( ))
#endif
       {
       rval = TRUE;
       if( c)
          *c = getchar( );
       }
    else
       rval = FALSE;
    return( rval);
}

bool PDC_check_key( void)
{
   return( check_key( NULL));
}

void PDC_flushinp( void)
{
   int thrown_away_char;

   while( check_key( &thrown_away_char))
      ;
}

/* Mouse events include six bytes.  First three are

ESC [ M

   Next byte is 96 for mouse wheel up,  97 for down,  or (for more
"traditional" mouse events) 32 plus :

   0 for button 1
   1 for button 2
   2 for button 3
   3 for release
   4 if Shift is pressed
   8 if Alt (Meta) is pressed
   16 if Ctrl is pressed

   Note that 'release' doesn't tell you _which_ is released.  If only
one has been pressed (the usual case),  it's presumably the one you
released.  If two or more buttons are pressed simultaneously,  the
"releases" are reported in the numerical order of the buttons,  not
the order in which they're actually released (which we don't know).

   My tilt mouse reports 'tilt left' as a left button (1) and 'tilt right'
as a middle button press.  Wheel events get shift,  alt,  ctrl added in
(but that doesn't seem to be getting through in PDCurses... to be fixed).
Button events only get Ctrl (though I think you might get the other events
on some terminals).

   "Correct" mouse handling will require that we detect a button-down,
then hold off for SP->mouse_wait to see if we get a release event.  */

static int xlate_vt_codes( const int *c, const int count)
{
   static const int tbl[] =  {
               KEY_UP,   2, '[', 'A',
               KEY_DOWN, 2, '[', 'B',
               KEY_LEFT, 2, '[', 'D',
               KEY_RIGHT,2, '[', 'C',
               KEY_HOME, 2, 'O', 'H',
               KEY_END,  2, 'O', 'F',
               KEY_IC,   3, '[', '2', '~',
               KEY_DC,   3, '[', '3', '~',
               KEY_PPAGE, 3, '[', '5', '~',
               KEY_NPAGE, 3, '[', '6', '~',

               CTL_LEFT,  5, '[', '1', ';', '5', 'D',
               CTL_RIGHT, 5, '[', '1', ';', '5', 'C',
               CTL_UP,    5, '[', '1', ';', '5', 'A',
               CTL_DOWN,  5, '[', '1', ';', '5', 'B',

               ALT_PGUP, 5, '[', '5', ';', '3', '~',
               ALT_PGDN, 5, '[', '6', ';', '3', '~',

               KEY_F(1), 2, 'O', 80,
               KEY_F(1), 4, '[', '1', '1', '~',
               KEY_F(2), 2, 'O', 81,
               KEY_F(2), 4, '[', '1', '2', '~',
               KEY_F(3), 2, 'O', 82,
               KEY_F(3), 4, '[', '1', '3', '~',
               KEY_F(4), 2, 'O', 83,
               KEY_F(4), 4, '[', '1', '4', '~',
               KEY_F(5), 4, '[', '1', '5', '~',
               KEY_F(6), 4, '[', '1', '7', '~',
               KEY_F(7), 4, '[', '1', '8', '~',
               KEY_F(8), 4, '[', '1', '9', '~',
               KEY_F(9), 4, '[', '2', '0', '~',
               KEY_F(10), 4, '[', '2', '1', '~',
/* doesn't go  KEY_F(11), 4, '[', '2', '3', '~', */
               KEY_F(12), 4, '[', '2', '4', '~',
               27, 0,
               0 };
   int i, rval = -1;
   const int *tptr;

   if( count == 1)
      {
      if( c[0] >= 'a' && c[0] <= 'z')
         rval = ALT_A + c[0] - 'a';
      if( c[0] >= '0' && c[0] <= '9')
         rval = ALT_0 + c[0] - '0';
      }
   else if( count == 5 && c[0] == '[' && c[1] == 'M')
      rval = KEY_MOUSE;
   for( tptr = tbl; rval == -1 && *tptr; tptr += 2 + tptr[1])
      if( count == tptr[1])
         {
         i = 0;
         while( tptr[i + 2] == c[i] && i < count)
            i++;
         if( i == count)
            rval = tptr[0];
         }
   return( rval);
}

int PDC_get_key( void)
{
   int rval = -1;

   if( PDC_resize_occurred)
      {
      PDC_resize_occurred = FALSE;
      return( KEY_RESIZE);
      }
   if( check_key( &rval))
      {
      int c[13], count = 0;

      SP->key_code = (rval == 27);
      if( rval == 27)
         {
         while( count < 5 && check_key( &c[count])
                  && (rval = xlate_vt_codes( c, count + 1)) == -1)
            count++;
         if( rval == KEY_MOUSE)
            {
            int idx = (c[2] & 3);
            const bool release = (idx == 3);
            static int held = 0;

            if( release)        /* it's a release */
               {
               idx = 0;
               while( idx < 3 && !((held >> idx) & 1))
                  idx++;
               held ^= (1 << idx);
               }
            if( idx < 3)
               {
               int flags = 0, i;

               memset(&pdc_mouse_status, 0, sizeof(MOUSE_STATUS));
               pdc_mouse_status.button[idx] =
                              (release ? BUTTON_RELEASED : BUTTON_PRESSED);
               if( c[2] & 64)    /* actually mouse wheel event */
                  pdc_mouse_status.changes =
                        (idx ? PDC_MOUSE_WHEEL_DOWN : PDC_MOUSE_WHEEL_UP);
               else     /* "normal" mouse button */
                  pdc_mouse_status.changes = (1 << idx);
               pdc_mouse_status.x = c[3] - ' ' - 1;
               pdc_mouse_status.y = c[4] - ' ' - 1;
               if( c[2] & 4)
                  flags |= PDC_BUTTON_SHIFT;
               if( c[2] & 8)
                  flags |= PDC_BUTTON_ALT;
               if( c[2] & 16)
                  flags |= PDC_BUTTON_CONTROL;
               for( i = 0; i < 3; i++)
                  pdc_mouse_status.button[i] |= flags;
               if( !release)     /* wait for a possible release */
                  {
                  PDC_napms( SP->mouse_wait);

                  if( check_key( c))      /* assume it's a release */
                     {
                     count = 0;
                     while( count < 5 && check_key( &c[count]))
                        count++;
                     pdc_mouse_status.button[idx] = BUTTON_CLICKED | flags;
                     }
                  else
                     held ^= (1 << idx);
                  }
               }
            }
         }
      else if( (rval & 0xc0) == 0xc0)      /* start of UTF-8 */
         {
         check_key( &c[0]);
         assert( (c[0] & 0xc0) == 0x80);
         c[0] &= 0x3f;
         if( !(rval & 0x20))      /* two-byte : U+0080 to U+07ff */
            rval = c[0] | ((rval & 0x1f) << 6);
         else if( !(rval & 0x10))   /* three-byte : U+0800 - U+ffff */
            {
            check_key( &c[1]);
            assert( (c[1] & 0xc0) == 0x80);
            c[1] &= 0x3f;
            rval = (c[1] | (c[0] << 6) | ((rval & 0xf) << 12));
            }
            /* Else... four-byte SMP char */
         }
      else if( rval == 127)
         rval = 8;
      }
   return( rval);
}

int PDC_modifiers_set( void)
{
   return( OK);
}

int PDC_mouse_set( void)
{
   return(  OK);
}

void PDC_set_keyboard_binary( bool on)
{
   return;
}

unsigned long PDC_get_input_fd( void)
{
   return( 0);
}

