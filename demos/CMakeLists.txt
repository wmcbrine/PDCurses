cmake_minimum_required(VERSION 3.18...3.27)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out-of-source build
  cmake -B build")
endif()

project(PDcursesDemos LANGUAGES C)

find_package(PDCurses CONFIG REQUIRED)

foreach(f IN ITEMS testcurs ozdemo xmas firework ptest rain worm)
  add_executable(${f} ${f}.c)
  target_link_libraries(${f} PRIVATE CURSES::CURSES)
endforeach()

add_executable(tuidemo tui.c tuidemo.c)
target_link_libraries(tuidemo PRIVATE CURSES::CURSES)

# --- auto-ignore build directory
file(GENERATE OUTPUT .gitignore CONTENT "*")
