
namespace :dream do
	namespace :documentation do
		task :generate do
			sh("doxygen")
		end
		
		task :open => :generate do
			sh("open docs/html/index.html")
		end
	end
end