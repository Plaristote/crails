#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

class BaseDockerMachine
  attr_accessor :distribution_dependencies
  attr_accessor :compile_dependencies

  def constructor
    distribution_dependencies = distribution_crails_dependencies
    compile_dependencies = {}
    compile_dependencies_tiers.each do |tier|
      compile_dependencies[tier] = []
    end
  end

  def compile_dependencies_tiers
    [:first, :second, :third]
  end

  def add_compile_dependency name, version, data
    if version == 'system'
      distribution_dependencies += data["versions"]["system"][distribution_name]
    else

      # Register system dependencies
      unless data["system-dependencies"].nil?
        unless data["system-dependencies"][distribution_name].nil?
          distribution_dependencies += data["system-dependencies"][distribution_name]
        else
          throw "/!\\ Dependency #{name} has system requirements that cannot be solved for `#{distribution_name}`"
        end
      end

      # Register compile file
      compile_dependencies[data["tier"]] << [name, version]
    end
  end

  def get_binding
    binding
  end
end

class DebianDockerMachine < BaseDockerMachine
  def distribution_name
    "debian"
  end

  def docker_image
    "debian:jessie"
  end

  def command_install_package
    "apt-get -y update && apt-get -y install"
  end

  def distribution_crails_dependencies
    [
      "cmake",
      "build-essential",
      "libbz2-dev",
      "libssl-dev",
      "git",
      "ruby",
      "ruby-dev"
      "nodejs",
    ]
  end
end

docker_machine = DebianDockerMachine.new
crails_hard_dependencies = [:boost, :cppnetlib]
compile_files = []

require 'pathname'
require 'json'

def prompt_use_dependency name
  puts "- Building optional dependency: #{name} (y/n):"
  gets.chomp == 'y'
end

def prompt_version_dependency name, data
  can_use_system_version = not data["versions"]["system"][docker_machine.distribution_name].nil?
  can_use_custom_version = not data["versions"]["custom"].nil?
  if can_use_custom_version
    print "- Which version of #{name} will you use ? "
    print "(type `system` to use the distribution package)" if can_use_system_version
    print "\n"
   gets.chomp
  elsif can_use_system_version
    "system"
  end
end

Dir["#{ENV["CRAILS_SHARED_DIR"]}/docker-dependencies/*.json"].each do |depfile|
  name = Pathname.new(depfile).basename
  name = name.to_s[0...-name.extname.to_s.length].to_sym
  data = JSON.parse File.read(depfile.to_s)

  use_dependency         = crails_hard_dependencies.include?(name) || prompt_use_dependency(name)
  version                = nil

  if use_dependency
    version = prompt_version_dependency data unless data["versions"].nil?
    docker_machine.add_compile_dependency name, version, data
    if version != "system"
      compile_file = "#{depfile.parent.to_s}/#{name}.sh"
      compile_files << compile_file
    end
  end
end

project.base_directory source, Dir.pwd do
  project.directory :docker do
    project.directory :base do
      project.generate_erb 'Dockerfile', 'Dockerfile.erb', binding: docker_machine.binding
    end
  end
end
