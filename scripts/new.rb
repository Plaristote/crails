#!/usr/bin/ruby

require "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib/project_model"
require "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib/cmakelists"

##
## Command Line Options management
##
require 'optparse'

options = {
            base_directory:      ARGV[0],
            sql:                 false,
            mongodb:             false,
            segvcatch:           false,
            server:              :async,
            session_store:       "cookie_store",
            session_store_class: "CookieStore"
          }
OptionParser.new do |opts|
  opts.banner = "usage: crails new [name] [options]"

  opts.on('',   '--use-sql',       'include sql module')        do options[:sql]       = true  end
  opts.on('',   '--use-mongodb',   'include mongodb module')    do options[:mongodb]   = true  end
  opts.on('',   '--use-segvcatch', 'use the segvcatch library') do options[:segvcatch] = true  end
  opts.on('',   '--synchronous',   'server wont use threads')   do options[:server]    = :sync end

  opts.on('',   '--session-store include,type', 'set a session store (ex: -session-store mongodb,MongoStore)') do |param|
    param = param.split ','
    options[:session_store]       = param[0]
    options[:session_store_class] = param[1]
    options[:mongodb] = true if param[0] == 'mongodb'
  end

  opts.on('',   '--directory directory', 'set app directory (defaults to the app name)') do |param|
    options[:base_directory] = param
  end
end.parse!

if ARGV[0].nil?
  puts "usage: crails new [app]"
  exit 1
end

bundler_gemspec = Gem::Specification::find_all_by_name('bundler')
unless bundler_gemspec.any?
  puts "/!\\ Bundler gem not found. Please install it with `gem install bundler`."
  exit 2
end
bundler_gem = bundler_gemspec.first
bundler_bin = "#{bundler_gem.bin_dir}/#{bundler_gem.executables.first}"

base_directory = options[:base_directory]

options[:name] = (base_directory.split '/').last
project        = ProjectModel.new
source         = ENV['CRAILS_SHARED_DIR'] + '/app_template/'

project.base_directory source, base_directory do
  project.generate_erb 'CMakeLists.txt', 'CMakeLists.txt.erb', options
  project.file 'Gemfile'
  project.file 'Guardfile'
  project.directory :build do end
  project.directory :app do
    project.file 'routes.cpp'
    project.file 'main.cpp'
    project.directory :assets do
      project.directory :stylesheets do end
      project.directory :javascripts do
        project.file 'application.js'
        project.file 'crails.js'
      end
    end
    project.directory :controllers do end
    project.directory :models do end
    project.directory :views do
      project.directory :layouts do
        project.file 'demo.html.ecpp'
      end
    end
  end
  project.directory :lib    do
    project.file 'exception.html.ecpp'
  end
  project.directory :config do
    project.file 'db.json'
    project.file 'logger.cpp'
    project.file 'request_pipe.cpp'
    project.file 'renderers.cpp'
    project.generate_erb 'salt.cpp',          'salt.cpp.erb',          options
    project.generate_erb 'session_store.cpp', 'session_store.cpp.erb', options
    project.file 'ssl.cpp'
  end
  project.directory :public do
    project.directory :assets do end
    project.file '404.html'
    project.file '500.html'
  end
  project.directory :spec do
    project.file 'spec.cpp'
  end
  project.directory :logs do end
end

FileUtils.cd base_directory do
  FileUtils.cd 'build' do
    puts "\033[32m[CMAKE]\033[0m " + "Configuring building tools"
    `cmake ..`
  end
  puts "\033[32m[BUNDLER]\033[0m " + "Installing dependencies"
  puts `#{bundler_bin} install`
end

puts ""
puts "  Successfully created a Crails App !"
puts "    -> Build by typing\t'cd #{options[:name]}/build && cmake .. && make'"
puts "    -> Launch by typing\t'crails server'"
