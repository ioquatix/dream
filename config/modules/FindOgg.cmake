
include(FindPackageHandleStandardArgs)

find_path(OGG_INCLUDE_DIR ogg/ogg.h)
find_library(OGG_LIBRARY NAMES ogg)

set(OGG_LIBRARIES ${OGG_LIBRARY})

if(OGG_INCLUDE_DIR AND OGG_LIBRARY)
    set(OGG_FOUND TRUE)
endif()

find_package_handle_standard_args(Ogg DEFAULT_MSG OGG_LIBRARIES OGG_INCLUDE_DIR)
