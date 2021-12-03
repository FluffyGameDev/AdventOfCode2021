cmake_minimum_required (VERSION 3.8)

add_executable (AdventOfCode2021_Day3 "day3.cpp" )

add_custom_command(TARGET AdventOfCode2021_Day3 POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                           ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
                           ${CMAKE_CURRENT_BINARY_DIR}/input.txt)

set_property(TARGET AdventOfCode2021_Day3 PROPERTY CXX_STANDARD 17)