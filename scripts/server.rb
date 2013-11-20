#!/usr/bin/ruby
require 'fileutils'

FileUtils.cd 'build'

built = system 'make'

if built == true
  exec "#{Dir.pwd}/crails-app", *ARGV
else
  puts "The server won't build. Please fix the issues and try again."
end
