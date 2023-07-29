cmake_minimum_required(VERSION 3.4)

add_library(glew STATIC src/glew.c src/glewinfo.c src/visualinfo.c)
target_include_directories(glew PUBLIC include)