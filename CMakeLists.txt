cmake_minimum_required(VERSION 3.10)

project(pigpiohwapi)

set(CMAKE_CXX_STANDARD 17)

add_library(hwapi INTERFACE)
add_library(pigpiohwapi STATIC src/pigpiohwapi/PiGpioHwApi.cpp)
target_include_directories(hwapi INTERFACE src/)
target_link_libraries(pigpiohwapi hwapi)
