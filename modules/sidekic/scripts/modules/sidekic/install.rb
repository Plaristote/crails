#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project         = ProjectModel.new
base_directory  = Dir.pwd
source          = ENV['CRAILS_SHARED_DIR'] + '/app_template/sidekic'

project.base_directory source, base_directory do
  project.directory :config do
    project.file "sidekic.cpp"
  end

  project.directory :tasks do
    project.directory :sidekic do
      project.directory :sidetasks do end
      project.file "CMakeLists.txt"
      project.file "main.cpp"
      project.file "sidetasks.cpp"
      project.file "ctpl.h"
    end
  end
end

cmake = CMakeLists.new
cmake.add_crails_module "sidekic"
cmake.add_crails_task   "sidekic"
cmake.write
