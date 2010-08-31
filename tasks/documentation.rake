
namespace :dream do
	namespace :docs do
		task :generate do
			sh("doxygen")
		end
	end
end