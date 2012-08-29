#!/usr/bin/env ruby

#	This file is part of the "Utopia Framework" project, and is released under the MIT license.
#	Copyright 2010 Samuel Williams. All rights reserved.
#	See <utopia.rb> for licensing details.

require '../tasks/file_processor.rb'

class String
	HTML_ESCAPE = {"&" => "&amp;", "<" => "&lt;", ">" => "&gt;", "\"" => "&quot;"}
	HTML_ESCAPE_PATTERN = Regexp.new("[" + Regexp.quote(HTML_ESCAPE.keys.join) + "]")

	def to_html
		gsub(HTML_ESCAPE_PATTERN){|c| HTML_ESCAPE[c]}
	end

	def to_quoted_string
		'"' + self.gsub('"', '\\"').gsub(/\r/, "\\r").gsub(/\n/, "\\n") + '"'
	end

	def to_title
		self.gsub(/(^|[ \-_])(.)/){" " + $2.upcase}.strip
	end

	def to_snake
		self.gsub("::", "").gsub(/([A-Z]+)/){"_" + $1.downcase}.sub(/^_+/, "")
	end
end

require 'fileutils'

def to_lower_case(path)
	components = path.split('/')
	
	components.collect! do |name|
		name.to_snake
	end
	
	components.join('/')
end

def process_filenames
	Dir['**/*'].each do |path|
		if path != to_lower_case(path)
			FileUtils.mv path, to_lower_case(path)
		end
	end
end


processor = FileProcessor.new(Dir['**/*'].select{|path|!File.directory?(path)})

processor.process_lines do |path, lines|
	lines.collect! do |line|
		if line.match(/\#include \"(.*?)\"/)
			"\#include \"#{to_lower_case($1)}\""
		else
			line
		end
	end
	
	lines
end

processor.save!
