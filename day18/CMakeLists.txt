cmake_minimum_required (VERSION 3.9)

include_directories("${CMAKE_SOURCE_DIR}/external/fmt/include")

add_executable (AdventOfCode2021_Day18 "day18.cpp" "tests.cpp" "pairregistry.cpp" "pairoperations.cpp")

target_link_libraries(AdventOfCode2021_Day18 PRIVATE fmt::fmt-header-only)

add_custom_command(TARGET AdventOfCode2021_Day18 POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                           ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
                           ${CMAKE_CURRENT_BINARY_DIR}/input.txt)

set_property(TARGET AdventOfCode2021_Day18 PROPERTY CXX_STANDARD 17)