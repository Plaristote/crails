#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project        = ProjectModel.new
base_directory = Dir.pwd
source         = "#{ENV['CRAILS_SHARED_DIR']}/guard/templates"

project.base_directory source, base_directory do
  project.directory :lib do
    project.directory :renderers do
      templates = []
      project.generate_erb 'json.cpp', 'json_renderer.cpp.erb', :binding => binding
    end
  end
end

source         = "#{ENV['CRAILS_SHARED_DIR']}/app_template/json"
project.base_directory source, base_directory do
  project.directory :lib do
    project.file 'exception.cjson'
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'json'

renderers_cpp = RenderersCppEditor.new
renderers_cpp.add_include 'crails/renderers/json_renderer.hpp'
renderers_cpp.add_initializer 'renderers.push_back(new JsonRenderer);'

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
guard 'crails-cjson' do
    watch(%r{app/views/.+\.cjson$})
  end
RUBY

cmake.write
renderers_cpp.write
guardfile.write
