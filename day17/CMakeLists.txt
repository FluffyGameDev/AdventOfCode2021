cmake_minimum_required (VERSION 3.9)

include_directories("${CMAKE_SOURCE_DIR}/external/fmt/include")

add_executable (AdventOfCode2021_Day17 "day17.cpp" )

target_link_libraries(AdventOfCode2021_Day17 PRIVATE fmt::fmt-header-only)

set_property(TARGET AdventOfCode2021_Day17 PROPERTY CXX_STANDARD 17)