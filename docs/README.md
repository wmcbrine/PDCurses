PDCurses Documentation
======================

The main documentation for PDCurses is here:

- [User's Guide] - Basic overview
- [Implementor's Guide] - What you need to know to port PDCurses
- [Manual] - Each function, etc.
- [History]

Also consult the README for each specific platform you'll be using:

- [DOS]
- [OS/2]
- [SDL 1.x]
- [SDL 2.x]
- [Windows]
- [X11]


Building
--------

The PDcurses library can be built for Unix-like systems and Windows via Makefiles
or CMake.
To build with CMake:

```sh
cmake -B build
cmake --build build
```

optionally, install the library binary and headers:

```sh
cmake -B build --install-prefix /path/to/install
cmake --build build
cmake --install build
```

If using X11, install X11 like:

* Linux: `apt install --no-install-recommends libxt-dev libxaw7-dev libx11-dev`
* macOS: `brew install xorgproto libxt libxaw libx11 pkgconf`

- To rebuild MANUAL.md from the "man page" sections of the source code,
  type "./mkman.sh". Needs a Unix-like shell and an Awk interpreter.


[User's Guide]: USERS.md
[Implementor's Guide]: IMPLEMNT.md
[Manual]: MANUAL.md
[History]: HISTORY.md
[DOS]: ../dos/README.md
[OS/2]: ../os2/README.md
[SDL 1.x]: ../sdl1/README.md
[SDL 2.x]: ../sdl2/README.md
[Windows]: ../wincon/README.md
[X11]: ../x11/README.md
