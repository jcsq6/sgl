cmake_minimum_required(VERSION 3.4)

add_library(stb_image INTERFACE stb_image.h)
target_include_directories(stb_image INTERFACE ".")
