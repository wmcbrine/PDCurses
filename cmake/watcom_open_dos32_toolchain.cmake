# This module is shared by multiple languages; use include blocker.
if(__WINDOWS_OPENWATCOM)
  return()
endif()
set(__WINDOWS_OPENWATCOM 1)

set(CMAKE_LIBRARY_PATH_FLAG "libpath ")
set(CMAKE_LINK_LIBRARY_FLAG "library ")
set(CMAKE_LINK_LIBRARY_FILE_FLAG "library")

if(CMAKE_VERBOSE_MAKEFILE)
  set(CMAKE_WCL_QUIET)
  set(CMAKE_WLINK_QUIET)
  set(CMAKE_LIB_QUIET)
else()
  set(CMAKE_WCL_QUIET "-zq")
  set(CMAKE_WLINK_QUIET "op q")
  set(CMAKE_LIB_QUIET "-q")
endif()

string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ")

set(CMAKE_C_COMPILE_OPTIONS_DLL "")
set(CMAKE_SHARED_LIBRARY_C_FLAGS "")

message(STATUS "Configured for 32-bit DOS")
set(WATCOM_DOS16 FALSE)
set(WATCOM_DOS32 TRUE)
set(CMAKE_C_COMPILER "wcc386")
set(SYSTEM_NAME dos4g)

# detect folder, add lib386 directory
execute_process(
    COMMAND where ${CMAKE_C_COMPILER}
    OUTPUT_VARIABLE COMPILER_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
get_filename_component(COMPILER_DIR ${COMPILER_PATH} PATH)
get_filename_component(WATCOM_TOOLCHAIN_DIR "${COMPILER_DIR}/.." ABSOLUTE)
string(APPEND LINK_LIBRARIES_PATH "${WATCOM_TOOLCHAIN_DIR}/lib386;${WATCOM_TOOLCHAIN_DIR}/lib386/dos")

set(CMAKE_ASM_COMPILER "wasm")

set(CMAKE_BUILD_TYPE_INIT Debug)

string(APPEND CMAKE_C_FLAGS_INIT " -bt=${SYSTEM_NAME}")

foreach(type CREATE_SHARED_LIBRARY CREATE_SHARED_MODULE LINK_EXECUTABLE)
  set(CMAKE_C_${type}_USE_WATCOM_QUOTE 1)
endforeach()

set(CMAKE_C_CREATE_IMPORT_LIBRARY
  "wlib -q -n -b <TARGET_IMPLIB> +<TARGET_QUOTED>")

set(CMAKE_C_LINK_EXECUTABLE
  "wlink ${CMAKE_START_TEMP_FILE} ${CMAKE_WLINK_QUIET} system ${SYSTEM_NAME} name <TARGET> <LINK_FLAGS> file {<OBJECTS>} <LINK_LIBRARIES> ${CMAKE_END_TEMP_FILE}")

# compile a C file into an object file
set(CMAKE_C_COMPILE_OBJECT
    "<CMAKE_C_COMPILER> ${CMAKE_START_TEMP_FILE} ${CMAKE_WCL_QUIET} -d+ <DEFINES> <INCLUDES> <FLAGS> -fo=\"<OBJECT>\" \"<SOURCE>\"${CMAKE_END_TEMP_FILE}")

# preprocess a C source file
set(CMAKE_C_CREATE_PREPROCESSED_SOURCE
    "<CMAKE_C_COMPILER> ${CMAKE_START_TEMP_FILE} ${CMAKE_WCL_QUIET} -d+ <DEFINES> <INCLUDES> <FLAGS> -fo=\"<PREPROCESSED_SOURCE>\" -pl \"<SOURCE>\"${CMAKE_END_TEMP_FILE}")

string(REPLACE " option implib=<TARGET_IMPLIB>" ""
  CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_LIBRARY}")

# create a C shared library
set(CMAKE_C_CREATE_SHARED_LIBRARY ${CMAKE_CXX_CREATE_SHARED_LIBRARY})

# create a C shared module
set(CMAKE_C_CREATE_SHARED_MODULE ${CMAKE_CXX_CREATE_SHARED_MODULE})

# create a C static library
set(CMAKE_C_CREATE_STATIC_LIBRARY "wlib ${CMAKE_LIB_QUIET} -c -n -b <TARGET_QUOTED> <LINK_FLAGS> <OBJECTS> ")

if(NOT _CMAKE_WATCOM_VERSION)
  set(_CMAKE_WATCOM_VERSION 1)
  if(CMAKE_C_COMPILER_VERSION)
    set(_compiler_version ${CMAKE_C_COMPILER_VERSION})
    set(_compiler_id ${CMAKE_C_COMPILER_ID})
  endif()
  set(WATCOM16)
  set(WATCOM17)
  set(WATCOM18)
  set(WATCOM19)
  set(WATCOM20)
  if("${_compiler_id}" STREQUAL "OpenWatcom")
    if("${_compiler_version}" VERSION_LESS 1.7)
      set(WATCOM16 1)
    endif()
    if("${_compiler_version}" VERSION_EQUAL 1.7)
      set(WATCOM17 1)
    endif()
    if("${_compiler_version}" VERSION_EQUAL 1.8)
      set(WATCOM18 1)
    endif()
    if("${_compiler_version}" VERSION_EQUAL 1.9)
      set(WATCOM19 1)
    endif()
    if("${_compiler_version}" VERSION_EQUAL 2.0)
      set(WATCOM20 1)
    endif()
  endif()
endif()
