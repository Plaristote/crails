#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/sql'
backends       = ['mysql', 'postgresql', 'sqlite3']

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'images.cpp'
  end
end

CMakeLists.add_dependency 'Magick++'
CMakeLists.add_crails_module 'image'
