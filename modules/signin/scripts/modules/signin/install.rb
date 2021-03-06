#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = "#{ENV['CRAILS_SHARED_DIR']}/app_template/signin"

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'signin.cpp'
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'signin'
cmake.write
