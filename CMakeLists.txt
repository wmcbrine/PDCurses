cmake_minimum_required(VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to MinSizeRel.")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

include (get_version)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX d)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")  # enable parallel builds
endif()

message(STATUS "Generator .............. ${CMAKE_GENERATOR}")
message(STATUS "Build Type ............. ${CMAKE_BUILD_TYPE}")

include(build_options)
include(build_dependencies)

project(pdcurses VERSION "${CURSES_VERSION}" LANGUAGES C)
message(STATUS "PDC Version ............ ${PROJECT_VERSION}")

include(gen_config_header)

file(GLOB pdcurses_src_files pdcurses/*.c)

if(CYGWIN)
    message(STATUS "Windows Kit UM lib path = ${WINDOWS_KIT_LIBRARY_DIR}")
    link_directories(${WINDOWS_KIT_LIBRARY_DIR})
endif()

if(PDC_DOS_BUILD) # currently requires a unique toolchain file

    add_subdirectory(dos)

elseif(PDC_OS2_BUILD) # currently requires a unique toolchain file

    add_subdirectory(os2)
    
else()

    add_subdirectory(ncurses)
    
    if(PDC_SDL2_BUILD)
    
        add_subdirectory(sdl2)
        
    endif()

    if(UNIX)
        add_subdirectory(vt)
    endif()

    if(WIN32)
    
        add_subdirectory(wincon)
        add_subdirectory(wingui)
        
    endif()
   
endif()

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/make_uninstall.cmake")

set(CPACK_COMPONENTS_ALL applications)
