cmake_minimum_required(VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "No build type specified, defaulting to MinSizeRel.")
endif()

PROJECT(dos VERSION "${PROJECT_VERSION}" LANGUAGES C)
MESSAGE(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(project_common)

if(WATCOM)
    # this requires using the following toolchainfile file
    #     -DCMAKE_TOOLCHAIN_FILE={path}\watcom_open_dos_toolchain.cmake

    string(APPEND CMAKE_C_FLAGS " -wx")  # warning level: to maximum setting
    
    if(WATCOM_DOS32)
        string(APPEND CMAKE_C_FLAGS " -mf")  # memory model: options are flat (-mf), large (-ml), memdium (-mm), small (-ms)
    else()
        string(APPEND CMAKE_C_FLAGS " -ml")  # memory model: options are large (-ml), memdium (-mm), small (-ms)
    endif()

    string(APPEND CMAKE_C_FLAGS_RELEASE " -oneatx")
    string(APPEND CMAKE_C_FLAGS_MINSIZEREL " -oneatx")
    # optimization: 
    # n -> allow numerically unstable optimizations
    # e -> expand user functions inline
    # a -> relax aliasing constraints
    # t -> favor execution time over code size in optimizations
    # x -> equivalent to -obmiler -s

    string(APPEND CMAKE_C_FLAGS_DEBUG " -d2") # full symbolic debugging info
    string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " debug watcom all")

endif()

demo_app(../demos version)
demo_app(../demos firework)
demo_app(../demos ozdemo)
demo_app(../demos newtest)
demo_app(../demos ptest)
demo_app(../demos rain)
demo_app(../demos testcurs)
demo_app(../demos worm)
demo_app(../demos xmas)
