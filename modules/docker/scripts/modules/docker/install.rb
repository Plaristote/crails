#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

# Dummy Guard::Plugin implementation to include Guard::CrailsPlugin
module ::Guard
  class Plugin
  end
end

require 'project_model'
require "#{ENV['CRAILS_SHARED_DIR']}/guard/crails-base"

project_name   = Guard::CrailsPlugin.new.get_cmake_variable 'CMAKE_PROJECT_NAME:STATIC'
project        = ProjectModel.new
base_directory = "#{Dir.pwd}/docker"
source         = "#{ENV['CRAILS_SHARED_DIR']}/app_template/docker"

project.base_directory source, Dir.pwd do
  project.directory :docker do
    project.file '.gitignore', 'gitignore'
    project.generate_erb 'build', 'build.erb', binding: binding
    project.generate_erb 'shell', 'shell.erb', binding: binding
    project.directory :base do
      project.file 'Dockerfile'
      project.file 'crails.sh'
      project.file 'cppnetlib.sh'
      project.file 'boost.sh'
    end
    project.directory :scripts do
      project.file 'package.sh'
    end
  end
end

FileUtils.chmod 'ug+x', [ 'docker/build', 'docker/shell' ]

