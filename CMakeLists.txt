cmake_minimum_required(VERSION 3.5.0)

project(
    labwork3
    VERSION 1.0
    DESCRIPTION "C++ Year 2024 Labwork3"
    LANGUAGES CXX
)


# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)


set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
add_subdirectory(bin)