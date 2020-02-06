#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

class CrailsDockerBase
  def crails_compile_files ; [] ; end
  def compile_files ; [] ; end
end

require 'dockerfile'
require 'project_model'
require "docker_#{ARGV[0]}_plugin"

dockerfile = Dockerfile.new
docker_plugin = CrailsDocker.new
project = ProjectModel.new 
source = "#{ENV['CRAILS_SHARED_DIR']}/app_template/docker"

project.base_directory source, Dir.pwd do
  project.directory :docker do
    project.directory :base do
      (docker_plugin.compile_files + docker_plugin.crails_compile_files).each do |file|
        project.file file
      end
    end
  end
end

docker_plugin.crails_compile_files.each do |file|
  dockerfile.add_crails_compile_file  file
end

docker_plugin.compile_files.each do |file|
  dockerfile.add_compile_file file
end

dockerfile.write
