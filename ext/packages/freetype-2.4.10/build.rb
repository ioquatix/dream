
Package.define("freetype-2.4.10") do |package|
	package.variant(:all) do |platform, config|
		RExec.env(config.build_environment) do
			Dir.chdir(package.src) do
				sh("make", "distclean") if File.exist? "Makefile"
				sh('rm', '-f', 'config.mk')
				sh("./configure", "--prefix=#{platform.prefix}", "--enable-shared=no", "--enable-static=yes", *config.configure)
				sh("make install")
			end
		end
	end
end
