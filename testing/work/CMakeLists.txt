cmake_minimum_required(VERSION 3.4)

file(GLOB_RECURSE SRC CONFIGURE_DEPEND "*.cpp")

add_executable(work WIN32 ${SRC})

target_include_directories(work PUBLIC include)

if(MSVC)
	target_compile_options(work PUBLIC $<$<CONFIG:RELEASE>:/O2>)
else()
	target_compile_options(work PUBLIC $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(work PUBLIC $<$<CONFIG:RELEASE>:-s>)
endif()

target_link_libraries(work PUBLIC sgl)
target_include_directories(work PUBLIC src)