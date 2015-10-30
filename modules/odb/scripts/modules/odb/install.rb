#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'

project         = ProjectModel.new
base_directory  = Dir.pwd
source          = ENV['CRAILS_SHARED_DIR'] + '/app_template/odb'
backends        = ['mysql', 'postgresql', 'sqlite', 'oracle']
picked_backends = []

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'db.json'
  end
end

do
  puts "/?\\ What backends do you wish to use ? (ex: mysql, sqlite)"
  puts "Available backends are #{backends.join ','}."
  print "$> "
  STDOUT.flush
  picked_backends = gets.chomp.split /\s*,\s*/
until (picked_backends - backends).empty? && picked_backends.count > 0

picked_backends.each do |backend|
  CMakeLists.add_dependency "odb-#{backend}"
end
CMakeLists.add_dependency 'odb'
CMakeLists.add_crails_module 'odb'
