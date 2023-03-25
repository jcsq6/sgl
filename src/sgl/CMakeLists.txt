cmake_minimum_required(VERSION 3.4)

file(GLOB_RECURSE SRC CONFIGURE_DEPEND "*.cpp")

add_library(sgl STATIC ${SRC})

target_include_directories(sgl PUBLIC include)

if(MSVC)
	target_compile_options(sgl PUBLIC $<$<CONFIG:RELEASE>:/O2>)
else()
	target_compile_options(sgl PUBLIC $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(sgl PUBLIC $<$<CONFIG:RELEASE>:-s>)
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(sgl PUBLIC OpenGL::GL GLEW::GLEW glfw Freetype::Freetype)