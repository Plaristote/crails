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

CMakeLists.add_crails_module 'mail'
MainCpp.add_include     "crails/mail_servers.hpp"
MainCpp.add_initializer "MailServers::singleton::Initialize();"
MainCpp.add_finalizer   "MailServers::singleton::Finalize();"
