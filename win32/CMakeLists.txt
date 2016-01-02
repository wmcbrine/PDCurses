# Written in 2015 by Henrik Steffen Gaﬂmann h.gassmann@online.de
#
# To the extent possible under law, the author(s) have dedicated all
# copyright and related and neighboring rights to this software to the
# public domain worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication
# along with this software. If not, see
#
#     http://creativecommons.org/publicdomain/zero/1.0/
#
########################################################################

if(PDCurses_WIN32_DLL)
    set(PDCurses_LIBRARY_MODE SHARED)
    
    # generate the .def file
    set(PDCurses_DEF_FILE "${CMAKE_CURRENT_BINARY_DIR}/pdcurses.def")
    file(WRITE "${PDCurses_DEF_FILE}" "LIBRARY pdcurses\nEXPORTS\n")
    cat("${CMAKE_SOURCE_DIR}/exp-base.def" "${PDCurses_DEF_FILE}")
    if(PDCurses_UNICODE)
        cat("${CMAKE_SOURCE_DIR}/exp-wide.def" "${PDCurses_DEF_FILE}")
    endif()
    
    # pass the .def file to the linker
    if(MSVC)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEF:${PDCurses_DEF_FILE}")
    elseif(MINGW)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PDCurses_DEF_FILE}")
    endif()
else()
    set(PDCurses_LIBRARY_MODE STATIC)
    
    # embed debug symbols in the static library
    if(MSVC)
        string(REGEX REPLACE "/Z[iI7]" "/Z7"
            CMAKE_C_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG}")
        
        string(REGEX REPLACE "/Z[iI7]" "/Z7"
            CMAKE_C_FLAGS_RELWITHDEBINFO
            "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    endif()
endif()

add_library(PDCurses ${PDCurses_LIBRARY_MODE}
    ${PDCurses_COMMON_SOURCES}
    
    pdcwin.h
    pdcclip.c
    pdcdisp.c
    pdcgetsc.c
    pdckbd.c
    pdcscrn.c
    pdcsetsc.c
    pdcutil.c
    
    # actually mingw could handle those resource files
    # if we wouldn't call add_definitions(-municode)
    $<$<C_COMPILER_ID:MSVC>:pdcurses.rc>
)

source_group(pdcurses REGULAR_EXPRESSION "${CMAKE_SOURCE_DIR}/pdcurses/.*")
source_group(platform REGULAR_EXPRESSION "${CMAKE_SOURCE_DIR}/win32/.*")

if(MINGW)
    if(PDCurses_UNICODE)
        add_definitions(-municode)
    endif()
    if(PDCurses_WIN32_DLL)
        MESSAGE(FATAL_ERROR "MinGW currently produces corrupted dlls")
        set_target_properties(PDCurses PROPERTIES PREFIX "")
    endif()
endif()

target_compile_definitions(PDCurses
    PRIVATE
    $<$<BOOL:${PDCurses_UNICODE}>:PDC_WIDE _UNICODE>
    $<$<BOOL:${PDCurses_UTF8}>:PDC_FORCE_UTF8>
    
    PUPLIC
    $<$<BOOL:${PDCurses_WIN32_DLL}>:PDC_DLL_BUILD>
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:PDCDEBUG>
)

target_include_directories(PDCurses
    PRIVATE "${CMAKE_SOURCE_DIR}"
)

install(TARGETS PDCurses EXPORT PDCurses-targets
    RUNTIME DESTINATION bin/$<CONFIG>
    LIBRARY DESTINATION lib/$<CONFIG>
    ARCHIVE DESTINATION lib/$<CONFIG>
    INCLUDES DESTINATION include
)
