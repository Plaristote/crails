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
    project.file 'db.json'
  end

  project.directory :tasks do
    project.directory :sql_migrate do
      project.file 'CMakeLists.txt'
      project.file 'main.cpp'
    end
  end
end

CMakeLists.add_dependency 'soci_core'
backends.each do |backend|
  CMakeLists.add_dependency "soci_#{backend}"
end
CMakeLists.add_crails_module 'sql'
CMakeLists.add_crails_task 'sql_migrate'

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
  guard 'crails-sql-models' do
    watch(%r{app/models/.+\.h(pp)?$})
  end
RUBY
guardfile.write