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
      project.generate_erb 'html.cpp', 'html_renderer.cpp.erb', :binding => binding
    end
  end
end

source         = "#{ENV['CRAILS_SHARED_DIR']}/app_template/html"
project.base_directory source, base_directory do
  project.directory :lib do
    project.file 'exception.ecpp'
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'html'

renderers_cpp = RenderersCppEditor.new
renderers_cpp.add_include 'crails/renderers/html_renderer.hpp'
renderers_cpp.add_initializer 'renderers.push_back(new HtmlRenderer);'

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', <<RUBY
guard 'crails-ecpp' do
    watch(%r{.+\.ecpp$})
  end
RUBY

cmake.write
renderers_cpp.write
guardfile.write
