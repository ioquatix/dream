# cmake -DCMAKE_PREFIX_PATH=../../build/darwin_osx/ ..

cmake_minimum_required(VERSION 2.8.9)
project(Dream)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/modules")

#
# Dream Client Build Options
# Graphics Pipeline: DREAM_OPENGLES20, DREAM_OPENGL32
# Windowing System: DREAM_COCOA, DREAM_UIKIT
#

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Ogg REQUIRED)
find_package(Vorbis REQUIRED)
find_package(FreeType REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

# set(DREAM_PLATFORM ${PROJECT_SOURCE_DIR}/ext/linux)

# link_directories(${DREAM_PLATFORM}/lib)
# include_directories(${DREAM_PLATFORM}/include ${DREAM_PLATFORM}/include/freetype2)

set(CMAKE_C_FLAGS "-Wall -std=c99")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -stdlib=libc++")

add_subdirectory(source)
add_subdirectory(share)
