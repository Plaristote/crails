#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/mongodb'

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'session_store.cpp'
  end
end

CMakeLists.add_crails_module 'mongodb-session'

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
  guard 'crails-mongodb-models' do
    watch(%r{app/models/.+\.h(pp)?$})
  end
RUBY
guardfile.write