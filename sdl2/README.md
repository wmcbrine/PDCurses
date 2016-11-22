PDCurses for SDL2
=================

This is a port of PDCurses for SDL2.


Building
--------

- On *nix (including Linux and Mac OS X), run `make` in the sdl2
  directory. It builds the library libpdcurses.a (dynamic lib not implemented).
  The makefile accepts the optional parameters `DEBUG=Y`, `WIDE=Y` and `UTF8=Y`.
  It also recognizes the optional `PDCURSES_SRCDIR` environment variable.


Distribution Status
-------------------

The files in this directory are released to the Public Domain.


Acknowledgements
----------------

The original SDL port was provided by William McBrine.

The initial SDL2 support patch was created by Laura Michaels.

The SDL2 port was put together and further developed by Robin Gustafsson.
