# CMake based build support

This is for more modern host environments which can run CMake. If you have very old operating systems that can't run
CMake, you are better off running the Makefile based builds. The builds have been tested for following targets:

| Compiler    | SDL-1.2     | SDL-2.x     | X11         | wincon      | DOS         |
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| DGJPP       | -           | -           | -           | -           | Y           |
| OpenWatcom  | -           | -           | -           | Y           | Y           |
| MinGW       | Y           | Y           | Y           | Y           | -           |
| GNU (Unix)  | Y           | Y           | Y           | Y           | -           |
| MSVC        | Y           | Y           | -           | Y           | -           |

Potentially, LLVM/clang should also work, but it's not tested yet. For Unix, the host environment was Linux when tested
on. But it could work well on Apple or other well-known POSIX hosts.

## Variables of interest

The CMake build accepts some interesting variables to change the back-end on which curses will work with. All variables
show here are boolean. So it's either TRUE/FALSE or ON/OFF.

| Variable                  | Description                                                |
| ------------------------- | ---------------------------------------------------------- |
| `BUILD_EXAMPLES`          | Compile all demo code                                      |
| `BUILD_TESTS`             | Some platforms like SDL have test support. Compile it too. |
| `ENABLE_WIDECHAR_SUPPORT` | Enable wide-character support into curses                  |
| `FORCE_UTF8`              | Force UTF-8 by ignoring system locale.                     |

To force select platforms in some cases:

| Variable       | OS Availability  |
| -------------- | ---------------- |
| `USE_SDL1`     | Unix, Win32      |
| `USE_SDL2`     | Unix, Win32      |
| `USE_WINCON`   | Win32            |
| `USE_X11`      | Unix             |
| `USE_DOS`      | DOS              |


## How to

Ensure you have CMake and necessary libraries pre-installed. Look up your OS package manager for the right dependencies.
Now, on the shell, enter the directory where PDCurses is checked-out or extracted and run:

### Linux

    $ cmake -H. -Bbuild -DUSE_X11=On -DBUILD_EXAMPLES=On
    ...
    $ cmake -Bbuild --build

In the `build` directory, you can find all example. For example,

    $ ./build/rain

### Windows

    D:\projects\PDCurses> cmake -H. -Bbuild -DUSE_WINCON=On -DBUILD_EXAMPLES=On
    ...
    D:\projects\PDCurses> cmake -Bbuild --build

And then, call (for example):

    D:\projects\PDCurses> build\rain.exe

### DOS

For DOS, however build a DJGPP compiler for Linux host, or download OpenWatcom for Linux/Windows hosts. The invoke CMake
as:

    $ cmake -H. -Bbuild \
        -DCMAKE_SYSTEM_NAME=DOS \
        -DCMAKE_C_COMPILER=/opt/cross/watcom/binl/wcl386 \
        -DUSE_DOS=On \
        -DBUILD_EXAMPLES=On

CMake will complain about unknown Platform because `CMAKE_SYSTEM_NAME` is set to one that it doesn't recognise yet, but
won't stop it from building. This applies to DJGPP too.

## Testing

This is a rudimentary CMake script. While it works well generating binaries for DOS, Windows and Linux, it hasn't been
tested for certain compiler+target OS combinations. Several reasons affect testability including the availability of
CMake on certain hosts, it's ability to recognise certain compilers, etc.