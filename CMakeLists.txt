cmake_minimum_required(VERSION 3.10)
project(rs2-config-clear)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(/std:c++latest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set (CMAKE_PREFIX_PATH "C:\\Dev\\Qt\\Qt5.14.1\\5.14.1\\msvc2017_64")
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

add_subdirectory(src)
