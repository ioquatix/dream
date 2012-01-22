
Platform.new(:darwin_iphonesimulator) do |config|
	config.platform = Pathname.new("/Developer/Platforms/iPhoneSimulator.platform")
	config.sdk_version = IPHONE_SDK_VERSION
	config.sdk = config.platform + "Developer/SDKs/iPhoneSimulator#{config.sdk_version}.sdk"
	
	config.arch = "-arch i386"
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -miphoneos-version-min=#{config.sdk_version} -mdynamic-no-pic"
	config.configure = []
	
	config.build_environment = {
		"CC" => config.platform + "Developer/usr/bin/gcc",
		"CPP" => config.platform + "Developer/usr/bin/cpp",
		"LD" => config.platform + "Developer/usr/bin/ld",
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags}"
	}
end
