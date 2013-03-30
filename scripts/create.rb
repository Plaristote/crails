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
    tpl  = ERB.new (File.new path).read
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
            sql:       false,
            mongodb:   false,
            segvcatch: false,
            server:    :async,
            debug:     false
          }
OptionParser.new do |opts|
  opts.banner = "usage: crails new [name] [options]"

  opts.on('',   '--use-sql',       'include sql module')        do options[:sql]       = true end
  opts.on('',   '--use-mongodb',   'include mongodb module')    do options[:mongodb]   = true end
  opts.on('',   '--use-segvcatch', 'use the segvcatch library') do options[:segvcatch] = true end
  opts.on('-d', '--debug-mode',    'use debug mode')            do options[:debug]     = true end
end.parse!

if ARGV[0].nil?
  puts "usage: crails new [app]"
  exit 1
end

base_directory = ARGV[0]

options[:name] = (base_directory.split '/').last
project        = ProjectModel.new
source         = "/home/plaristote/projects/MongoDB_Test"

source = unless ENV['CRAILS_SHARED_DIR'].nil?
  ENV['CRAILS_SHARED_DIR']
else
  path = nil
  (ENV['XDG_DATA_DIRS'].split ':').each do | dir |
    full_path = "#{dir}/crails"
    path = full_path ; break if File.directory? full_path
  end
  path
end

project.base_directory source, base_directory do
  project.generate_erb 'CMakeLists.txt', 'CMakeLists.txt.erb', options
  project.directory :build do end
  project.directory :app do
    project.file 'routes.cpp'
    project.file 'main.cpp'
    project.directory :assets do
      project.directory :stylesheets do end
      project.directory :javascript  do end
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
    project.directory :db   do
      project.file 'mongodb.cpp' if options[:sql]     != false
      project.file 'sql.cpp'     if options[:mongodb] != false
    end
    project.file 'exception.html.ecpp'
  end
  project.directory :config do
    project.file 'db.json'
  end
  project.directory :public do
    project.file '404.html'
    project.file '500.html'
    project.directory :js          do
      project.file 'crails.js'
    end
    project.directory :stylesheets do end
  end
end

puts ""
puts "  Successfully created a Crails App !"
puts "    -> Build by typing\t'cd #{options[:name]}/build && cmake .. && make'"
puts "    -> Launch by typing\t'./crails-app'"
