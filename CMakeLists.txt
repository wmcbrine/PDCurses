cmake_minimum_required(VERSION 2.8)

project(PDCurses C)

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(GNUInstallDirs)

# Basic switches
option(BUILD_EXAMPLES           "Build some demo examples"  OFF)
option(BUILD_TESTS              "Build tests if any for the chosen platform." OFF)
# Common features.
option(ENABLE_WIDECHAR_SUPPORT  "Enable wide-char support"  OFF)
option(FORCE_UTF8               "Force UTF-8 by ignoring system locale." OFF)

# Do some compiler fiddling.
set(CMAKE_POSITION_INDEPENDENT_CODE On)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "clang")
    set(CMAKE_C_FLAGS_DEBUG             "-D_DEBUG -DPDCDEBUG -DPANEL_DEBUG -O0 -ggdb -g3")
    set(CMAKE_C_FLAGS_RELEASE           "-DNDEBUG -O3 -s")
    set(CMAKE_C_FLAGS_MINSIZEREL        "-DNDEBUG -Os -s")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-D_DEBUG -DPDCDEBUG -DPANEL_DEBUG -g3 -O3")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_DEBUG     "/D_DEBUG /DPDCDEBUG /DPANEL_DEBUG /Zi /EHsc /Oy- /Ob0")
    set(CMAKE_C_FLAGS_RELEASE   "/DNDEBUG /Ox /EHc")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
endif()

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CONSOLE)

    if (ENABLE_WIDECHAR_SUPPORT)
        add_definitions(-D_UNICODE)
        add_definitions(-DUNICODE)
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Back-end support.
if (UNIX OR WIN32)
    set(USE_SDL1        FALSE CACHE BOOL
        "Build against SDL-1.x platform")
    set(USE_SDL2        FALSE CACHE BOOL
        "Build against SDL-2.x platform")
    set(USE_WINCON      FALSE CACHE BOOL
        "Build against Windows console platform")
elseif (UNIX)
    set(USE_X11         FALSE CACHE BOOL
        "Build against XOrg/X11 platform")
elseif (CMAKE_SYSTEM_NAME STREQUAL "DOS")
    set(USE_DOS         TRUE CACHE BOOL
        "Build for DOS platform")
endif()

include_directories(${CMAKE_SOURCE_DIR})

set(srcs_all
    pdcurses/addch.c
    pdcurses/addchstr.c
    pdcurses/addstr.c
    pdcurses/attr.c
    pdcurses/beep.c
    pdcurses/bkgd.c
    pdcurses/border.c
    pdcurses/clear.c
    pdcurses/color.c
    pdcurses/debug.c
    pdcurses/delch.c
    pdcurses/deleteln.c
    pdcurses/getch.c
    pdcurses/getstr.c
    pdcurses/getyx.c
    pdcurses/inch.c
    pdcurses/inchstr.c
    pdcurses/initscr.c
    pdcurses/inopts.c
    pdcurses/insch.c
    pdcurses/insstr.c
    pdcurses/instr.c
    pdcurses/kernel.c
    pdcurses/keyname.c
    pdcurses/mouse.c
    pdcurses/move.c
    pdcurses/outopts.c
    pdcurses/overlay.c
    pdcurses/pad.c
    pdcurses/panel.c
    pdcurses/printw.c
    pdcurses/refresh.c
    pdcurses/scanw.c
    pdcurses/scr_dump.c
    pdcurses/scroll.c
    pdcurses/slk.c
    pdcurses/termattr.c
    pdcurses/touch.c
    pdcurses/util.c
    pdcurses/window.c
)

if (USE_X11)
    find_package(X11 REQUIRED)

    set(ENABLE_WIDECHAR_SUPPORT ON)

    set(CMAKE_REQUIRED_INCLUDES ${X11_INCLUDE_DIR}/X11)
    check_include_file(DECkeysym.h HAVE_DECKEYSYM_H)
    check_include_file(Sunkeysym.h HAVE_SUNKEYSYM_H)
    check_include_file(xpm.h       HAVE_XPM_H)
    check_include_file(unistd.h    HAVE_UNISTD_H)

    check_symbol_exists("poll"      poll.h      HAVE_POLL)
    check_symbol_exists("usleep"    unistd.h    HAVE_USLEEP)
    check_symbol_exists("vsnprintf" stdio.h     HAVE_VSNPRINTF)
    check_symbol_exists("vsscanf"   stdio.h     HAVE_VSSCANF)

    check_c_source_compiles(
        "
            #include <X11/Xlib.h>

            XPointer *ptr;
            int main(void) {return 0;}
        "
        XPOINTER_TYPEDEFED
    )

    configure_file(x11/config.h.cmake.in ${CMAKE_BINARY_DIR}/config.h)
    add_definitions(-DHAVE_CONFIG_H)
    add_definitions(-DXCURSES)

    include_directories(${X11_INCLUDE_DIR}/X11)
    include_directories(x11)
    include_directories(${CMAKE_BINARY_DIR})

    set(srcs_all
        ${srcs_all}
        x11/pdcclip.c
        x11/pdcdisp.c
        x11/pdcgetsc.c
        x11/pdckbd.c
        x11/pdcscrn.c
        x11/pdcsetsc.c
        x11/pdcutil.c
        x11/sb.c
        x11/scrlbox.c
    )
elseif (USE_SDL1)
    find_package(SDL REQUIRED)

    include_directories(${SDL_INCLUDE_DIR})

    set(srcs_all
        ${srcs_all}
        sdl1/pdcclip.c
        sdl1/pdcdisp.c
        sdl1/pdcgetsc.c
        sdl1/pdckbd.c
        sdl1/pdcscrn.c
        sdl1/pdcsetsc.c
        sdl1/pdcutil.c
    )
elseif (USE_SDL2)
    find_package(SDL2 REQUIRED)

    include_directories(${SDL2_INCLUDE_DIR})

    set(srcs_all
        ${srcs_all}
        sdl2/pdcclip.c
        sdl2/pdcdisp.c
        sdl2/pdcgetsc.c
        sdl2/pdckbd.c
        sdl2/pdcscrn.c
        sdl2/pdcsetsc.c
        sdl2/pdcutil.c
    )
elseif (USE_WINCON)
    set(srcs_all
        ${srcs_all}
        wincon/pdcclip.c
        wincon/pdcdisp.c
        wincon/pdcgetsc.c
        wincon/pdckbd.c
        wincon/pdcscrn.c
        wincon/pdcsetsc.c
        wincon/pdcutil.c
    )
elseif (USE_DOS)
    set(srcs_all
        ${srcs_all}
        dos/pdcclip.c
        dos/pdcdisp.c
        dos/pdcgetsc.c
        dos/pdckbd.c
        dos/pdcscrn.c
        dos/pdcsetsc.c
        dos/pdcutil.c
    )
endif()

if (ENABLE_WIDECHAR_SUPPORT)
    add_definitions(-DPDC_WIDE)
endif()

if (FORCE_UTF8)
    add_definitions(-DPDC_FORCE_UTF8)
endif()

add_library(${PROJECT_NAME} ${srcs_all})

if (USE_X11)
    target_link_libraries(${PROJECT_NAME} ${X11_Xmu_LIB})
    target_link_libraries(${PROJECT_NAME} ${X11_Xt_LIB})
    target_link_libraries(${PROJECT_NAME} ${X11_X11_LIB})
    target_link_libraries(${PROJECT_NAME} ${X11_Xpm_LIB})
    target_link_libraries(${PROJECT_NAME} ${X11_SM_LIB})
    target_link_libraries(${PROJECT_NAME} ${X11_ICE_LIB})
    target_link_libraries(${PROJECT_NAME} ${X11_Xext_LIB})
elseif (USE_SDL1)
    target_link_libraries(${PROJECT_NAME} ${SDL_LIBRARY})
elseif (USE_SDL2)
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
endif()

function(gen_exe out_f srcs)
    add_executable(${out_f} ${srcs})
    target_link_libraries(${out_f} ${PROJECT_NAME})
    if (USE_X11)
        # -lXaw -lXmu -lXt -lX11 -lXpm -lSM -lICE -lXext
        target_link_libraries(${out_f} ${X11_Xmu_LIB})
        target_link_libraries(${out_f} ${X11_Xt_LIB})
        target_link_libraries(${out_f} ${X11_X11_LIB})
        target_link_libraries(${out_f} ${X11_Xpm_LIB})
        target_link_libraries(${out_f} ${X11_SM_LIB})
        target_link_libraries(${out_f} ${X11_ICE_LIB})
        target_link_libraries(${out_f} ${X11_Xext_LIB})
        # Unfortunately, CMake's X11 as yet doesn't support Xaw in the FindX11.
        # See, https://gitlab.kitware.com/cmake/cmake/-/issues/20963
        # Link directly and hope everything works fine.
        target_link_libraries(${out_f} -lXaw)
    endif()
    if (USE_SDL1)
        target_link_libraries(${out_f} ${SDL_LIBRARY})
    endif()
    if (USE_SDL2)
        target_link_libraries(${out_f} ${SDL2_LIBRARY})
    endif()
endfunction()

if (BUILD_TESTS)
    if (USE_SDL1)
        gen_exe(sdltest sdl1/sdltest.c)
    endif()
    if (USE_SDL2)
        gen_exe(sdltest sdl2/sdltest.c)
    endif()
endif()

if (BUILD_EXAMPLES)
    include_directories(demos)
    gen_exe(firework  "demos/firework.c")
    gen_exe(ozdemo    "demos/ozdemo.c")
    gen_exe(ptest     "demos/ptest.c")
    gen_exe(rain      "demos/rain.c")
    gen_exe(tui       "demos/tuidemo.c;demos/tui.c")
    gen_exe(testcurs  "demos/testcurs.c")
    gen_exe(worm      "demos/worm.c")
    gen_exe(xmas      "demos/xmas.c")
endif()

configure_file(FindPDCurses.cmake.in ${CMAKE_BINARY_DIR}/FindPDCurses.cmake)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES curses.h          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES panel.h           DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
