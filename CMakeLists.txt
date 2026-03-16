cmake_minimum_required(VERSION 3.31.6)
project(RamenGL VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # compile_commands.json
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(./dependencies/SDL EXCLUDE_FROM_ALL)

add_executable(task01 
    ./dependencies/glad/src/glad.c
    task01.cpp)
target_include_directories(task01 PUBLIC
./dependencies/glad/include)
target_link_libraries(task01 PRIVATE SDL3::SDL3)
