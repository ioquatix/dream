
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
