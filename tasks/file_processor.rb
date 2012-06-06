#!/usr/bin/env ruby

require 'stringio'
require 'fileutils'
require 'digest/md5'

class FileProcessor
	def initialize(paths = nil)
		@dry = true
		@buffers = {}
		@checksums = {}
		
		if paths
			add_paths(paths)
		end
	end
	
	def wet!
		@dry = false
	end
	
	def self.checksum_text(text, ignore_whitespace = true)
		if ignore_whitespace
			text = text.gsub(/\/\/.*$/, "")
			text = text.gsub(/(\/\*)(.*?)(\*\/)/m, "")
			text = text.gsub(/\s+/, "")
		end

		Digest::MD5.hexdigest(text)
	end

	def self.checksum_lines(lines, ignore_whitespace = true)
		checksum_text(lines.join("\n"), ignore_whitespace)
	end

	def self.checksum_buffer(buffer, ignore_whitespace = false)
		buffer.seek(0)
		checksum = checksum_text(buffer.read, ignore_whitespace)
		buffer.seek(0)

		return checksum
	end
	
	def add(path)
		@buffers[path] = StringIO.new(File.read(path))
		@checksums[path] = self.class.checksum_buffer(@buffers[path])
	end
	
	def add_paths(paths)
		paths.each do |path|
			add(path)
		end
	end
	
	def process_buffers
		@buffers.dup.each do |path, buffer|
			buffer.seek(0)
			result = yield path, buffer
			
			if result
				@buffers[path] = result
			end
		end
	end
		
	def process_lines
		@buffers.dup.each do |path, buffer|
			buffer.seek(0)
			lines = buffer.readlines
			lines.each{|line| line.chomp!}
			
			result = yield path, lines
			
			if result
				@buffers[path] = StringIO.new(result.join("\n"))
			end
		end
	end
	
	def save!
		tmp_path = File.join("/tmp/normalize-#{Time.now.to_s}/")
		puts "Backups written to #{tmp_path}"

		process_buffers do |name, buffer|
			if @checksums[name] == self.class.checksum_buffer(buffer)
				next
			end
			
			puts " #{name.dump} ".center(80, "-")
			
			buffer.seek(0)
			
			IO.popen("diff #{name.dump} -", "w") do |io|
				io.write(buffer.read)
				io.close_write
			end
			
			buffer.seek(0)
			
			if @dry
				$stdout.write(buffer.read)
			else
				backup_path = File.join(tmp_path, name)
				FileUtils.mkdir_p(File.dirname(backup_path))
				FileUtils.cp(name, backup_path)

				File.open(name, "w") { |f| f.write(buffer.read) }
			end
		end
	end
end
