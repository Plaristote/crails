#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project         = ProjectModel.new
base_directory  = Dir.pwd
source          = ENV['CRAILS_SHARED_DIR'] + '/app_template/odb'
backends        = ['mysql', 'pgsql', 'sqlite', 'oracle']
picked_backends = []

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'odb.cpp'
    project.file 'odb.hpp'
  end

  project.directory :tasks do
    project.directory :odb_migrate do
      project.file 'CMakeLists.txt'
      project.file 'main.cpp'
    end
  end

  project.directory :lib do
    project.directory :odb do
    end
  end
end

until (picked_backends - backends).empty? && picked_backends.count > 0
  puts "/?\\ What backends do you wish to use ? (separate with commas)"
  puts "Available backends are #{backends.join ','}."
  print "$> "
  STDOUT.flush
  picked_backends = gets.chomp.split /\s*,\s*/
end

cmake = CMakeLists.new
cmake.add_dependency 'odb'
picked_backends.each do |backend|
  cmake.add_option("ODB_WITH_#{backend.upcase} \"enables #{backend} support for crails-odb\" ON")
  cmake.add_code <<CMAKE
if (ODB_WITH_#{backend.upcase})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DODB_WITH_#{backend.upcase}")
  set(dependencies "${dependencies} odb-#{backend}")
endif()
CMAKE
end
cmake.add_crails_module 'odb'
cmake.add_crails_task 'odb_migrate'

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
guard 'crails-odb', backends: [#{(picked_backends.map {|b| ":#{b}"}).join ','}] do
    watch(%r{app/models/.+\.h(pp|xx)?$})
  end
RUBY

cmake.write
guardfile.write
