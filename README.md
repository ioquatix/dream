Dream
=====

* Author: Samuel G. D. Williams (<http://www.oriontransfer.co.nz>)
* Copyright (C) 2006, 2011 Samuel G. D. Williams.
* Released under the MIT license.

Dream is a cross-platform game development framework. It is currently under heavy development. It has the following features:

* Reference counted pointers for memory management.
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

Dependencies
------------

The following external dependencies are included and compiled statically for all supported platforms:

* libjpeg
* libpng
* libfreetype
* libvorbis
* libogg

For more information, see the `ext/README.md` directory.

Compatibility
-------------

This framework has been designed for cross-platform compilation. It currently has support for the following targets:

* Mac OS X [Full]
* iPhone [Full]
* Linux [Partial]
* Android NDK [Partial]
* Windows [Unsupported at this time]

Dream uses features from C++11, and therefore requires a C++11 compiler such as [clang][1] or [gcc][2].

[1]: http://clang.llvm.org/
[2]: http://gcc.gnu.org/gcc-4.6/

License
-------

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
