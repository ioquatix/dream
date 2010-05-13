
namespace :dream do
	task :copy_headers do
		files = YAML::load(File.read(HEADERS_YAML))
		public_path = INCLUDE_PATH

		updated = lambda do |path|
			src_path = path
			dst_path = File.join(public_path, path)

			# If the file doesn't even exist... or check if the src has been updated
			!File.exist?(dst_path) || File.mtime(src_path) > File.mtime(dst_path)
		end

		Dir.chdir(SOURCE_PATH) do
			files.each do |path|
				dir = File.dirname(path)

				if updated.call(path)
					puts "Copying #{path.dump}"
					FileUtils.mkdir_p(File.join(public_path, dir))
					FileUtils.cp(path, File.join(public_path, path))
				end
			end
		end
	end
end