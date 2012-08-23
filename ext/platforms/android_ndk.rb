
#
#  ext/platforms/android_ndk.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

# To use the android_ndk, you need to make sure that the directory "android-ndk-r7-crystax-4" is available by default.

Platform.new(:android_ndk) do |config|
	# You can override these parameters by specifying appropriate environment variables; here are the defaults:
	android_ndk_platform = ENV["ANDROID_NDK_PLATFORM"] || "android-ndk-r7-crystax-4"
	android_ndk_sdk = ENV["ANDROID_NDK_SDK"] || "android-14"
	android_ndk_toolchain = ENV["ANDROID_NDK_TOOLCHAIN"] || "arm-linux-androideabi-4.6.3"

	android_ndk_build = ENV["ANDROID_NDK_BUILD"]
	if RUBY_PLATFORM =~ /darwin/
		android_ndk_build ||= "darwin-x86"
	elsif RUBY_PLATFORM =~ /linux/
		android_ndk_build ||= "linux-x86"
	end
	
	config.platform = PLATFORMS_PATH + android_ndk_platform
	config.sdk = config.platform + "platforms/#{android_ndk_sdk}/arch-arm"
	
	config.toolchain = android_ndk_toolchain
	config.toolchain_version = config.toolchain.split(/-/).last
		
	config.toolchain_bin = config.platform + "toolchains/#{config.toolchain}/prebuilt/#{android_ndk_build}/bin/"
	
	config.bin_prefix = "arm-linux-androideabi"
	
	toolchain_bin = lambda do |executable|
		config.toolchain_bin + (config.bin_prefix + "-" + executable)
	end
	
	# Because we hard code the compilers to use, we don't touch this (one might think it should be "-arch arm").
	config.arch = ""
	
	config.crystax_lib_path = config.platform + "sources/crystax/libs/armeabi/#{config.toolchain_version}/"
	config.cflags = "#{config.arch} -nostdlib -L#{config.sdk}/usr/lib -L#{config.crystax_lib_path}"
	config.configure = ["--host=arm-eabi"]
	
	config.build_environment = {
		"CC" => toolchain_bin["gcc"],
		"CXX" => toolchain_bin["g++"],
		"CPP" => toolchain_bin["cpp"],
		"LD" => toolchain_bin["ld"],
		"AR" => toolchain_bin["ar"],
		"STRIP" => toolchain_bin["strip"],
		"RANLIB" => toolchain_bin["ranlib"],
		"CPPFLAGS" => "-I#{config.sdk}/usr/include",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags} -Wl,-entry=main,-no-undefined,-rpath-link=#{config.sdk}/usr/lib",
		"LIBS" => "-lc -lcrystax_static -lstdc++ -lm -llog -lgcc -lgcc_eh -ldl"
	}
end
