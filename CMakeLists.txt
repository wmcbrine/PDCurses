cmake_minimum_required(VERSION 3.19...4.2)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out-of-source build
  cmake -B build")
endif()

project(PDCurses
LANGUAGES C
VERSION 3.9
DESCRIPTION "PDCurses - a curses library for environments that don't fit the termcap/terminfo model."
HOMEPAGE_URL "https://github.com/wmcbrine/PDCurses")

include(CheckIncludeFile)
include(CheckSymbolExists)

# --- system config
option(PDCurses_WIDE "Enable wide-character support" ON)
option(PDCurses_UTF8 "Enable UTF-8 support")
option(PDCurses_sdl "Use SDL2")
option(PDCurses_xaw3d "Use Xaw3d instead of Xaw")
option(PDCurses_nextaw "Use NeXT Athena widgets instead of Xaw")

include(cmake/compilers.cmake)

add_library(pdcurses pdcurses/addch.c pdcurses/addchstr.c pdcurses/addstr.c
pdcurses/attr.c pdcurses/beep.c pdcurses/bkgd.c pdcurses/border.c pdcurses/clear.c
pdcurses/color.c pdcurses/delch.c pdcurses/deleteln.c pdcurses/getch.c
pdcurses/getstr.c pdcurses/getyx.c pdcurses/inch.c pdcurses/inchstr.c
pdcurses/initscr.c pdcurses/inopts.c pdcurses/insch.c pdcurses/insstr.c
pdcurses/instr.c pdcurses/kernel.c pdcurses/keyname.c pdcurses/mouse.c pdcurses/move.c
pdcurses/outopts.c pdcurses/overlay.c pdcurses/pad.c pdcurses/panel.c pdcurses/printw.c
pdcurses/refresh.c pdcurses/scanw.c pdcurses/scr_dump.c pdcurses/scroll.c
pdcurses/slk.c pdcurses/termattr.c pdcurses/touch.c pdcurses/util.c
pdcurses/window.c pdcurses/debug.c
)
set_property(TARGET pdcurses PROPERTY EXPORT_NAME Curses)
target_include_directories(pdcurses
PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<INSTALL_INTERFACE:include>
PRIVATE pdcurses
)
target_compile_definitions(pdcurses
PUBLIC
$<$<BOOL:${PDCurses_WIDE}>:PDC_WIDE>
PRIVATE
$<$<BOOL:${PDCurses_UTF8}>:PDC_FORCE_UTF8>
$<$<BOOL:${PDCurses_xaw3d}>:USE_XAW3D>
$<$<BOOL:${PDCurses_nextaw}>:USE_NEXTAW>
$<$<CONFIG:Debug,RelWithDebugInfo>:PDCDEBUG>
)

# for FetchContent (need GLOBAL for the other project to see Curses::Curses)
add_library(Curses::Curses IMPORTED INTERFACE GLOBAL)
target_link_libraries(Curses::Curses INTERFACE pdcurses)
target_include_directories(Curses::Curses INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

check_symbol_exists("vsnprintf" "stdio.h" HAVE_VSNPRINTF)
check_symbol_exists("vsscanf" "stdio.h" HAVE_VSSCANF)

target_compile_definitions(pdcurses PRIVATE
$<$<BOOL:${HAVE_VSNPRINTF}>:HAVE_VSNPRINTF>
$<$<BOOL:${HAVE_VSSCANF}>:HAVE_VSSCANF>
)

if(sdl)
  find_package(SDL2 CONFIG REQUIRED)

  target_link_libraries(pdcurses PRIVATE SDL2::SDL2)
  target_include_directories(pdcurses PRIVATE sdl2)

  target_sources(pdcurses PRIVATE sdl2/pdcclip.c sdl2/pdcdisp.c sdl2/pdcgetsc.c
    sdl2/pdckbd.c sdl2/pdcscrn.c sdl2/pdcsetsc.c sdl2/pdcutil.c sdl2/pdcsdl.h
  )
elseif(WIN32)
  target_compile_definitions(pdcurses PRIVATE
  $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>
  $<$<BOOL:${BUILD_SHARED_LIBS}>:PDC_DLL_BUILD>
  )
  target_include_directories(pdcurses PRIVATE wincon)

  target_sources(pdcurses PRIVATE wincon/pdcclip.c wincon/pdcdisp.c wincon/pdcgetsc.c
    wincon/pdckbd.c wincon/pdcscrn.c wincon/pdcsetsc.c wincon/pdcutil.c wincon/pdcwin.h
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "DOS")
  target_sources(pdcurses PRIVATE dos/pdcclip.c dos/pdcdisp.c dos/pdcgetsc.c
      dos/pdckbd.c dos/pdcscrn.c dos/pdcsetsc.c dos/pdcutil.c)
  target_include_directories(pdcurses PRIVATE dos)
else()
  check_include_file("DECkeySym.h" HAVE_DECKEYSYM_H)
  check_include_file("Sunkeysym.h" HAVE_SUNKEYSYM_H)
  check_include_file("unistd.h" HAVE_UNISTD_H)
  check_symbol_exists("poll" "poll.h" HAVE_POLL)
  check_symbol_exists("usleep" "unistd.h" HAVE_USLEEP)

  target_compile_definitions(pdcurses PRIVATE
  $<$<BOOL:${HAVE_DECKEYSYM_H}>:HAVE_DECKEYSYM_H>
  $<$<BOOL:${HAVE_SUNKEYSYM_H}>:HAVE_SUNKEYSYM_H>
  $<$<BOOL:${HAVE_UNISTD_H}>:HAVE_UNISTD_H>
  $<$<BOOL:${HAVE_POLL}>:HAVE_POLL>
  $<$<BOOL:${HAVE_USLEEP}>:HAVE_USLEEP>
  )

  include(cmake/x11.cmake)

  target_compile_definitions(pdcurses PRIVATE XCURSES)

  target_include_directories(pdcurses PRIVATE ${X11_INCLUDE_DIR} x11)
  target_link_libraries(pdcurses PRIVATE X11::X11 X11::Xaw X11::Xt X11::Xpm X11::Xmu)

  target_sources(pdcurses PRIVATE x11/pdcclip.c x11/pdcdisp.c x11/pdcgetsc.c
    x11/pdckbd.c x11/pdcscrn.c x11/pdcsetsc.c x11/pdcutil.c x11/sb.c x11/scrlbox.c
    x11/pdcx11.h x11/scrlbox.h
  )
endif()

install(TARGETS pdcurses EXPORT ${PROJECT_NAME}-targets)
install(FILES curses.h panel.h TYPE INCLUDE)

include(cmake/install.cmake)

# --- auto-ignore build directory
file(GENERATE OUTPUT .gitignore CONTENT "*")
