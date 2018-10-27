cmake_minimum_required(VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "No build type specified, defaulting to MinSizeRel.")
endif()

PROJECT(vt VERSION "${PROJECT_VERSION}" LANGUAGES C)

INCLUDE(project_common)


demo_app(../demos firework)
demo_app(../demos ozdemo)
demo_app(../demos newtest)
demo_app(../demos ptest)
demo_app(../demos rain)
demo_app(../demos testcurs)
demo_app(../demos tuidemo)
demo_app(../demos worm)
demo_app(../demos xmas)


IF(WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME}_newtest PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
ENDIF(WIN32)

SET(CPACK_COMPONENTS_ALL applications)
