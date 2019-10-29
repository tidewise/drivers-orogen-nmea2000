project(nmea2000)
cmake_minimum_required(VERSION 3.5)

SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.orogen/config")
INCLUDE(nmea2000Base)

find_package(Rock)
add_subdirectory(test)