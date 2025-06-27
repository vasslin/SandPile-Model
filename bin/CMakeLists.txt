add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE sandpile_model)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})


include_directories(${CMAKE_SOURCE_DIR}/lib)