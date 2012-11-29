
class Component
	def initialize(root, name, environment)
		@root = root
		@name = name
		@environment = environment
		
		@parts = [@name]
	end
	
	attr :root
	attr :name
	attr :parts
	
	def add(path)
		@parts << path
	end
	
	def variant
		@environment[:variant]
	end
	
	def destination_path
		@environment[:build_prefix] + "components"
	end
	
	def prepare!
		source_path = destination_path + @name
		
		if source_path.exist?
			source_path.rmtree
		end
		
		source_path.mkpath
		
		@parts.each do |path|
			full_path = @root + path
			
			FileUtils.cp_r(full_path.children, source_path.to_s)
		end
		
		return source_path
	end
end

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
		
		puts "Sources in #{component.destination_path + 'Dream/**/*.{cpp,m,mm}'}"
		
		Pathname.glob(component.destination_path + 'Dream/**/*.{cpp,m,mm}')
	end
	
	def headers(environment)
		component = component(environment)
		
		Pathname.glob(component.destination_path + 'Dream/**/*.{h,hpp}')
	end
end
