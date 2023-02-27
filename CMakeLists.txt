cmake_minimum_required(VERSION 2.8)

project(FIR_filter)

set(CMAKE_BUILD_TYPE "Release")

add_library(${PROJECT_NAME} SHARED
    BVector_Filter.cpp
)

add_executable(main BVector_FIlter_Test.cpp)
target_link_libraries(main ${PROJECT_NAME})
