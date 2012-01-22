
Platform.new(:android_ndk) do |config|
	config.platform = PLATFORMS_PATH + "android-ndk-r7-crystax-4"
	config.sdk = config.platform + "platforms/android-14/arch-arm"
	
	# This needs to be modified for the platform you are building from:
	config.toolchain = config.platform + "toolchains/arm-linux-androideabi-4.6.3/prebuilt/darwin-x86/bin/"
	
	config.bin_prefix = "arm-linux-androideabi"
	
	toolchain_bin = lambda do |executable|
		config.toolchain + (config.bin_prefix + "-" + executable)
	end
	
	config.arch = "" # "-arch arm"
	config.crystax_lib_path = config.platform + "sources/crystax/libs/armeabi/4.6.3/"
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
