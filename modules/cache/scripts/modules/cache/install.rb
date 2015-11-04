#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/cache'

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'memcached.cpp'
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'cache'
cmake.write
