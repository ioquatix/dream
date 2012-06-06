#!/usr/bin/env ruby

# How to use:
# PROCESSOR=wet ./fix_headers.rb ../src/ "**/*.{h,cpp}"
# This command rewrites all header comments (e.g. the first 10 lines of source code files)
# It will stuff up files which don't ha
require 'file_processor.rb'

Dir.chdir(ARGV[0])

$processor = FileProcessor.new(Dir[ARGV[1]])

if ENV['PROCESSOR'] == 'wet'
	$processor.wet!
end

$processor.process_lines do |path, lines|
	result = []
	
	header = lines[0].match(/\/\*/) != nil
	
	lines.each_with_index do |line, index|
		if header
			if line.match(/\*\//)
				result << "//"
				header = false
			elsif line.match(/^[\/ ]\*(.*)$/)
				result << "//#{$1}"
			end
		else
			result << line
		end
	end
	
	result
end

$processor.process_lines do |path, lines|
	result = []
	
	if lines[0] == "//" && lines[1] =~ "//"
		lines[1] = "//  #{path}"
		lines[2] = "//  This file is part of the \"Dream\" project, and is released under the MIT license."
	end
	
	# Make sure the last line is blank.
	if lines.last != ""
		lines << ""
	end
	
	lines
end

$processor.save!