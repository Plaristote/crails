#!/usr/bin/ruby
require 'erb'
require 'pathname'
require 'fileutils'

def compile_model filepath
  file_content            = (File.open filepath).read
  filename                = filepath.match(/\/app\/models\/(.*).h(pp)?$/)[1]
  relative_path           = filename + '.cpp'
  classname               = nil
  fields                  = []
  has_many                = []
  belongs_to              = []
  has_and_belongs_to_many = []

  # SCAN CLASS
  file_content.scan /MONGODB_MODEL\([a-zA-Z0-9_]+\)/ do |param|
    throw "#{filename} hosts several MONGODB_MODEL. Don't do that." if not classname.nil?
    classname  = param[14...param.length-1]
  end

  # SCAN FIELDS
  file_content.scan /MONGODB_FIELD\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+),(.*)\)/ do |param1, param2,param3|
    field_data = {
        type:    param1,
        name:    param2,
        default: param3
      }
    fields << field_data
  end
  
  # SCAN RELATIONS
  # Scan has_many
  file_content.scan /MONGODB_HAS_MANY\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+)\)/ do |param1,param2|
    has_many_data = {
        type:          param1,
        remote_field:  param2,
        relation_name: param1
      }
   has_many << has_many_data
  end

  file_content.scan /MONGODB_HAS_MANY_AS\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+),\s*([a-zA-Z0-9_]+)\)/ do |param1,param2,param3|
    has_many_data = {
        type:          param1,
        remote_field:  param2,
        relation_name: param3
      }
    has_many << has_many_data
  end
  
  # Scan belongs_to
  file_content.scan /MONGODB_BELONGS_TO\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+)\)/ do |param1|
    throw "bite"
    belongs_to_data = {
        type:          param1,
        relation_name: param1
      }
    belongs_to << belongs_to_data
  end

  file_content.scan /MONGODB_BELONGS_TO_AS\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+)\)/ do |param1,param2|
    belongs_to_data = {
        type:          param1,
        relation_name: param2
      }
    belongs_to << belongs_to_data
  end
  
  file_content.scan /MONGODB_HAS_AND_BELONGS_TO_MANY\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+)\)/ do |param1,param2|
    has_many_data = {
        type:          param1,
        relation_name: param2
      }
    has_and_belongs_to_many << has_many_data
  end

  belongs_to.each do |relation|
    relation_field = {
        type:    "mongo::OID",
        name:    "#{relation[:relation_name]}_id",
        default: "mongo::OID()"
      }
    fields << relation_field
  end
  
  has_and_belongs_to_many.each do |relation|
    relation_field = {
      type:    "MongoDB::Array<mongo::OID>",
      name:    "#{relation[:relation_name]}_ids",
      default: "MongoDB::Array<mongo::OID>()"
      }
    fields << relation_field
  end

  template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/mongodb_model.cpp.erb").read
  path     = Pathname.new "lib/models/#{relative_path}"

  FileUtils.mkdir_p path.dirname
  File.open path.to_s, 'w' do |file|
    file.write template.result binding
  end
end

if ARGV[0].nil?
else
  compile_model ARGV[0]
end
