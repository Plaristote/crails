#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/image'
backends       = ['mysql', 'postgresql', 'sqlite3']

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'images.cpp'
  end
end

cmake = CMakeLists.new
cmake.add_find_package        'ImageMagick COMPONENTS Magick++ REQUIRED'
cmake.add_include_directories '${ImageMagick_Magick++_INCLUDE_DIRS}'
cmake.add_dependency          '${ImageMagick_Magick++_LIBRARY}'
cmake.add_crails_module       'image'
cmake.write
