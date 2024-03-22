cmake_minimum_required(VERSION 3.18...3.27)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out-of-source build
  cmake -B build")
endif()

project(PDcursesDemos LANGUAGES C)

enable_testing()

find_package(PDCurses CONFIG REQUIRED)

foreach(f IN ITEMS testcurs ozdemo xmas firework ptest rain worm test_init)
  add_executable(${f} ${f}.c)
  target_link_libraries(${f} PRIVATE CURSES::CURSES)
endforeach()

add_executable(tuidemo tui.c tuidemo.c)
target_link_libraries(tuidemo PRIVATE CURSES::CURSES)

file(GENERATE OUTPUT .gitignore CONTENT "*")

add_test(NAME Init COMMAND test_init)
set_tests_properties(Init PROPERTIES
SKIP_REGULAR_EXPRESSION "Can't open display|no DISPLAY variable set"
)

# Note: using STDIN INPUT_FILE for CTest doesn't work as Curses wants an actual keypress, not pipe.
