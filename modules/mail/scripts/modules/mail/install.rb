#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/'

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'mailers.json'
  end
end
