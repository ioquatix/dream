
BOOST_PACKAGE = Pathname.new(__FILE__).dirname
BOOST_IPHONE_CONFIG = BOOST_PACKAGE + "iphone.jam"
BOOST_MODULES = ["--with-filesystem", "--with-thread", "--with-system"]

Package.define("boost_1_43_0") do |package|
	bootstrap_bjam = lambda do
		Dir.chdir(package.src) do
			sh("./bootstrap.sh") unless File.exist? "bjam"
		end
		
		return package.src + "bjam"
	end
	
	package.variant(:all) do |platform, config|
		bjam = bootstrap_bjam.call
		
		RExec.env(config.build_flags) do
			Dir.chdir(package.src) do
				args = ["--prefix=#{platform.prefix}", "--build-type=minimal"]
				args += BOOST_MODULES
				args += ["link=static", "threading=multi", "architecture=combined", "address-model=32_64"]
				args << "install"
				
				sh(bjam, *args)
			end
		end
	end
	
	package.variant(:darwin_iphoneos) do |platform, config|
		bjam = bootstrap_bjam.call
		
		RExec.env(config.build_flags) do
			Dir.chdir(package.src) do
				args = ["--prefix=#{platform.prefix}", "--build-type=minimal"]
				args += ["--user-config=#{BOOST_IPHONE_CONFIG}"]
				args += BOOST_MODULES
				args += ["link=static", "threading=multi", "architecture=arm"]
				args += ["target-os=iphone", "macosx-version=iphone-#{config.sdk_version}"]
				args << "install"
				
				sh(bjam, *args)
			end
		end
	end
	
	package.variant(:darwin_iphonesimulator) do |platform, config|
		bjam = bootstrap_bjam.call

		RExec.env(config.build_flags) do
			Dir.chdir(package.src) do
				args = ["--prefix=#{platform.prefix}", "--build-type=minimal"]
				args += ["--user-config=#{BOOST_IPHONE_CONFIG}"]
				args += BOOST_MODULES
				args += ["link=static", "threading=multi", "architecture=x86"]
				args += ["target-os=iphone", "macosx-version=iphonesim-#{config.sdk_version}"]
				
				# Fix a bug when compiling C++ code with SDK 3.2
				args += ["include=#{config.sdk}/usr/include/c++/4.2.1/armv6-apple-darwin9/"]
				
				args << "install"
				
				sh(bjam, *args)
			end
		end
	end
end
