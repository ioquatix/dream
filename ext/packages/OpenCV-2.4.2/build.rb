
Package.define("OpenCV-2.4.2") do |package|
	package.variant(:all) do |platform, config|
		RExec.env(config.build_environment) do
			Dir.chdir(package.src) do
				sh("rm", "-rf", "build")
				sh("mkdir", "build")
				
				Dir.chdir("build") do
					sh("cmake", "-G", "Unix Makefiles", "-DCMAKE_INSTALL_PREFIX:PATH=#{platform.prefix}", "-DCMAKE_CXX_COMPILER_WORKS=TRUE", "-DCMAKE_C_COMPILER_WORKS=TRUE", "-DBUILD_opencv_legacy=OFF", "-DBUILD_opencv_nonfree=OFF", "-DBUILD_opencv_ts=OFF", "-DBUILD_opencv_ts=OFF", "-DBUILD_opencv_highgui=OFF", "-DBUILD_SHARED_LIBS=OFF", "..")
					
					sh("make")
					sh("make", "install")
				end
			end
		end
	end
end
