#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'
require 'cmakelists'
require 'maincpp'
require 'bundle'

project           = ProjectModel.new
base_directory    = Dir.pwd
comet_app_path    = "app/comet"
comet_vendor_path = "lib/comet"

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
  project.directory :config do
    project.file 'comet.json'
  end

  project.directory :app do
    project.directory :comet do
      project.generate_erb 'CMakeLists.txt', 'CMakeLists.txt.erb', :binding => binding
      project.file 'main.cpp'
      project.file 'routes.cpp'
      project.file 'application.hpp'

      project.directory :controllers do
      end

      project.directory :models do
      end

      project.directory :views do
        project.directory :layouts do
        end
      end
    end
  end

  project.directory :lib do
    project.directory :comet do
      project.directory :crails do
        project.file 'archive.cpp'
        project.file 'string_semantics.cpp'
        project.file 'string_split.cpp'
        project.file 'raise.cpp'
        project.file 'model.cpp'
      end
    end
  end
end

cmake = CMakeLists.new
cmake.add_crails_module 'front'

renderers_cpp = RenderersCppEditor.new
renderers_cpp.add_include 'crails/renderers/archive_renderer.hpp'
renderers_cpp.add_initializer 'renderers.push_back(new ArchiveRenderer);'

comet_html_guard = <<RUBY

  guard 'comet-html', source: '#{comet_app_path}', output: '#{comet_vendor_path}/html', config: 'config/comet.json' do
    watch(%r{app/comet/.+\\.html$})
  end
RUBY

comet_guard = <<RUBY

  guard 'comet', cmakelists: '#{comet_app_path}', output: 'public/assets'
RUBY

guardfile = GuardfileEditor.new
guardfile.add_task 'before_compile', comet_html_guard
guardfile.add_task 'before_compile', "\n  guard 'crails-archive'\n"
guardfile.add_task 'compile', comet_guard

gemfile = GemfileEditor.new
gemfile.add_ruby_dependency 'comet-cpp', git: "https://github.com/crails-framework/comet.cpp.git"
gemfile.write

cmake.write
renderers_cpp.write
guardfile.write

comet_install_cmd = "comet-new --skip-project-files --project-path=#{comet_app_path} --vendor-path=#{comet_vendor_path}"

if bundle_install == 0
  puts "+ comet-new --skip-project-files --project-path=#{comet_app_path} --vendor-path=#{comet_vendor_path}"
  run_using_bundler comet_install_cmd
else
  puts "/!\\ Failed to install comet: `bundle install` command failed"
  puts "The following command could not be run:"
  puts "  #{comet_install_cmd}"
end
