
Package.define("libvorbis-1.3.3") do |package|
	package.variant(:all) do |platform, config|
		RExec.env(config.build_environment) do
			Dir.chdir(package.src) do
				sh("make", "clean") if File.exist? "Makefile"
				
				# The default configure has problems when compiling with multiple architectures because of the following option..
				# so we remove it forcefully.
				sh("sed", "-i", "-e", "s/-force_cpusubtype_ALL//g", "configure", "configure.ac")
				
				sh("./configure", "--prefix=#{platform.prefix}", "--disable-dependency-tracking", "--enable-shared=no", "--enable-static=yes", *config.configure)
				sh("make install")
			end
		end
	end
end
