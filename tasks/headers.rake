
PRIVATE_HEADERS_YAML = File.join(SOURCE_PATH, "private-headers.yaml")

namespace :dream do
	task :copy_headers do
		private_headers = YAML.load_file(PRIVATE_HEADERS_YAML)
		public_path = INCLUDE_PATH

		updated = lambda do |path|
			src_path = path
			dst_path = File.join(public_path, path)

			# If the file doesn't even exist... or check if the src has been updated
			!File.exist?(dst_path) || File.mtime(src_path) > File.mtime(dst_path)
		end

		Dir.chdir(SOURCE_PATH) do
			Dir["**/*.{h}"].each do |path|
				next if private_headers.include?(path)

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