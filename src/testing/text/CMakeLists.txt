cmake_minimum_required(VERSION 3.4)

file(GLOB_RECURSE SRC CONFIGURE_DEPEND "*.cpp")

add_executable(text ${SRC})

target_include_directories(text PUBLIC include)

if(MSVC)
	target_compile_options(text PUBLIC $<$<CONFIG:RELEASE>:/O2>)
else()
	target_compile_options(text PUBLIC $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(text PUBLIC $<$<CONFIG:RELEASE>:-s>)
endif()

target_link_libraries(text PUBLIC sgl)
target_include_directories(text PUBLIC src)