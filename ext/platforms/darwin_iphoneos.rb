
Platform.new(:darwin_iphoneos) do |config|
	config.platform = Pathname.new("/Developer/Platforms/iPhoneOS.platform")
	config.sdk_version = IPHONE_SDK_VERSION
	config.sdk = config.platform + "Developer/SDKs/iPhoneOS#{config.sdk_version}.sdk"
	
	# Add -mthumb to config.arch if desired
	# config.arch = "-arch armv6 -arch armv7"
	
	# Apparently this now builds for armv6 _and_ armv7?
	config.arch = "-arch armv7"
	
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -miphoneos-version-min=#{config.sdk_version} -mdynamic-no-pic"
	config.configure = ["--host=arm-apple-darwin"]
	
	# SDK Version 3.2 ?, version 3.1 used darwin9
	# cc = "Developer/usr/bin/arm-apple-darwin10-gcc-4.0.1"
	
	config.build_environment = {
		# This line must be a compiler that will produce ARM code.
		"CC" => config.platform + "Developer/usr/bin/gcc",
		"CPP" => config.platform + "Developer/usr/bin/cpp",
		"LD" => config.platform + "Developer/usr/bin/ld",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags}"
	}
end
