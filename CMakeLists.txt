project(PDCurses)
cmake_minimum_required(VERSION 2.8)

set(VERSION_MAJOR "3")
set(VERSION_MINOR "4")
set(VERSION_PATCH "0")

set(PACKAGE_NAME    ${PROJECT_NAME})
set(PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION})
set(PACKAGE_BUGREPORT wmcbrine@gmail.com)

option(BUILD_EXAMPLES "Build example programs" ON)
option(ENABLE_XCURSES "Build with XCURSES flag" ON)
option(ENABLE_WIDEC "Build wide-character version of the library" OFF)
option(ENABLE_XIM "Use X Input Methods" OFF)
option(ENABLE_FORCE_UTF8 "Force the use of UTF-8 instead of the system locale" OFF)
option(ENABLE_NEXTAW "Enable if you want to use neXtaw library" OFF)
option(ENABLE_XAW3D "Enable if you want to use Xaw3d library" OFF)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

IF(ENABLE_XCURSES)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DXCURSES")
    MESSAGE("Using XCURSES flag")
ENDIF(ENABLE_XCURSES)
IF(ENABLE_WIDEC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPDC_WIDE")
    MESSAGE("Building wide-character version of the library")
ENDIF(ENABLE_WIDEC)
IF(ENABLE_XIM)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPDC_XIM")
    MESSAGE("Using X Input Methods")
ENDIF(ENABLE_XIM)
IF(ENABLE_FORCE_UTF8)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPDC_FORCE_UTF8")
    MESSAGE("Forcing the use of UTF-8 instead of the system locale")
ENDIF(ENABLE_FORCE_UTF8)
IF(ENABLE_NEXTAW)
    SET(USE_NEXTAW "1")
    MESSAGE("Using neXtaw library")
ENDIF(ENABLE_NEXTAW)
IF(ENABLE_XAW3D)
    SET(USE_XAW3D "1")
    MESSAGE("Using Xaw3d library")
ENDIF(ENABLE_XAW3D)

include_directories(.)

add_subdirectory(x11)
if(BUILD_EXAMPLES)
    add_subdirectory(demos)
endif(BUILD_EXAMPLES)



add_custom_target(install_libXCurses
        $(MAKE) install
        DEPENDS XCurses
        COMMENT "Installing ${PROJECT_NAME}")
