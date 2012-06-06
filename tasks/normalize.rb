#!/usr/bin/env ruby

require 'stringio'
require 'fileutils'
require 'digest/md5'

Wet = false
DefinitionTypes = ['class', 'struct', 'namespace', 'enum']

class String
	def begins_with?(c)
		r = index(c)
		
		# True if begins with the sequence or doesn't include the sequence
		return !(r == nil || (r != nil && r == 0))
	end
	
	def remove_quoted_strings
		gsub(/\"[^\"]+\"/, "")
	end
	
	def code_before_comment?
		flat = lstrip.remove_quoted_strings
		
		return true if flat.begins_with?("//") || flat.begins_with?("/*")
	end
	
	def pad_symbols!(lop, rop = nil)
		rop ||= lop
		
		gsub!(/([^ \(<])#{Regexp.escape(lop)}/) { "#{$1} #{lop}" }
		gsub!(/#{Regexp.escape(rop)}([^ \)>,])/) { "#{rop} #{$1}" }
	end
end

Files = Dir[ARGV[0]]
Buffers = {}

def checksum_text(text, ignore_whitespace = true)
	if ignore_whitespace
		text = text.gsub(/\/\/.*$/, "")
		text = text.gsub(/(\/\*)(.*?)(\*\/)/m, "")
		text = text.gsub(/\s+/, "")
	end
	
	Digest::MD5.hexdigest(text)
end

def checksum_lines(lines, ignore_whitespace = true)
	checksum_text(lines.join("\n"), ignore_whitespace)
end

def checksum_buffer(buf, ignore_whitespace = false)
	buf.seek(0)
	cksum = checksum_text(buf.read, ignore_whitespace)
	buf.seek(0)
	
	return cksum
end

Files.each do |f|
	Buffers[f] = StringIO.new(File.read(f))
end

InitialChecksums = {}

puts "Pass 1: Initial Statistics"
Buffers.each do |name, buf|
	buf.seek(0)
	bytes = buf.read.size
	buf.seek(0)
	lines = buf.readlines.size
	
	InitialChecksums[name] = checksum_buffer(buf)
	
	puts "#{name.rjust(60)}: #{bytes.to_s.rjust(8)} bytes, #{lines.to_s.rjust(8)} lines, #{sprintf("%0.2f", (bytes.to_f / lines.to_f)).rjust(8)} bytes per line"
end

def fix_code_blocks(lines)
	result = []
	
	lines.each do |l|
		l.rstrip!
		processed = false
		
		if l.match(/^(\s*)(.*)\{\s*([^\s]+.*)$/)
			result << "#{$1}#{$2}"
			result << "#{$1}{"
			result << "#{$1}\t#{$3}"
		elsif l.match(/(\s*)([^\s]+.*)(\}.*;)$/)
			result << "#{$1}#{$2}"
			result << "#{$1.chop}#{$3}"
			result << ""
		elsif l.match(/(\s*)([^\s]*.*)\}([^;]+)$/)
			result << "#{$1}#{$2}"
			result << "#{$1}}"
			result << "#{$1}#{$3.strip}"
		else
			result << l unless processed
		end
	end
	
	return result
end

#puts "Pass 1.5: Fixing code blocks"
#Buffers.each do |name, buf|
#	buf.seek(0)
#	lines = buf.readlines
#	
#	count = 0
#	
#	cur_cksum = checksum_buffer(buf, false)
#	while true
#		lines = fix_code_blocks(lines)
#		
#		updated_cksum = checksum_lines(lines, false)
#		
#		break if cur_cksum == updated_cksum
#		
#		count += 1
#		
#		cur_cksum = updated_cksum
#	end
#	
#	$stdout.puts "#{name.rjust(30)}: #{count} updates" if count > 0
#	
#	Buffers[name] = StringIO.new(lines.join("\n"))
#end

puts "Pass 2: Syntax check and adjustment"
Buffers.each do |name, buf|
	blocks = []
	current_block = :global
	
	buf.seek(0)
	lines = buf.readlines
	
	comment_block = false
	comment_block_line = nil
	indentation_level = ""
	
	line_no = 0
	
	lines.collect! do |l|
		line_no += 1
		
		$stderr.puts "Comment is after code; <#{name}:#{line_no}>: #{l}" if l.code_before_comment? && !comment_block
		$stderr.puts "C-style comment on a single line; <#{name}:#{line_no}>: #{l}" if l.match(/\/\*.*?\*\//)
		
		comment = l.lstrip.match(/\/\//) != nil
		comment_block_line = comment_block ? :intermediate : nil
		
		# Match the start of a C-style comment block indented by tabs
		if l.match(/^(\s)*\/\*/) && !comment_block
			comment_block = true
			comment_block_line = :first
			indentation_level = $1
		end
		
		unless comment || comment_block
			DefinitionTypes.each do |d|
				current_block = d.to_sym if l.remove_quoted_strings.match(/#{d}/)
			end
			
			# Entering block
			if l.match(/\{/)
				blocks << current_block
				current_block = nil
			end
			
			# Are we outside a function
			if blocks.last != nil
				l.pad_symbols!("(", ")")
				
				l.pad_symbols!("*")
				l.pad_symbols!("&")
			else
				# Remove all whitespace around brackets
				l.gsub!(/\s+\(\s+/, "(")
				l.gsub!(/\s+\)\s+/, ")")
			end
			
			l.gsub!(/,([^ ])/) { ", #{$1}" }
			
			# Finally, remove any gaps between ";"
			l.gsub!(/\s+;/, ";")
			
			l.gsub!(/^(\t*)(.*)\{/) { "#{$1}#{$2.rstrip}\n#{$1}{" }
			
			# Exiting current block
			if l.match(/\}/)
				blocks.pop
				current_block = nil
			end
		end
		
		if comment_block && l.match(/\*\//)
			comment_block = false
			comment_block_line = :end
		end
		
		if comment_block_line == :intermediate
			l.gsub!(/^#{indentation_level}\s\*?/, "#{indentation_level}\t")
		end
		
		l.rstrip
	end
	
	Buffers[name] = StringIO.new(lines.join("\n"))
end

puts "Pass 2.5: K&R style curly brackets"
Buffers.each do |name, buf|
	buf.seek(0)
	lines = buf.readlines
	result = []
	
	lines.each_with_index do |line, index|
		if result.last
			if result.last.match(/^\s+\}\s*$/) && line.match(/else|else if/)
				result[result.size-1] += " #{line}"
				next
			end
		
			if line.match(/^\s+\{\s*$/) && result.last.match(/if|while|for|else|do|switch/)
				result[result.size-1] += " {"
				next
			end
		end
			
		result << line
	end
	
	Buffers[name] = StringIO.new(result.join("\n"))
end



puts "Pass 3: Tidying up empty newlines and whitespace"
Buffers.each do |name, buf|
	buf.seek(0)
	lines = buf.readlines
	result = []
	
	lines.each_with_index do |line, index|
		blank_line = (line.strip.size == 0)
		if lines[index+1] && blank_line
			skip_line_nextline = false
			# Next line is blank
			skip_line_nextline ||= (lines[index+1].strip.size == 0)
			skip_line_nextline ||= (lines[index+1].match(/^(\s*)\{$/) != nil)
			skip_line_nextline ||= (lines[index+1].match(/^(\s*)\*\/$/) != nil)
			
			next if skip_line_nextline
		end
		
		if lines[index-1] && blank_line
			skip_line_nextline = false
			# Next line is blank
			skip_line_nextline ||= (lines[index-1].strip.size == 0)
			skip_line_nextline ||= (lines[index-1].match(/^(\s*)\{$/) != nil)
			
			next if skip_line_nextline
		end
		
		result << line.rstrip
	end
	
	result << ""
	
	Buffers[name] = StringIO.new(result.join("\n"))
end

puts "Pass 4: Checksums"
Buffers.each do |name, buf|
	cur_cksum = checksum_buffer(buf)
	
	status = (InitialChecksums[name] == cur_cksum ? "" : "X")
	
	if status == "X"
		buf.seek(0)
		puts "diff #{name.dump}..."
		IO.popen("diff #{name.dump} -", "w") do |io|
			io.write(buf.read)
			io.close_write
			
			# Write to stdout happens implicitly
		end
	end
	
	puts "#{name.rjust(30)}: #{InitialChecksums[name]} -> #{cur_cksum}\t\t#{status}"
end

puts "Pass 5: Dumping buffers"
tmp_path = File.join("/tmp/normalize-#{Time.now.to_s}/")
puts "Backups written to #{tmp_path}"

Buffers.each do |name, buf|
	buf.seek(0)
	
	if Wet
		backup_path = File.join(tmp_path, name)
		FileUtils.mkdir_p(File.dirname(backup_path))
		FileUtils.cp(name, backup_path)
	
		File.open(name, "w") { |f| f.write(buf.read) }
	else
		$stdout.puts(" #{name} ".center(80, "-"))
		$stdout.write(buf.read)	
	end
end
