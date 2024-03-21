# extra parts of X11 needed
find_package(X11 REQUIRED)

find_package(PkgConfig)

# xt.pc is broken on Ubuntu
if(PKG_CONFIG_FOUND)

pkg_check_modules(_xt xt)
pkg_check_modules(_x11 x11)
pkg_check_modules(_xmu xmu)
pkg_check_modules(_xpm xpm)
pkg_check_modules(_xaw REQUIRED xaw7)

endif()

find_path(_xt_inc
NAMES Intrinsic.h
HINTS ${_xt_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11
REQUIRED
)

find_path(_xatom_inc
NAMES Xatom.h
HINTS ${_x11_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11
REQUIRED
)

find_path(_xmu_inc
NAMES StdSel.h
HINTS ${_xmu_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11/Xmu
REQUIRED
)

find_path(_xpm_inc
NAMES xpm.h
HINTS ${_xpm_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11
REQUIRED
)

find_path(_xaw_inc
NAMES Box.h
HINTS ${_xaw_INCLUDE_DIRS} ${X11_INCLUDE_DIR}
PATH_SUFFIXES X11/Xaw
REQUIRED
)

list(APPEND X11_INCLUDE_DIR
${_xt_inc} ${_xatom_inc}
${_xmu_inc} ${_xmu_inc}/..
${_xpm_inc}
${_xaw_inc} ${_xaw_inc}/..
)

message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}
X11_LIBRARIES: ${X11_LIBRARIES}")
