cmake_minimum_required(VERSION 3.15)
project(PI_projectSDL2_part_lib C)


set(CMAKE_C_STANDARD 11)
find_library(SDL2 REQUIRED SDL2)
include_directories(${SDL2_INCLUDE_DIRS})


link_libraries(${CMAKE_SOURCE_DIR}/SDL2.dll)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lSDL2")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lm -ldl")

FILE(GLOB_RECURSE SOURCE_FILES src/* [src/])
add_executable(PI_projectSDL2_part_lib ${SOURCE_FILES})

target_link_libraries(PI_projectSDL2_part_lib -lm SDL2)
