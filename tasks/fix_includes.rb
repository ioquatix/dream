#!/usr/bin/env ruby

require 'stringio'
require 'fileutils'

Buffers = {}
Index = {}

Dir.chdir("src") do
  Files = Dir["**/*.{cpp,h}"]
  
  Files.each do |f|
    Buffers[f] = StringIO.new(File.read(f))
  end
end

Dir.chdir("headers") do
  Headers = Dir["**/*.h"]

  Headers.each do |f|
    Index[File.basename(f)] = f
  end
end

puts "Source: #{Files.inspect}"
puts "Headers: #{Headers.inspect}"

Wet = true

def implementation_header_path(name, full_path)
  return ("../" * (name.split("/").size + 1) + "headers/" + full_path).dump
end

Buffers.each do |name, buf|
  puts "Processing #{name}..."
  buf.seek(0)
  lines = buf.readlines
  result = []
  
  lines.each do |line|
    if line.match(/^\#include "(.*)"$/)
      path = $1
      
      include_name = File.basename(path)
      full_path = Index[include_name]
      
      if (full_path)
        new_path = implementation_header_path(name, full_path)
        
        if (path != new_path)
          path = new_path
          puts "#{name}: #{line.strip} => \#include #{path}"
        end
      end
      
      result << "\#include #{path}"
    else
      result << line.chomp
    end
  end
  
	Buffers[name] = StringIO.new(result.join("\n"))
end

Buffers.each do |name, buf|
	buf.seek(0)
	
	tmp_path = "/tmp/src-backup/"
	
	if Wet
		backup_path = File.join(tmp_path, name)
		FileUtils.mkdir_p(File.dirname(backup_path))
		FileUtils.cp(name, backup_path)
	
	  puts "Writing to file #{File.expand_path(name)}"
		File.open(name, "w") do |f| 
		  f.write(buf.read.strip)
		  f.puts
	  end
	else
		$stdout.puts(" #{name} ".center(80, "-"))
		$stdout.write(buf.read)
	end
end
