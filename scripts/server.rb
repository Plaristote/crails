#!/usr/bin/ruby
built = system 'crails compile'

if built == true
  exec "#{Dir.pwd}/build/server", *ARGV
else
  puts "The server won't build. Please fix the issues and try again."
end
