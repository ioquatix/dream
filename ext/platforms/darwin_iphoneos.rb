
#
#  ext/platforms/darwin_iphoneos.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:darwin_iphoneos) do |config|
	iphone_sdk_version = ENV["IPHONE_SDK_VERSION"] || "5.0" 
	
	config.platform = Pathname.new("/Developer/Platforms/iPhoneOS.platform")
	config.sdk_version = iphone_sdk_version
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
		"CC" => config.platform + "Developer/usr/bin/clang",
		"CXX" => config.platform + "Developer/usr/bin/clang++",
		# Seems to have disappeared?
		# "CPP" => config.platform + "Developer/usr/bin/clang",
		"LD" => config.platform + "Developer/usr/bin/ld",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = PLATFORM.include?("darwin")
end
