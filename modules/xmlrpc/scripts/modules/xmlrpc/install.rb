#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'

project        = ProjectModel.new
base_directory = Dir.pwd

cmake = CMakeLists.new
cmake.add_crails_module 'xmlrpc'
cmake.write
