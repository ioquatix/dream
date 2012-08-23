
#
#  ext/platforms/darwin_ios_simulator.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:darwin_ios_simulator) do |config|
	iphone_sdk_version = ENV["IPHONE_SDK_VERSION"] || "5.1" 
	
	config.xcode_path = Pathname.new(`xcode-select --print-path`.chomp)
	config.platform = config.xcode_path + "Platforms/iPhoneSimulator.platform"
	config.toolchain = config.xcode_path + "Toolchains/XcodeDefault.xctoolchain"
	
	config.sdk_version = iphone_sdk_version
	config.sdk = config.platform + "Developer/SDKs/iPhoneSimulator#{config.sdk_version}.sdk"
	
	config.arch = "-arch i386"
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -miphoneos-version-min=#{config.sdk_version} -mdynamic-no-pic"
	config.configure = []
	
	config.build_environment = {
		"CC" => config.toolchain + "usr/bin/clang",
		"CXX" => config.toolchain + "usr/bin/clang++",
		"LD" => config.toolchain + "usr/bin/ld",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"CXXFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags} -std=c++0x -stdlib=libc++ -Wno-c++11-narrowing",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = RUBY_PLATFORM.include?("darwin")
end
