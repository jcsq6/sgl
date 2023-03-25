add_subdirectory(sgl)
add_subdirectory(testing)

add_custom_target(three_target DEPENDS sgl three)