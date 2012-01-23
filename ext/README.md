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

Platforms
---------

Platforms are specific configurations for compiling external dependencies. Unfortunately, since most dependencies use `autoconf`, we need to work hard to setup the correct build environment to ensure that libraries are compiled properly. Feel free to adjust the platform configuration files for your own requirements.

### Darwin ###

At this time, Mac OS X 10.7 is required to build `darwin_*` platforms. 

### Android NDK ###

You will need to download the [android_ndk][1] (only the latest version is supported) into platforms in order for this to work.

[1]: http://www.crystax.net/android/ndk

Packages
--------

A variety of packages required by Dream are automatically downloaded. The list of URLs is stored in `fetch.yaml`.
