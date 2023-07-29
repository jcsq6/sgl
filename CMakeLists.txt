cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

project ("sgl")

add_subdirectory(sgl)
add_subdirectory(testing)
add_subdirectory(contrib)

add_subdirectory(projects)

add_custom_target(three_target DEPENDS sgl work)