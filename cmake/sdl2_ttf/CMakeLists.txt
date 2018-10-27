CMAKE_MINIMUM_REQUIRED (VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to MinSizeRel.")
endif()

PROJECT (sdl2_ttf VERSION ${SDL2_TTF_RELEASE} LANGUAGES C)

if(MSVC)
    SET(CMAKE_DEBUG_POSTFIX d)
endif()    

INCLUDE_DIRECTORIES(
    ${ZLIB_INCLUDE_DIR}
    ${FT2_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR})

LINK_DIRECTORIES(
    ${ZLIB_LIBRARY_DIR}
    ${FT2_LIBRARY_DIR}
    ${SDL2_LIBRARY_DIR})


INSTALL (FILES SDL_ttf.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SDL2_ttf)

set(SDL2_ttf_source_files SDL_ttf.c)

IF(WIN32)
    LIST(INSERT SDL2_ttf_source_files 0 Version.rc)
ENDIF()    

if (APPLE)
    find_library(COREVIDEO CoreVideo)
    list(APPEND EXTRA_LIBS ${COREVIDEO})

    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND EXTRA_LIBS ${COCOA_LIBRARY})

    find_library(IOKIT IOKit)
    list(APPEND EXTRA_LIBS ${IOKIT})

    find_library(FORCEFEEDBACK ForceFeedback)
    list(APPEND EXTRA_LIBS ${FORCEFEEDBACK})

    find_library(CARBON_LIBRARY Carbon)
    list(APPEND EXTRA_LIBS ${CARBON_LIBRARY})

    find_library(COREAUDIO CoreAudio)
    list(APPEND EXTRA_LIBS ${COREAUDIO})

    find_library(AUDIOTOOLBOX AudioToolbox)
    list(APPEND EXTRA_LIBS ${AUDIOTOOLBOX})

    include(CheckLibraryExists)
    check_library_exists(iconv iconv_open "" HAVE_LIBICONV)
    if(HAVE_LIBICONV)
      list(APPEND EXTRA_LIBS iconv)
    endif()
endif()

ADD_LIBRARY(SDL2_ttf-static STATIC ${SDL2_ttf_source_files} SDL_ttf.h)
INSTALL (TARGETS SDL2_ttf-static 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    COMPONENT libraries)

ADD_LIBRARY(SDL2_ttf SHARED ${SDL2_ttf_source_files} SDL_ttf.h)
TARGET_LINK_LIBRARIES (SDL2_ttf  ${SDL2_LIBRARY} ${FT2_LIBRARY} ${ZLIB_LIBRARY} ${EXTRA_LIBS})
INSTALL (TARGETS SDL2_ttf
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    COMPONENT libraries)

ADD_EXECUTABLE (showfont ${CMAKE_SOURCE_DIR}/showfont.c)
TARGET_LINK_LIBRARIES (showfont SDL2_ttf  ${SDL2_LIBRARIES} ${FT2_LIBRARY} ${ZLIB_LIBRARY} ${EXTRA_LIBS})
INSTALL (TARGETS showfont RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin COMPONENT applications)

ADD_EXECUTABLE (glfont ${CMAKE_SOURCE_DIR}/glfont.c)
TARGET_LINK_LIBRARIES (glfont SDL2_ttf ${SDL2_LIBRARIES} ${FT2_LIBRARY} ${ZLIB_LIBRARY} ${EXTRA_LIBS})
INSTALL (TARGETS glfont RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin COMPONENT applications)

set(CPACK_COMPONENTS_ALL libraries applications)
