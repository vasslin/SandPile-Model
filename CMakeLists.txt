cmake_minimum_required(VERSION 3.5.0)

project(
    abelian_sandpile_model
    VERSION 2.0
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
add_subdirectory(bin)