cmake_minimum_required(VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "No build type specified, defaulting to MinSizeRel.")
endif()

project(sdl2 VERSION "${PROJECT_VERSION}" LANGUAGES C)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")
message(STATUS "SDL2_INCLUDE_DIR = ${SDL2_INCLUDE_DIR}")
message(STATUS "SDL2_LIBRARY_DIR = ${SDL2_LIBRARY_DIR}")
if(PDC_WIDE)
    message(STATUS "SDL2_TTF_LIBRARY = ${SDL2_TTF_LIBRARY}")
    message(STATUS "SDL2_TTF_INCLUDE_DIR = ${SDL2_TTF_INCLUDE_DIR}")
    message(STATUS "SDL2_TTF_LIBRARY_DIR = ${SDL2_TTF_LIBRARY_DIR}")
endif()

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
link_directories(${SDL2_LIBRARY_DIR} ${SDL2_TTF_LIBRARY_DIR})

include(project_common)

macro (sdl2_app dir targ)

    set(bin_name "${PROJECT_NAME}_${targ}")

    if(${targ} STREQUAL "tuidemo")
        set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/tuidemo.c ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/tui.c)
    else()
        set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${targ}.c)
    endif()

    add_executable(${bin_name} ${src_files})

    if(PDC_WIDE)
        target_link_libraries(${bin_name} ${PDCURSE_PROJ} ${EXTRA_LIBS}
            "${SDL2_LIBRARIES};${SDL2_TTF_LIBRARY};${FT2_LIBRARY};${ZLIB_LIBRARY};${SDL2_DEP_LIBRARIES}")
    else()
        target_link_libraries(${bin_name} ${PDCURSE_PROJ} ${EXTRA_LIBS}
        "${SDL2_LIBRARIES};${SDL2_TTF_DEP_LIBRARIES};${SDL2_DEP_LIBRARIES}")
    endif()

    add_dependencies(${bin_name} ${PDCURSE_PROJ})
    set_target_properties(${bin_name} PROPERTIES OUTPUT_NAME ${targ})

    install(TARGETS ${bin_name} RUNTIME DESTINATION ${PDCURSES_DIST}/bin/${PROJECT_NAME} COMPONENT applications)
    
endmacro ()

sdl2_app(../demos version)
sdl2_app(../demos firework)
sdl2_app(../demos ozdemo)
sdl2_app(../demos newtest)
sdl2_app(../demos ptest)
sdl2_app(../demos rain)
sdl2_app(../demos testcurs)
sdl2_app(../demos tuidemo)
sdl2_app(../demos worm)
sdl2_app(../demos xmas)
sdl2_app(./ sdltest)

if(WIN32)
    set_target_properties(${PROJECT_NAME}_newtest PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif(WIN32)

if(PDC_SDL2_DEPS_BUILD)
    if(PDC_WIDE)
        add_dependencies(${PDCURSE_PROJ} sdl2_ext sdl2_ttf_ext)
    else()
        add_dependencies(${PDCURSE_PROJ} sdl2_ext)
    endif()
endif()

set(CPACK_COMPONENTS_ALL applications)
