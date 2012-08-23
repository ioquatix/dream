
#
#  ext/platforms/darwin_ios.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:darwin_ios) do |config|
	iphone_sdk_version = ENV["IPHONE_SDK_VERSION"] || "5.1" 
	
	config.xcode_path = Pathname.new(`xcode-select --print-path`.chomp)
	config.platform = config.xcode_path + "Platforms/iPhoneOS.platform"
	config.toolchain = config.xcode_path + "Toolchains/XcodeDefault.xctoolchain"
	
	config.sdk_version = iphone_sdk_version
	config.sdk = config.platform + "Developer/SDKs/iPhoneOS#{config.sdk_version}.sdk"
	
	# Add -mthumb to config.arch if desired
	# config.arch = "-arch armv6 -arch armv7"
	
	# Apparently this now builds for armv6 _and_ armv7?
	config.arch = "-arch armv7"
	
	# -mdynamic-no-pic
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -miphoneos-version-min=#{config.sdk_version}"
	config.configure = ["--host=arm-apple-darwin"]
	
	# SDK Version 3.2 ?, version 3.1 used darwin9
	# cc = "Developer/usr/bin/arm-apple-darwin10-gcc-4.0.1"
	
	config.build_environment = {
		# This line must be a compiler that will produce ARM code.
		"CC" => config.toolchain + "usr/bin/clang",
		"CXX" => config.toolchain + "usr/bin/clang++",
		"LD" => config.toolchain + "usr/bin/ld",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"CXXFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags} -std=c++0x -stdlib=libc++ -Wno-c++11-narrowing",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = RUBY_PLATFORM.include?("darwin")
end
