#!/usr/bin/ruby

require 'optparse'

options = {
    actions: [ 'index', 'show', 'edit', 'update', 'new', 'create', 'delete' ]
  }

OptionParser.new do |opts|
  opts.on("-a", "--actions action1,action2,actionN", String, "Actions to generate") do |l|
    options[:actions] = l.split(",")
  end
  opts.on('-n', '--name NAME', String, 'The controller\'s name') do |l|
    options[:name] = l
  end
end.parse!(ARGV)

if options[:name].nil?
  puts "usage: crails scaffold [name] [action1,action2,actionN]"
  options[:name] = 'scaffold'
end

controller_actions = []
views_actions      = []

options[:actions].each do |action|
  if action != 'update' and action != 'create' and action != 'delete'
    views_actions << action
  end
  controller_actions << action
end

model_cmd      = "crails generate/model #{options[:name]}"
controller_cmd = "crails generate/controller -n #{options[:name]} -a #{controller_actions.join ','}"
views_cmds     = Array.new
views_actions.each do |action|
  views_cmds << "crails generate/view #{options[:name]} #{action}"
end

#
# Generate the controller and the views
#
system controller_cmd
views_cmds.each do |cmd|
  system cmd
end

#
# Optionally generate a model ?
#
print "\033[35m[?]\033[0m Do you wish to generate a model ? [y/n] "
answer = STDIN.gets.chomp
action = if answer =~ /^y(es){0,1}$/
  system model_cmd
end

#
# Optionally generate a layout ?
#
layout_path = "app/views/layouts/#{options[:name]}.html.ecpp"
unless File.exists? layout_path
  print "\033[35m[?]\033[0m Do you wish to generate a layout ? [y/n] "
  answer = STDIN.gets.chomp
  action = if answer =~ /^y(es){0,1}$/
    system "crails generate/layout bootstrap #{options[:name]}"
  end
end