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

add_executable(tuidemo
    tui.h
    tui.c
    tuidemo.c
)
target_link_libraries(tuidemo PRIVATE PDCurses)
target_include_directories(tuidemo PRIVATE "${CMAKE_SOURCE_DIR}")

macro(add_demo NAME)
    add_executable(${NAME} ${NAME}.c)
    target_link_libraries(${NAME} PRIVATE PDCurses)
    target_include_directories(${NAME} PRIVATE "${CMAKE_SOURCE_DIR}")
endmacro()

add_demo(firework)

add_demo(newdemo)

add_demo(ptest)

add_demo(rain)

add_demo(testcurs)

add_demo(worm)

add_demo(xmas)
