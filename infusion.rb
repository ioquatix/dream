
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

required_version "0.2"
require 'teapot/build'

define_package "dream-0.2.0" do |package|
	package.depends = ["libogg", "freetype", "png", "jpeg", "unit-test", "euclid"]

	package.build(:all) do |platform, environment|
		top = Teapot::Build.top(Pathname.new(__FILE__).dirname)

		top.add_directory('source')
		top.add_directory('test')
		
		top.execute(:install, environment)
	end
end
