#!/usr/bin/ruby

command = ARGV[0]
_module = ARGV[1]
options = ARGV[2..-1]

module_script_dir = ENV['CRAILS_SHARED_DIR'] + '/scripts/modules/' + _module

unless File.directory? module_script_dir
  puts "Could not find a crails module named `#{_module}`"
  exit -1
end

unless File.exists? "#{module_script_dir}/#{command}.rb"
  puts "Module `#{_module}` does not implement any command named `#{command}`"
  exit -2
end

system "ruby #{module_script_dir}/#{command}.rb #{options.join ' '}"
