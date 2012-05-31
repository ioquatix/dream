
#
#  ext/platforms/darwin_iphonesimulator.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:darwin_iphonesimulator) do |config|
	iphone_sdk_version = ENV["IPHONE_SDK_VERSION"] || "5.0" 
	
	config.platform = Pathname.new("/Developer/Platforms/iPhoneSimulator.platform")
	config.sdk_version = iphone_sdk_version
	config.sdk = config.platform + "Developer/SDKs/iPhoneSimulator#{config.sdk_version}.sdk"
	
	config.arch = "-arch i386"
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -miphoneos-version-min=#{config.sdk_version} -mdynamic-no-pic"
	config.configure = []
	
	config.build_environment = {
		"CC" => config.platform + "Developer/usr/bin/clang",
		"CXX" => config.platform + "Developer/usr/bin/clang++",
		# Seems to have disappeared?
		# "CPP" => config.platform + "Developer/usr/bin/cpp",
		"LD" => config.platform + "Developer/usr/bin/ld",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = PLATFORM.include?("darwin")
end
