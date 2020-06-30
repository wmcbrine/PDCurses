include(CheckIncludeFile)
include(CheckSymbolExists)

# extra parts of X11 needed
find_package(X11 REQUIRED)

find_package(PkgConfig REQUIRED)

# xt.pc is broken on Ubuntu
pkg_check_modules(_xt xt)

find_path(_xt_inc
NAMES Intrinsic.h
HINTS ${_xt_INCLUDE_DIRS}
PATHS /usr/include
PATH_SUFFIXES X11
REQUIRED
)

pkg_check_modules(_x11 x11)

find_path(_xatom_inc
NAMES Xatom.h
HINTS ${_x11_INCLUDE_DIRS}
PATH_SUFFIXES X11
REQUIRED
)

pkg_check_modules(_xmu xmu)

find_path(_xmu_inc
NAMES StdSel.h
HINTS ${_xmu_INCLUDE_DIRS}
PATH_SUFFIXES X11/Xmu
REQUIRED
)

pkg_check_modules(_xpm xpm)

find_path(_xpm_inc
NAMES xpm.h
HINTS ${_xpm_INCLUDE_DIRS}
PATH_SUFFIXES X11
REQUIRED
)

pkg_check_modules(_xaw REQUIRED xaw7)

find_path(_xaw_inc
NAMES Box.h
HINTS ${_xaw_INCLUDE_DIRS}
PATH_SUFFIXES X11/Xaw
REQUIRED
)

list(APPEND X11_INCLUDE_DIR
${_xt_inc} ${_xatom_inc}
${_xmu_inc} ${_xmu_inc}/..
${_xpm_inc}
${_xaw_inc} ${_xaw_inc}/..
)

find_library(Xaw NAMES Xaw7
HINTS ${_xaw_LIBRARY_DIRS}
REQUIRED
)

link_directories(${_xaw_LIBRARY_DIRS})
list(APPEND X11_LIBRARIES ${Xaw})

message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}
X11_LIBRARIES: ${X11_LIBRARIES}")


check_include_file("DECkeySym.h" HAVE_DECKEYSYM_H)
check_include_file("Sunkeysym.h" HAVE_SUNKEYSYM_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_symbol_exists("poll" "poll.h" HAVE_POLL)
check_symbol_exists("usleep" "unistd.h" HAVE_USLEEP)

add_compile_definitions(
$<$<BOOL:${HAVE_DECKEYSYM_H}>:HAVE_DECKEYSYM_H>
$<$<BOOL:${HAVE_SUNKEYSYM_H}>:HAVE_SUNKEYSYM_H>
$<$<BOOL:${HAVE_UNISTD_H}>:HAVE_UNISTD_H>
$<$<BOOL:${HAVE_POLL}>:HAVE_POLL>
$<$<BOOL:${HAVE_USLEEP}>:HAVE_USLEEP>
)
