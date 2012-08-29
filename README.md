# Dream #

* Author: Samuel G. D. Williams (<http://www.oriontransfer.co.nz>)
* Copyright (C) 2006, 2011 Samuel G. D. Williams.
* Released under the MIT license.

Dream is a cross-platform game development framework. It is currently under heavy development. It has the following features:

* Reference counted pointers for resource/memory management.
* Event driven networking and rendering engine.
* Vector, Matrix and Quaternion implementation for mathematics.
* Sphere, Line, Plane, Frustrum, AlignedBox, Triangle for geometry calculations.
* Robust message based network framework.
* Loading and saving images (PNG, JPEG) of various channels and bit depths.
* Flexible text rendering using FreeType2.
* Positional audio using using OpenAL and Ogg Vorbis.
* High performance graphics using OpenGL (Support for OpenGL3.2+ and OpenGL2.0ES+)

For documentation and examples please see the main [project page][1].

[1]: http://www.oriontransfer.co.nz/projects/dream

## Documentation ##

To generate documentation, make sure you have `doxygen` installed and run:

	rake dream:documentation:generate
	
The documentation can be found in `docs/html/index.html`.

### Compiling examples on Mac OS X ###

Firstly, execute the following using Terminal.app:

	cd $DREAM/ext
	rake fetch
	rake build platform=dream_darwin

Secondly, open Dream.xcodeproj, select "Dream-Darwin-MacOSX" target and build.

Finally, open Demo.xcodeproj, select "Demo" target and run.

### Compiling examples on Linux ###

Firstly, execute the following commands using a shell:

	cd $DREAM/ext
	rake fetch
	rake build platform=linux

Secondly, build with CMake:

	cd $DREAM
	mkdir build
	cd build
	CC=clang CXX=clang++ cmake ..
	make

## Compatibility ##

This framework has been designed for cross-platform compilation. It currently has support for the following targets:

* Mac OS X [Full]
* iPhone [Full]
* Linux [Partial]
* Android NDK [Partial]
* Windows [Unsupported at this time]

Dream uses features from C++11, and therefore requires a C++11 compiler such as [clang][1] or [gcc][2].

[1]: http://clang.llvm.org/
[2]: http://gcc.gnu.org/gcc-4.6/

### Linux Build ###

The bare minimum to build on linux:

	sudo apt-get install rake curl zlibg1-dev
	sudo gem install rexec
	
	cd $DREAM/ext
	rake
	
	mkdir $DREAM/build
	cd $DREAM/build
	cmake ../
	make

## Coding Guidelines ##

### File Variants ###

Depending on the platform, there may be different files available to build the required functionality. These will typically only be implementation files. For example:

* `Core/Timer.h`
* `Core/Timer-CoreVideo.cpp`
* `Core/Timer-Unix.cpp`

Only one of these implementations need to be compiled for a specific platform. Generally, there will be specific implementations (`Core/Timer-CoreVideo.cpp`) and generic implementations (`Core/Timer-Unix.cpp`). You should generally compile the most specific implementation for a platform, rather than the generic implementation.

### Resource Management ###

One feature of the Dream framework is automatic resource/memory management. This is done by using Ref counted pointers. It is important to understand a few things about the semantics of these "values" in order to get the best performance.

When an object has ownership of a resource, it should use a `Dream::Ref<Object>`. Ownership is typically referred to as a <em>has-a</em> or <em>has-many</em> relationship.

When passing objects to a function, you don't need to incur the performance cost of memory management. Therefore, you should use `Dream::Ptr<Object>`. Return values should still be `Dream::Ref<Object>`. Return value optimization ensures that reference counting overhead is minimized.

## Related Links ##

* [Perspective Correct Texturing][http://chrishecker.com/Miscellaneous_Technical_Articles]

## License ##

Copyright (c) 2006, 2011 Samuel G. D. Williams. <http://www.oriontransfer.co.nz>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
