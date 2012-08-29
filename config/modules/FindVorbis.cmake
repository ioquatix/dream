
include(FindPackageHandleStandardArgs)

find_path(VORBIS_INCLUDE_DIR vorbis/codec.h)
find_library(VORBIS_LIBRARY NAMES vorbis)
find_library(VORBISFILE_LIBRARY NAMES vorbisfile)

set(VORBIS_LIBRARIES ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARY})

if(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY AND VORBISFILE_LIBRARY)
	set(VORBIS_FOUND TRUE)
endif()

find_package_handle_standard_args(Vorbis DEFAULT_MSG VORBIS_LIBRARIES VORBIS_INCLUDE_DIR)
