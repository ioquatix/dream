
#
#  ext/platforms/darwin_macosx.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:darwin_macosx) do |config|
	macosx_sdk_version = ENV["MACOSX_SDK_VERSION"] || "10.7"
	
	config.platform = Pathname.new("/")
	config.sdk_version = macosx_sdk_version
	config.sdk = config.platform + Pathname.new("Developer/SDKs/MacOSX#{config.sdk_version}.sdk")
	
	# PowerPC is no longer supported on the current architectures.
	# config.arch = "-arch ppc -arch i386 -arch x86_64"
	config.arch = "-arch i386 -arch x86_64"
	config.cflags = "#{config.arch} -isysroot #{config.sdk} -mmacosx-version-min=#{config.sdk_version} -mdynamic-no-pic"
	config.configure = []
	
	config.build_environment = {
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = PLATFORM.include?("darwin")
end
