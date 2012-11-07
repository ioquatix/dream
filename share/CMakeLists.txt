
install(
	DIRECTORY "build"
	DESTINATION "share/Dream/build"
	FILES_MATCHING PATTERN "*.xcconfig"
)

install(
	DIRECTORY "resources"
	DESTINATION "share/Dream/resources"
	FILES_MATCHING PATTERN "*"
)

configure_file(
	"build/dream-config.cmake.in"
	"${PROJECT_BINARY_DIR}/dream-config.cmake"
	@ONLY
)


set(SHARE_CMAKE_DIR "share/dream/cmake")

install(FILES
	"${PROJECT_BINARY_DIR}/dream-config.cmake"
	DESTINATION "${SHARE_CMAKE_DIR}"
)

