#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/mail'

project.base_directory source, base_directory do
  project.directory :config do
    project.file 'mailers.json'
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'mail'

main_cpp = MainCppEditor.new
main_cpp.add_include     "crails/mail_servers.hpp"
main_cpp.add_initializer "MailServers::singleton::Initialize();"
main_cpp.add_finalizer   "MailServers::singleton::Finalize();"

cmake.write
main_cpp.write
