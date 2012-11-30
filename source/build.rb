
require 'teapot/build/component'

add_library 'Dream' do
	@variants = {}
	
	def component(environment)
		variant_name = environment[:variant]
		
		@variants.fetch(variant_name) do
			component = Component.new(root, "Dream", environment)
			
			component.add("Dream")
			
			case environment[:platform]
			when /darwin-osx/
				component.add("Dream-NSFileManager")
				component.add("Dream-CoreVideo")
				component.add("Dream-Cocoa")
			when /darwin-ios/
				component.add("Dream-NSFileManager")
				component.add("Dream-CoreVideo")
				component.add("Dream-UIKit")
			when /linux/
				component.add("Dream-Unix")
				component.add("Dream-X11")
			end
			
			component.prepare!
			
			@variants[variant_name] = component
		end
	end
	
	def sources(environment)
		component = component(environment)
		
		FileList[component.destination_path, 'Dream/**/*.{cpp,m,mm}']
	end
	
	def headers(environment)
		component = component(environment)
		
		FileList[component.destination_path, 'Dream/**/*.{h,hpp}']
	end
end
