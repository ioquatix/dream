cmake_minimum_required(VERSION 2.6)
project(Dream)

find_package(OpenGL)
find_package(OpenAL)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/inf)
set(DREAM_PLATFORM ${PROJECT_SOURCE_DIR}/ext/linux)

link_directories(${DREAM_PLATFORM}/lib)
include_directories(${DREAM_PLATFORM}/include ${DREAM_PLATFORM}/include/freetype2)

set(CMAKE_C_FLAGS "-Wall -std=c99")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

add_subdirectory(src)
