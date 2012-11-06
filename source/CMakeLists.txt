
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
set(CORELIBS freetype jpeg png ogg vorbis ${OPENGL_LIBRARY} ${OPENAL_LIBRARY})

set(DREAM_SOURCE_FILES
	./Assertion.cpp
	./Class.cpp
	./Client/Audio/Mixer.cpp
	./Client/Audio/OggResource.cpp
	./Client/Audio/Sound.cpp
	./Client/Audio/Stream.cpp
	./Client/Display/Application.cpp
	./Client/Display/Context.cpp
	./Client/Display/Scene.cpp
	./Client/Graphics/Buffer.cpp
	./Client/Graphics/FrameBuffer.cpp
	./Client/Graphics/Graphics.cpp
	./Client/Graphics/MeshBuffer.cpp
	./Client/Graphics/ParticleRenderer.cpp
	./Client/Graphics/PixelBufferRenderer.cpp
	./Client/Graphics/Renderer.cpp
	./Client/Graphics/ShaderManager.cpp
	./Client/Graphics/TextureManager.cpp
	./Client/Graphics/VertexArray.cpp
	./Client/Graphics/View.cpp
	./Client/Graphics/WireframeRenderer.cpp
	./Core/Buffer.cpp
#	Unit-test specific
#	./Core/CodeTest.cpp
	./Core/Data.cpp
	./Core/Dictionary.cpp
	./Core/Endian.cpp
	./Core/ObjectSet.cpp
	./Core/Path.cpp
	./Core/Serialization.cpp
	./Core/Strings.cpp
	./Core/System.cpp
	./Core/Timer.cpp
	./Core/URI.cpp
	./Core/Value.cpp
	./Events/Console.cpp
	./Events/Fader.cpp
	./Events/Input.cpp
	./Events/Logger.cpp
	./Events/Loop.cpp
	./Events/MultiFingerInput.cpp
	./Events/Source.cpp
	./Events/Thread.cpp
# 	Incomplete
#	./Events/WorkQueue.cpp
	./Framework.cpp
	./Geometry/AlignedBox.cpp
	./Geometry/Axis.cpp
	./Geometry/Frustum.cpp
	./Geometry/Generate/Cylinder.cpp
	./Geometry/Geometry.cpp
	./Geometry/Intersection.cpp
	./Geometry/Line.cpp
	./Geometry/Mesh.cpp
	./Geometry/Plane.cpp
	./Geometry/Sphere.cpp
	./Geometry/Spline.cpp
	./Geometry/Triangle.cpp
	./Imaging/Image.cpp
	./Imaging/ImageLoader.cpp
	./Imaging/PixelBuffer.cpp
	./Imaging/PixelBufferSaver-PNG.cpp
	./Network/Address.cpp
	./Network/Message.cpp
	./Network/Network.cpp
	./Network/Server.cpp
	./Network/ServerTest.cpp
	./Network/Socket.cpp
	./Numerics/Average.cpp
	./Numerics/Color.cpp
	./Numerics/Matrix.NEON.cpp
	./Numerics/Matrix.SSE.cpp
	./Numerics/Matrix.cpp
	./Numerics/Number.cpp
	./Numerics/Numerics.cpp
	./Numerics/PerlinNoise.cpp
	./Numerics/Quaternion.cpp
	./Numerics/RandomSequence.cpp
	./Numerics/Vector.cpp
	./Reference.cpp
	./Renderer/BirdsEyeCamera.cpp
	./Renderer/Camera.cpp
	./Renderer/PointCamera.cpp
	./Renderer/Projection.cpp
	./Renderer/Viewport.cpp
	./Resources/Loadable.cpp
	./Resources/Loader.cpp
	./Simulation/AlignedTree.cpp
	./Simulation/HeightMap.cpp
	./Simulation/PathFinder.cpp
	./Text/Font.cpp
	./Text/FontFace.cpp
	./Text/TextBlock.cpp
	./Text/TextBuffer.cpp
)

set(DREAM_PLATFORM_LINUX_FILES
	./Core/Path-Unix.cpp
	./Core/Timer-Unix.cpp
	
	./Resources/Loader-Generic.cpp

	./Client/Display/X11/Application.cpp
	./Client/Display/X11/Context.cpp
)

set(DREAM_PLATFORM_DARWIN_IOS_FILES
	./Core/Path-Mac.mm
	./Core/Timer-CoreVideo.cpp
	
	./Resources/Loader-UIKit.mm

	./Client/Display/UIKit/Application.mm
	./Client/Display/UIKit/Context.mm
	./Client/Display/UIKit/DApplicationDelegate.mm
	./Client/Display/UIKit/DOpenGLView.mm
	./Client/Display/UIKit/EAGLView.mm
)

set(DREAM_PLATFORM_DARWIN_OSX_FILES
	./Core/Path-Mac.mm
	./Core/Timer-CoreVideo.cpp
	
	./Resources/Loader-Cocoa.mm
	
	./Client/Display/Cocoa/Application.mm
	./Client/Display/Cocoa/Context.mm
	./Client/Display/Cocoa/DApplicationDelegate.mm
	./Client/Display/Cocoa/DOpenGLView.mm
	./Client/Display/Cocoa/DWindowDelegate.mm
)

message(STATUS "Configuring based on variant: " ${DREAM_PLATFORM})

if(${DREAM_PLATFORM} MATCHES "darwin_ios")
	message(STATUS "Building 'darwin_ios' display context.")
	set(DREAM_SOURCE_FILES ${DREAM_SOURCE_FILES} ${DREAM_PLATFORM_DARWIN_IOS_FILES})
endif()

if(${DREAM_PLATFORM} MATCHES "darwin_osx")
	message(STATUS "Building 'darwin_osx' display context.")
	set(DREAM_SOURCE_FILES ${DREAM_SOURCE_FILES} ${DREAM_PLATFORM_DARWIN_OSX_FILES})
endif()

if(${DREAM_PLATFORM} MATCHES "linux")
	message(STATUS "Building 'darwin_osx' display context.")
	set(DREAM_SOURCE_FILES ${DREAM_SOURCE_FILES} ${DREAM_PLATFORM_LINUX_FILES})
endif()

# % find . -name "*.cpp"
add_library(Dream STATIC
	${DREAM_SOURCE_FILES}
)

target_link_libraries(Dream ${CORELIBS})

install(
	TARGETS
		Dream
	ARCHIVE DESTINATION
		lib
)

# find . -name "*.h"
install(
	DIRECTORY . 
	DESTINATION "include/Dream" 
	FILES_MATCHING PATTERN "*.h"
)
