add_library(sandpile_model sandpile_model.cpp sandpile_model.h lattice_block.cpp lattice_block.h)


target_include_directories(sandpile_model PRIVATE ${CMAKE_SOURCE_DIR}/lib/bmp_lib)

target_link_libraries(sandpile_model PRIVATE bmp_lib)