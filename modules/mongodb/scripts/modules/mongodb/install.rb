#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/mongodb'

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'db.json'
  end
end

CMakeLists.add_crails_module 'mongodb'
