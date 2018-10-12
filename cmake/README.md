
CMake Options
-------------

    option(PDC_BUILD_SHARED "Build dynamic libs for pdcurses" ON)
    option(PDC_UTF8 "Force to UTF8" OFF)
    option(PDC_WIDE "Wide - pulls in sdl-ttf" OFF)
    option(PDCDEBUG "Debug tracing" OFF)
    option(PDC_CHTYPE_32 "CHTYPE_32" OFF)
    option(PDC_CHTYPE_16 "CHTYPE_16" OFF)
    option(PDC_DOS_BUILD "Build DOS Project" OFF)
    option(PDC_SDL2_BUILD "Build SDL2 Project" ON)
    option(PDC_SDL2_DEPS_BUILD "Build SDL2 and dependencies" ON)

    

To override the default option value, use the "-DBUILD_SDL2=ON" scheme.  If you set it once, CMake caches the value.

When using pre-built SDL2 bits, simply set these two variables when invoking cmake the first time.

        -DSDL2_INCLUDE_DIR=/my/path/to/sdl2/include/SDL2
        -DSDL2_LIBRARY_DIR=/my/path/to/sdl2/lib/folder


Appveyor
--------

Upon cmake branch check-ins, Appveyor will build the following variants:
wide/utf8 - Release/Debug, Visual Studio 2015 - amd64_x86/amd64/amd64_arm
wide/utf8 - Release/Debug, Visual Studio 2017 - amd64_x86/amd64/amd64_arm/amd64_arm64

^There is currently a SDL2 bug preventing MinSizeRel builds:
https://bugzilla.libsdl.org/show_bug.cgi?id=4233


Native Windows Building
-----------------------

Win32 (pdcurses.sln)

    mkdir build32 & pushd build32
	cmake -G"Visual Studio 15" -DPDC_WIDE=ON -DCMAKE_INSTALL_PREFIX=c:\tmp\pdcurses\Win32 -DCMAKE_BUILD_TYPE=Debug -DPDCDEBUG=ON ..
	popd
	cmake --build build32 --config Debug --target install

Win64 (pdcurses.sln)

    mkdir build64 & pushd build64
	cmake -G"Visual Studio 15 Win64" -DPDC_WIDE=ON -DCMAKE_INSTALL_PREFIX=c:\tmp\pdcurses\Win64 -DCMAKE_BUILD_TYPE=Debug -DPDCDEBUG=ON ..
	popd
	cmake --build build64 --config Debug --target install


Cygwin
------

    mkdir build && pushd build
    cmake .. -G"Unix Makefiles" -DPDC_SDL2_BUILD=OFF -DCMAKE_INSTALL_PREFIX=/cygdrive/c/tmp/pdcurses/Cyg64 -DCMAKE_BUILD_TYPE=Debug -DPDCDEBUG=ON -DWINDOWS_KIT_LIBRARY_DIR=/cygdrive/c/Program\ Files\ \(x86\)/Windows\ Kits/10/Lib/10.0.14393.0/um/x64 ..
    popd
	cmake --build build --config Debug --target install

Note: The demo apps will all build, only version.exe works.  All other apps print "Redirection is not supported."


Linux Building
--------------

SDL2 (Currently the only project supported with CMake)

    cmake .. -DPDC_WIDE=ON -DCMAKE_INSTALL_PREFIX=/home/joel/pdcurses/out -DCMAKE_BUILD_TYPE=Debug -GNinja
    autoninja install
