cmake_minimum_required(VERSION 3.4)

file(GLOB_RECURSE SRC CONFIGURE_DEPEND "*.cpp")

add_executable(testing ${SRC})

target_include_directories(testing PUBLIC include)

if(MSVC)
	target_compile_options(testing PUBLIC $<$<CONFIG:RELEASE>:/O2>)
else()
	target_compile_options(testing PUBLIC $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(testing PUBLIC $<$<CONFIG:RELEASE>:-s>)
endif()

target_link_libraries(testing PUBLIC sgl)
target_include_directories(testing PUBLIC src)