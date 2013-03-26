#!/usr/bin/ruby

if not ((File.exists? 'CMakeLists.txt') and (File.directory? 'app'))
  puts 'You must run this at the root of your crails project'
  exit 1
end

class String
  def underscore
    self.gsub(/::/, '/').
    gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2').
    gsub(/([a-z\d])([A-Z])/,'\1_\2').
    tr("-", "_").
    downcase
  end

  def camelize
    return self if self !~ /_/ && self =~ /[A-Z]+.*/
    split('_').map{|e| e.capitalize}.join
  end
end

require 'optparse'

options = {
    actions: [ 'index', 'show', 'edit', 'update', '_new', 'create', '_delete' ]
  }

OptionParser.new do |opts|
  opts.on("-a", "--actions action1,action2,actionN", String, "Actions to generate") do |l|
    options[:actions] = l.split(",")
  end
  opts.on('-n', '--name NAME', String, 'The controller\'s name') do |l|
    options[:name] = l
  end
end.parse!(ARGV)

if options[:name].nil?
  puts "usage: crails scaffold [name] [action1,action2,actionN]"
  options[:name] = 'scaffold'
end


name        = options[:name]
header_name = name.underscore
class_name  = name.camelize

function_headers = String.new
options[:actions].each do |action|
  function_headers += "  static DynStruct " + action.underscore + "(Params& params);\n"
end

header = <<CPP
#ifndef  #{header_name.upcase}_HPP
# define #{header_name.upcase}_HPP

# include <crails/appcontroller.hpp>

class #{name}Controller : public ControllerBase
{
public:
#{function_headers}
};

#endif
CPP

functions = String.new

options[:actions].each do |action|
  functions += <<CPP
DynStruct #{name}::#{action.underscore}(Params& params)
{
  DynStruct render_data;
  
  return (render_data);
}

CPP
end

source = <<CPP
#include "app/controllers/#{header_name}.hpp"

using namespace std;

#{functions}
CPP

File.open "app/controllers/#{header_name}.hpp", 'w' do |file|
  file.write header
  puts "\033[32m[CREATE]\033[0m " + "File app/controllers/#{header_name}.hpp"
end
File.open "app/controllers/#{header_name}.cpp", 'w' do |file|
  file.write source
  puts "\033[32m[CREATE]\033[0m " + "File app/controllers/#{header_name}.cpp"  
end
