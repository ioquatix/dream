Dream External Dependencies
===========================

Dream provides a simple package build system to build external dependencies as static libraries. This is the preferred option for supported platforms as it reduces the chance that problems will arise with library versions.

To list all available packages:

	% rake list
	(in /Users/samuel/Documents/Programming/Graphics/Dream/ext)
	Package: libpng
	Package: freetype
	Package: libvorbis
	Package: libogg
		(depends on libvorbis)
	Package: jpeg

To build a specific package use the following command:

	rake build package=freetype platform=android_ndk

Not all host platforms can build all packages. To build all packages for one platform:

	rake build platform=darwin_iphoneos

Darwin
------

At this time, Mac OS X 10.7 is required to build `darwin_*` platforms. 

Android NDK
-----------

You will need to download the [android_ndk][1] (only the latest version is supported) into platforms in order for this to work.

[1]: http://www.crystax.net/android/ndk
