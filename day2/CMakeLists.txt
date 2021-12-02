cmake_minimum_required (VERSION 3.8)

add_executable (AdventOfCode2021_Day2 "day2.cpp" )

add_custom_command(TARGET AdventOfCode2021_Day2 POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                           ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
                           ${CMAKE_CURRENT_BINARY_DIR}/input.txt)