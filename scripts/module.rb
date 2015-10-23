#!/usr/bin/ruby

_module = ARGV[0]
command = ARGV[1]
options = ARGV[2..-1]

if _module.nil?
  puts 'Available modules:'
  Dir["#{ENV['CRAILS_SHARED_DIR']}/scripts/modules/*"].each do |cmd|
    module_name = cmd.scan(/([^\/]+)$/).flatten.first
    puts " - #{module_name}"
  end
else
  module_script_dir = ENV['CRAILS_SHARED_DIR'] + '/scripts/modules/' + _module

  unless File.directory? module_script_dir
    puts "Could not find a crails module named `#{_module}`"
    exit -1
  end

  if command.nil?
    puts "Available commands for module #{_module}:"
    Dir["#{ENV['CRAILS_SHARED_DIR']}/scripts/modules/#{_module}/*"].each do |cmd|
      cmd_name = cmd.scan(/([^\/]+)[.]rb$/).flatten.first
      puts " - #{cmd_name}"
    end
  else
    unless File.exists? "#{module_script_dir}/#{command}.rb"
      puts "Module `#{_module}` does not implement any command named `#{command}`"
      exit -2
    end

    system "ruby #{module_script_dir}/#{command}.rb #{options.join ' '}"
  end
end
