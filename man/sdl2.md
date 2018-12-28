SDL2 Considerations
===================

Most of the information in the [SDL Considerations](sdl.md) document is also 
valid for the SDL2 platform.  This document describes the differences.


Integration with SDL2
---------------------

As mentioned in the [SDL Considerations](sdl.md) document, the SDL port uses
the following variables:

    SDL_Surface *pdc_screen, *pdc_font, *pdc_icon, *pdc_back;
    int pdc_sheight, pdc_swidth, pdc_yoffset, pdc_xoffset;

The SDL2 port adds:

    SDL_Window *pdc_window;

These can be used or modified in your own code.  Like the `pdc_screen` variable
used by the SDL port, you can initialize `pdc_window` in your own code.  See the
sdltest demo for an example.  If it's not initialized, PDCurses will do it for
you.  If PDCurses initializes `pdc_window`, it will clean up/deallocate these
variables on exit.


Fonts
-----

If the `PDC_WIDE` flag is set during compilation, SDL2_ttf is used to render
fonts.  This allows for use of a full UCS-2 (16 bit Unicode) character set.

Using SDL2_ttf adds:

    TTF_Font *pdc_ttffont;
    int pdc_font_size;

The default TrueType font can be modified by setting a define during compile
time (e.g. `-DPDC_FONT_PATH=/usr/share/fonts/truetype/dejavu/`). This can be
overridden by initializing `pdc_ttffont` in your own code.  Similar
to the `PDC_FONT` environment variable for bitmap fonts, one can also override
TTF fonts using the `PDC_FONT` environment variables.  Unless modified during
compile time, the defaults are:

    Windows: C:/Windows/Fonts/lucon.ttf
    OS X:    /Library/Fonts/Courier New.ttf
    Other:   /usr/share/fonts/truetype/freefont/FreeMono.ttf

Font size can be set using the `PDC_FONT_SIZE` environment variable or by
setting the value of `pdc_font_size` in your own code.  The default size is 18.


Clipboard
---------

SDL2 port uses functionality provided by the SDL2 clipboard functions to add
basic clipboard support.
