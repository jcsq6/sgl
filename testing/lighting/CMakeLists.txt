cmake_minimum_required(VERSION 3.4)

file(GLOB_RECURSE SRC CONFIGURE_DEPEND "*.cpp")

add_executable(lighting WIN32 ${SRC})

target_include_directories(lighting PUBLIC include)

if(MSVC)
	target_compile_options(lighting PUBLIC $<$<CONFIG:RELEASE>:/O2>)
else()
	target_compile_options(lighting PUBLIC $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(lighting PUBLIC $<$<CONFIG:RELEASE>:-s>)
endif()

target_link_libraries(lighting PUBLIC sgl)
target_include_directories(lighting PUBLIC src)