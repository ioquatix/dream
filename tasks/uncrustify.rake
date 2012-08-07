
UNCRUSTIFY = Pathname.new(__FILE__).dirname + "../materials/uncrustify/src/uncrustify"
UNCRUSTIFY_CONFIG = Pathname.new(__FILE__).dirname + "uncrustify.config"

namespace :dream do
	task :format do
		Dir.chdir(SOURCE_PATH) do
			Dir["**/*.{cpp,h}"].each do |path|
				run UNCRUSTIFY, "-c", UNCRUSTIFY_CONFIG.to_s, "--replace", path
			end
		end
	end
end