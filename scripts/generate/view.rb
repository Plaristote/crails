#!/usr/bin/ruby

controller = ARGV[0]
action     = ARGV[1]

if controller.nil? or action.nil?
  puts "usage: crails generate/view [controller] [action]"
  exit 1
end

html = <<HTML
// END LINKING
<h2>#{controller}#{action}</h2>
HTML

directory = "app/views/#{controller}"

unless File.directory? directory
  require 'fileutils'
  FileUtils.mkdir directory
  puts "\033[32m[CREATE]\033[0m " + "Directory #{directory}"
end
path = "#{directory}/##{action}.html.ecpp"

File.open path, 'w' do |file|
  file.write html
end

puts "\033[32m[CREATE]\033[0m " + "File #{path} (bootstrap template)"