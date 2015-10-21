#!/usr/bin/ruby
require 'fileutils'

FileUtils.cd 'build'

built = system 'make'

if built == true
  FileUtils.cd '..'
  exec "#{Dir.pwd}/build/server", *ARGV
else
  puts "The server won't build. Please fix the issues and try again."
end
