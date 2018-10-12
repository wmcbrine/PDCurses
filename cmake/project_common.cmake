message(STATUS "**** ${PROJECT_NAME} ****")

set(PDCURSES_SRCDIR ${CMAKE_SOURCE_DIR})
set(PDCURSES_DIST ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE})

set(osdir ${PDCURSES_SRCDIR}/${PROJECT_NAME})
set(demodir ${PDCURSES_SRCDIR}/demos)

set(pdc_src_files
    ${osdir}/pdcclip.c
    ${osdir}/pdcdisp.c
    ${osdir}/pdcgetsc.c
    ${osdir}/pdckbd.c
    ${osdir}/pdcscrn.c
    ${osdir}/pdcsetsc.c
    ${osdir}/pdcutil.c
)

include_directories (..)
include_directories (${osdir})


if(WIN32 AND NOT WATCOM)
    include(dll_version)
    list(APPEND pdc_src_files ${CMAKE_CURRENT_BINARY_DIR}/version.rc)

    add_definitions(-D_WIN32 -D_CRT_SECURE_NO_WARNINGS)

    if(${TARGET_ARCH} STREQUAL "ARM" OR ${TARGET_ARCH} STREQUAL "ARM64")
        add_definitions(-D_ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE=1)
    endif()

    set(EXTRA_LIBS gdi32.lib winspool.lib shell32.lib ole32.lib comdlg32.lib advapi32.lib)
    set(SDL2_DEP_LIBRARIES version.lib winmm.lib imm32.lib)
elseif(APPLE)
    set(EXTRA_LIBS "")
    set(SDL2_DEP_LIBRARIES "dl")
else()
    set(EXTRA_LIBS "")
    set(SDL2_DEP_LIBRARIES "dl")
endif()

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

if(PDC_BUILD_SHARED)
    set(PDCURSE_PROJ ${PROJECT_NAME}_pdcurses)
    add_library(${PDCURSE_PROJ} SHARED ${pdc_src_files} ${pdcurses_src_files})

    if(APPLE)
        set_target_properties(${PDCURSE_PROJ} PROPERTIES MACOSX_RPATH 1)
    endif()

    if(${PROJECT_NAME} STREQUAL "sdl2")
        if(PDC_WIDE)
            target_link_libraries(${PDCURSE_PROJ} ${EXTRA_LIBS}
                ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARY} ${FT2_LIBRARY} ${ZLIB_LIBRARY} 
                ${SDL2_DEP_LIBRARIES})
        else()
            target_link_libraries(${PDCURSE_PROJ} ${EXTRA_LIBS}
                ${SDL2_LIBRARIES} ${SDL2_DEP_LIBRARIES})
        endif()
    else()
        target_link_libraries(${PDCURSE_PROJ} ${EXTRA_LIBS})
    endif()

    install(TARGETS ${PDCURSE_PROJ}
        ARCHIVE DESTINATION ${PDCURSES_DIST}/lib/${PROJECT_NAME}
        LIBRARY DESTINATION ${PDCURSES_DIST}/lib/${PROJECT_NAME}
        RUNTIME DESTINATION ${PDCURSES_DIST}/bin/${PROJECT_NAME} COMPONENT applications)
    set_target_properties(${PDCURSE_PROJ} PROPERTIES OUTPUT_NAME "pdcurses")
else()
    set(PDCURSE_PROJ ${PROJECT_NAME}_pdcursesstatic)
    add_library (${PDCURSE_PROJ} STATIC ${pdc_src_files} ${pdcurses_src_files})
    install (TARGETS ${PDCURSE_PROJ} ARCHIVE DESTINATION ${PDCURSES_DIST}/lib/${PROJECT_NAME} COMPONENT applications)
    set_target_properties(${PDCURSE_PROJ} PROPERTIES OUTPUT_NAME "pdcursesstatic")
endif()

macro (demo_app dir targ)
    set(bin_name "${PROJECT_NAME}_${targ}")
    if(${targ} STREQUAL "tuidemo")
        set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/tuidemo.c ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/tui.c)
    else()
        set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${targ}.c)
    endif()

    add_executable(${bin_name} ${src_files})

    target_link_libraries(${bin_name} ${PDCURSE_PROJ} ${EXTRA_LIBS})

    add_dependencies(${bin_name} ${PDCURSE_PROJ})
    set_target_properties(${bin_name} PROPERTIES OUTPUT_NAME ${targ})

    install(TARGETS ${bin_name} RUNTIME DESTINATION ${PDCURSES_DIST}/bin/${PROJECT_NAME} COMPONENT applications)
endmacro ()
