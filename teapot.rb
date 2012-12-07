
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

required_version "0.5"

define_target "dream" do |target|
	target.install do |environment|
		top = Teapot::Build.top(Pathname.new(__FILE__).dirname)

		top.add_directory('source')
		top.add_directory('test')
		
		top.execute(:install, environment)
	end
	
	target.depends :platform
	
	target.depends "Library/png"
	target.depends "Library/jpeg"
	target.depends "Library/freetype"
	target.depends "Library/ogg"
	
	target.depends "Language/C++11"
	target.depends "Library/UnitTest"
	target.depends "Library/Euclid"
	
	target.provides "Library/Dream" do
		append linkflags "-lDream"
	end
end
