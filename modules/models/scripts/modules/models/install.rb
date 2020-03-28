#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'
require 'bundle'

base_modules     = ["crails/edit", "crails/data", "crails/destroy", "crails/query"]
optional_modules = ["crails/view", "comet-cpp"]

puts "/?\\ Which optionals generators do you wish to use ? (separate with commas)"
puts "- Available modules are: #{optional_modules.join ', '}"
print "$> "
STDOUT.flush
picked_modules = base_modules + (gets.chomp.split /\s*,\s*/)

# Setup comet modules if needed
if picked_modules.include? "comet-cpp"
  picked_modules.delete "comet-cpp"
  ["edit", "data"].each do |mod|
    picked_modules.delete "crails/#{mod}"
    picked_modules << "comet/#{mod}"
  end
end

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
guard 'metarecord',
        input: ['app/data'],
        output: 'lib',
        generators: [#{(picked_modules.collect{|v| "'#{v}'"}).join ','}] do
    watch(%r{app/data/.+\.rb$})
  end
RUBY

project        = ProjectModel.new
base_directory = Dir.pwd

gemfile = GemfileEditor.new
gemfile.add_ruby_dependency 'meta-record', git: "https://github.com/crails-framework/meta-record.git"

project.base_directory nil, base_directory do
  project.directory :app do
    project.directory :data do
    end
  end
end

gemfile.write
guardfile.write
bundle_install
