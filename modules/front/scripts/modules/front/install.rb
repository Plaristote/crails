#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'

project        = ProjectModel.new
base_directory = Dir.pwd

##
## Guard templates
##
source         = "#{ENV['CRAILS_SHARED_DIR']}/guard/templates"
project_name   = `cat CMakeLists.txt | grep -e "^project\(.*\)$"`.chomp.gsub(/project\(/, '').gsub(/\)$/, '')

project.base_directory source, base_directory do
  project.directory :lib do
    project.directory :renderers do
      templates = []
      project.generate_erb 'archive.cpp', 'archive_renderer.cpp.erb', :binding => binding
    end
  end
end

##
## Application templates
##
source         = "#{ENV['CRAILS_SHARED_DIR']}/app_template/front"
project_name   = `cat CMakeLists.txt | grep -e "^project\(.*\)$"`.chomp.gsub(/project\(/, '').gsub(/\)$/, '')

project.base_directory source, base_directory do
  project.directory :front do
    project.generate_erb 'CMakeLists.txt', 'CMakeLists.txt.erb', :binding => binding
    project.file 'main.cpp'
    project.file 'router.hpp'
    project.file 'router.cpp'

    project.directory :controllers do
      project.file 'application_controller.hpp'
    end

    project.directory :layouts do
      project.file 'application_layout.hpp'
    end

    project.directory :views do
    end
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'front'

renderers_cpp = RenderersCppEditor.new
renderers_cpp.add_include 'crails/renderers/archive_renderer.hpp'
renderers_cpp.add_initializer 'renderers.push_back(new ArchiveRenderer);'

use_html = nil
while use_cheerp_html.nil?
  puts "/?\\ Do you want to use the crails-cheerp-html generator ? (yes/no)"
  print "$> "
  STDOUT.flush
  answer = gets.chomp
  if answer.start_with?('y')
    use_html = true
  elsif answer.start_with?('n')
    use_html = false
  end
end

guardfile_html_src += <<RUBY
guard 'crails-cheerp-html' do
  watch(%r{front/.+\.html$})
end
RUBY
guardfile_src += "\nguard 'crails-cheerp'\n"

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', guardfile_html_src if use_html
guardfile.add_task 'before_compile', "\nguard 'crails-archive'\n"
guardfile.add_task 'compile', guardfile_src

if use_cheerp_html
  gemfile = GemfileEditor.new
  gemfile.add_ruby_dependency 'nokogiri', '~> 1.10.8'
  gemfile.write
end

cmake.write
renderers_cpp.write
guardfile.write
