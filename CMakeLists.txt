# Written in 2015 by Henrik Steffen Gaﬂmann h.gassmann@online.de
#
# To the extent possible under law, the author(s) have dedicated all
# copyright and related and neighboring rights to this software to the
# public domain worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication
# along with this software. If not, see
#
#     http://creativecommons.org/publicdomain/zero/1.0/
#
########################################################################
cmake_minimum_required(VERSION 3.0)
project(PDCurses VERSION 3.4)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in-source builds are not supported!")
endif()

function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

set(PDCurses_PUBLIC_HEADERS
    "${CMAKE_SOURCE_DIR}/curses.h"
    "${CMAKE_SOURCE_DIR}/panel.h"
    "${CMAKE_SOURCE_DIR}/term.h"
)

set(PDCurses_COMMON_SOURCES
    ${PDCurses_PUBLIC_HEADERS}
    "${CMAKE_SOURCE_DIR}/curspriv.h"
    
    "${CMAKE_SOURCE_DIR}/pdcurses/addch.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/addchstr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/addstr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/attr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/beep.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/bkgd.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/border.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/clear.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/color.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/delch.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/deleteln.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/deprec.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/getch.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/getstr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/getyx.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/inch.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/inchstr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/initscr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/inopts.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/insch.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/insstr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/instr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/kernel.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/keyname.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/mouse.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/move.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/outopts.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/overlay.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/pad.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/panel.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/printw.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/refresh.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/scanw.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/scr_dump.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/scroll.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/slk.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/termattr.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/terminfo.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/touch.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/util.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/window.c"
    "${CMAKE_SOURCE_DIR}/pdcurses/debug.c"
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(PDCurses_DEMO_PROJECTS "enable if you want to include the demo projects")

if(WIN32)
    option(PDCurses_WIN32_DLL "build the shared library version of pdcurses")
    option(PDCurses_UNICODE "build the library with wide character support" ON)
    option(PDCurses_UTF8 "treat narrow string content as utf8 (only effective if PDCurses_WIDE_API is turned on)" ON)
    
    if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
    
    add_subdirectory(win32)
else()
    MESSAGE(FATAL_ERROR "your platform is currently not supported :(")
endif()

if(PDCurses_DEMO_PROJECTS)
    add_subdirectory(demos)
endif()

########################################################################
# install target
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/PDCursesConfigVersion.cmake"
  VERSION ${PDCurses_VERSION}
  COMPATIBILITY SameMajorVersion # I don't know how backwards compatible pdcurses is...
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/PDCursesConfigVersion.cmake" DESTINATION cmake)

configure_file(cmake/PDCursesConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/PDCursesConfig.cmake"
  COPYONLY
)
install(FILES ${PDCurses_PUBLIC_HEADERS} DESTINATION include)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/PDCursesConfig.cmake" DESTINATION cmake)

install(EXPORT PDCurses-targets DESTINATION cmake)
