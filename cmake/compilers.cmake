if(CMAKE_C_COMPILER_ID STREQUAL OpenWatcom)
  message(STATUS "OpenWatcom: $ENV{WATCOM}
  Host: ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION}
  Target: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}
  ")
  # https://wiki.archlinux.org/title/Open_Watcom
  add_compile_options(-bt=dos -bcl=dos)
elseif(NOT MSVC)
  add_compile_options(-Wall)
endif()
