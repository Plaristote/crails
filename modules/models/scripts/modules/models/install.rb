#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

base_modules     = [:edit, :data, :destroy, :query]
optional_modules = [:view]

puts "/?\\ Which optionals generators do you wish to use ? (separate with commas)"
puts "- Available modules are: #{optional_modules.join ','}"
print "$> "
STDOUT.flush
picked_modules = base_modules + (gets.chomp.split /\s*,\s*/)

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
guard 'crails-models',
        input: ['app/data'],
        output: 'lib',
        generators: [#{(picked_modules.collect{|v| ":#{v}"}).join ','}] do
    watch(%r{app/data/.+\.rb$})
  end
RUBY

project        = ProjectModel.new
base_directory = Dir.pwd

project.base_directory nil, base_directory do
  project.directory :app do
    project.directory :data do
    end
  end
end

guardfile.write
