
# The standard libraries to link with
set(CORELIBS stdc++ freetype jpeg png ogg vorbis)

# % find . -name "*.cpp"
add_library(dream STATIC
	./Assertion.cpp
	./Class.cpp
	./Client/Audio/Mixer.cpp
	./Client/Audio/OggResource.cpp
	./Client/Audio/Sound.cpp
	./Client/Audio/Stream.cpp
	./Client/Display/Application.cpp
	./Client/Display/Context.cpp
	./Client/Display/Scene.cpp
	./Client/Graphics/Graphics.cpp
	./Client/Graphics/ShaderManager.cpp
	./Client/Graphics/TextureManager.cpp
	./Core/Buffer.cpp
	./Core/Data.cpp
	./Core/Dictionary.cpp
	./Core/Endian.cpp
	./Core/ObjectSet.cpp
	./Core/Path-Unix.cpp
	./Core/Path.cpp
	./Core/Serialization.cpp
	./Core/Strings.cpp
	./Core/System.cpp
	./Core/Timer-Unix.cpp
	./Core/Timer.cpp
	./Core/URI.cpp
	./Core/Value.cpp
	./Events/Console.cpp
	./Events/Fader.cpp
	./Events/Input.cpp
	./Events/Loop.cpp
	./Events/MultiFingerInput.cpp
	./Events/Source.cpp
	./Events/Thread.cpp
	./Framework.cpp
	./Geometry/AlignedBox.cpp
	./Geometry/Frustum.cpp
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
	./Resources/Loader-Generic.cpp
	./Resources/Loader.cpp
	./Simulation/AlignedTree.cpp
	./Simulation/PathFinder.cpp
	./Text/Font.cpp
	./Text/FontFace.cpp
	./Text/TextBlock.cpp
	./Text/TextBuffer.cpp
)

target_link_libraries(dream ${CORELIBS})

install(
	TARGETS
		dream
	ARCHIVE DESTINATION
		lib
)
