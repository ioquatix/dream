#!/usr/bin/env ruby

require 'stringio'
require 'fileutils'

SRC_ROOT = File.join(File.dirname(__FILE__), "src")
INC_ROOT = File.join(File.dirname(__FILE__), "include")

def execute(*args)
  puts " *** " + args.join(" ")
  system(*args)
end

moves = []
Dir.chdir(SRC_ROOT) do
  Dir["**/*.{h}"].each do |header|
    dst = File.join(INC_ROOT, header)
    moves << [File.join(SRC_ROOT, header), dst]
  end
end

moves.each do |src, dst|
  puts "Moving #{src} to #{dst}"
  FileUtils.mkdir_p(File.dirname(dst))
  execute("git", "mv", src, dst)
end

