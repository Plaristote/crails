#!/usr/bin/ruby

command = ARGV[0]

if command.nil?
  puts "Usage: `crails task [command] [task_name]"
  exit -1
end

if command == 'add'
  task_name = ARGV[1]

  $: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"
  require 'project_model'
  require 'cmakelists'

  CMakeLists.add_crails_task task_name

  task = ProjectModel.new
  source = ENV['CRAILS_SHARED_DIR'] + '/app_template/task'

  task.base_directory source, Dir.pwd do
    task.directory :tasks do
      task.directory task_name do
        task.file 'CMakeLists.txt'
        task.file 'main.cpp'
      end
    end
  end
elsif command == 'run'
  system "build/tasks/#{task_name}/task"
else
  puts "command #{command} not found"
end