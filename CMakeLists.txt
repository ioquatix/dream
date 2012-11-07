# cmake -DCMAKE_PREFIX_PATH=../../build/darwin_osx/ ..

cmake_minimum_required(VERSION 2.8.9)

project(Dream)

# Standalone build modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/share/cmake/modules")

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

# Required headers:
include_directories(
	${OPENGL_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${VOBIS_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
)

# The standard libraries to link with:
set(CORE_LIBRARIES
	${OPENGL_LIBRARY}
	${OPENAL_LIBRARY}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${FREETYPE_LIBRARY}
	${JPEG_LIBRARY}
	${PNG_LIBRARY}
)

add_subdirectory(source)
add_subdirectory(share)

