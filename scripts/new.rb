#!/usr/bin/ruby

##
## ProjectModel
##
require 'fileutils'
require 'erb'

class ProjectModel
  include FileUtils
  
  def base_directory template_path, path, &block
    @template_path = template_path
    directory path do block.call end
  end

  def directory path, &block
    makedir path
    cd path.to_s, verbose: false do
      block.call
    end
  end

  def file name, source = nil
    action = "CREATE"
    if File.exists? name
      print "\033[35m[?]\033[0m File #{name} exists. Overwrite ? [y/n] "
      answer = STDIN.gets.chomp
      action = if answer =~ /^y(es){0,1}$/
        "REPLACE"
      else
        "EXISTS"
      end
    end
    if action != "EXISTS"
      source = name if source.nil?
      cp "#{@template_path}/#{source}", name.to_s
    end
    puts "\033[32m[#{action}]\033[0m " + "File #{name}"
  end
  
  def generate_erb target, source, options
    path = "#{@template_path}/#{source}"
    tpl  = ERB.new (File.new path).read, nil, '-'
    File.open target, 'w' do | f |
      f.write (tpl.result binding)
    end
    puts "\033[32m[GENERATED]\033[0m " + "File #{target}"
  end

private
  def makedir path
    begin
      mkdir path.to_s
      puts "\033[32m[CREATE]\033[0m " + "Directory #{path}"
    rescue Errno::EEXIST
      puts "\033[37m[EXISTS]\033[0m " + "Directory #{path}"
    end
  end
end

##
## Command Line Options management
##
require 'optparse'

options = {
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

base_directory = ARGV[0]

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
    project.file 'mailers.json'
    project.file 'request_pipe.cpp'
    project.file 'renderers.cpp'
    project.generate_erb 'salt.cpp',          'salt.cpp.erb',          options
    project.generate_erb 'session_store.cpp', 'session_store.cpp.erb', options
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
puts "    -> Launch by typing\t'./crails-app'"
