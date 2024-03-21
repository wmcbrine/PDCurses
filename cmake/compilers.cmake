include(CheckSymbolExists)

check_symbol_exists("vsnprintf" "stdio.h" HAVE_VSNPRINTF)
check_symbol_exists("vsscanf" "stdio.h" HAVE_VSSCANF)

add_compile_definitions(
$<$<BOOL:${HAVE_VSNPRINTF}>:HAVE_VSNPRINTF>
$<$<BOOL:${HAVE_VSSCANF}>:HAVE_VSSCANF>
)

# --- compiler options

if(CMAKE_C_COMPILER_ID STREQUAL OpenWatcom)
  message(STATUS "OpenWatcom: $ENV{WATCOM}
  Host: ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION}
  Target: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}
  ")
  # https://wiki.archlinux.org/title/Open_Watcom
  add_compile_options(-bt=dos -bcl=dos)
else()
  add_compile_options(-Wall)
endif()
