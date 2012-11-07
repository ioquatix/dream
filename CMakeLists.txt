# cmake -DCMAKE_PREFIX_PATH=../../build/darwin_osx/ ..

cmake_minimum_required(VERSION 2.8.9)

project(Dream)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/config/modules")

#
# Dream Client Build Options
# Graphics Pipeline: DREAM_OPENGLES20, DREAM_OPENGL32
# Windowing System: DREAM_COCOA, DREAM_UIKIT, DREAM_X11
#

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Ogg REQUIRED)
find_package(Vorbis REQUIRED)
find_package(FreeType REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

if(${DREAM_VARIANT} MATCHES "debug")
  add_definitions(-DDREAM_DEBUG)
endif()
  
add_subdirectory(source)
add_subdirectory(share)

