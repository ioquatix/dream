
#
#  ext/platforms/darwin_osx.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:darwin_osx) do |config|
	macosx_sdk_version = ENV["MACOSX_SDK_VERSION"] || "10.7"
	
	config.xcode_path = Pathname.new(`xcode-select --print-path`.chomp)
	config.platform = config.xcode_path + "Platforms/MacOSX.platform"
	config.toolchain = config.xcode_path + "Toolchains/XcodeDefault.xctoolchain"
	
	config.sdk_version = macosx_sdk_version
	config.sdk = config.platform + Pathname.new("Developer/SDKs/MacOSX#{config.sdk_version}.sdk")
	
	config.arch = "-arch i386 -arch x86_64"
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -mmacosx-version-min=#{config.sdk_version} -mdynamic-no-pic"
	config.configure = []
	
	config.build_environment = {
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"CXXFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags} -std=c++0x -stdlib=libc++ -Wno-c++11-narrowing",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = RUBY_PLATFORM.include?("darwin")
end
