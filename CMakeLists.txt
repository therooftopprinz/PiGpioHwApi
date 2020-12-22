cmake_minimum_required(VERSION 3.10)

project(pigpiohwapi)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    pigpio
    GIT_REPOSITORY https://github.com/joan2937/pigpio.git
    GIT_TAG master)
FetchContent_MakeAvailable(pigpio)

execute_process(
    COMMAND ${CMAKE_SOURCE_DIR}/find_header_path ${CMAKE_BINARY_DIR} pigpio.h
    OUTPUT_VARIABLE pigpio_INCLUDE_DIR)

message("-- Finding pigpio from ${CMAKE_BINARY_DIR} - found at ${pigpio_INCLUDE_DIR}")

execute_process(
    COMMAND find ${CMAKE_BINARY_DIR} -maxdepth 3 OUTPUT_VARIABLE find_OUT)
message("-- Debug find: \n${find_OUT}")

add_library(hwapi INTERFACE)
target_include_directories(hwapi INTERFACE src/)

add_library(pigpiohwapi STATIC src/pigpiohwapi/PiGpioHwApi.cpp)
target_include_directories(pigpiohwapi PUBLIC ${pigpio_INCLUDE_DIR})
target_link_libraries(pigpiohwapi PUBLIC hwapi pigpiod_if2)
