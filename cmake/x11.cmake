# extra parts of X11 needed
find_package(X11 REQUIRED)

find_package(PkgConfig)

# xt.pc is broken on Ubuntu
if(PKG_CONFIG_FOUND)

pkg_check_modules(_xt xt)
pkg_check_modules(_x11 x11)

endif()

# Ubuntu may need this
find_path(_xt_inc
NAMES Intrinsic.h
HINTS ${_xt_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11
REQUIRED
)

# macOS may need this
find_path(_xatom_inc
NAMES Xatom.h
HINTS ${_x11_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11
REQUIRED
)

list(APPEND X11_INCLUDE_DIR
${_xt_inc} ${_xatom_inc}
)

message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}
X11_LIBRARIES: ${X11_LIBRARIES}")
