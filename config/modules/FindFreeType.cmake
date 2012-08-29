
include(FindPackageHandleStandardArgs)

find_path(FREETYPE_INCLUDE_DIR_ft2build ft2build.h)
find_path(FREETYPE_INCLUDE_DIR_freetype2 freetype/config/ftheader.h PATH_SUFFIXES freetype2)
find_library(FREETYPE_LIBRARY NAMES freetype)

if (FREETYPE_INCLUDE_DIR_ft2build AND FREETYPE_INCLUDE_DIR_freetype2)
    set(FREETYPE_INCLUDE_DIRS "${FREETYPE_INCLUDE_DIR_ft2build};${FREETYPE_INCLUDE_DIR_freetype2}")
endif()

set(FREETYPE_LIBRARIES ${FREETYPE_LIBRARY})

if(FREETYPE_INCLUDE_DIRS AND FREETYPE_LIBRARIES)
    set(FREETYPE_FOUND TRUE)
endif()

find_package_handle_standard_args(FreeType DEFAULT_MSG FREETYPE_LIBRARIES FREETYPE_INCLUDE_DIRS)
mark_as_advanced(FREETYPE_LIBRARY FREETYPE_INCLUDE_DIR_freetype2 FREETYPE_INCLUDE_DIR_ft2build)
