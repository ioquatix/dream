#!/usr/bin/env ruby

require 'stringio'
require 'fileutils'
require 'set'

SRC_ROOT = ARGV[0]

identifiers = {}

def underscore_name(name)
	name.gsub(/[A-Z]+/, '_\0').downcase
end

WET = true

Dir.chdir(SRC_ROOT) do
	# Make a list of all symbols
	Dir["**/*.{cpp,h}"].each do |path|
		buffer = File.read(path)

		buffer.scan(/\b_?[a-z]+[A-Z]+[a-zA-Z]+/).each do |identifier|
			next if identifier =~ /^gl/

			identifiers[identifier] = underscore_name(identifier)
		end
		
		buffer.scan(/\b(m_([a-zA-Z0-9]+))/).each do |variable|
			$stderr.puts variable.inspect
			identifiers[variable[0]] = "_" + underscore_name(variable[1])
		end
	end
	
	identifier_regexp = Regexp.union(identifiers.keys.collect{|identifier| Regexp.escape(identifier)})

	$stderr.puts identifiers.keys.join("\n")

	Dir["**/*.{cpp,h}"].each do |path|
		buffer = File.read(path)
		
		buffer.gsub!(identifier_regexp){|name|identifiers[name]}
		
		if WET
			File.open(path, "w") {|file| file.write(buffer)}
		else
			#$stderr.puts(path.center(80, "-"))
			#$stderr.write(buffer)
		end
	end
end
