cmake_minimum_required(VERSION 2.6)
project(Dream)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/inf)
set(DREAM_PLATFORM ${PROJECT_SOURCE_DIR}/ext/darwin_macosx)

link_directories(${DREAM_PLATFORM}/lib)
include_directories(${DREAM_PLATFORM}/include ${DREAM_PLATFORM}/include/freetype2)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_FLAGS "-Wall -std=c99")
set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -stdlib=libc++")

add_subdirectory(src)
