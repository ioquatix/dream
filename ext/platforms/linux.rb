
#
#  ext/platforms/linux.rb
#  This file is part of the "Dream" project, and is released under the MIT license.
#

Platform.new(:linux) do |config|
	config.platform = Pathname.new("/")
	
	# PowerPC is no longer supported on the current architectures.
	# config.arch = "-arch ppc -arch i386 -arch x86_64"
	config.arch = "-arch i386 -arch x86_64"
	config.configure = []
	
	config.build_environment = {
		"CFLAGS" => "#{GLOBAL_CFLAGS} #{config.cflags}",
		"LDFLAGS" => "#{config.cflags}"
	}

	config.available = RUBY_PLATFORM.include?("linux") 
end
