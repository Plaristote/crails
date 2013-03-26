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

model = Hash.new

class InputHandler
  def self.array desc, prompt, &block
    puts "#{desc} (type Ctrl+D to end, type cancel to remove last entry)"
    array = Array.new
    print prompt + ' '
    until (line = STDIN.gets).nil?
      line = line.chomp
      if line == 'cancel'
	array.remove array.last if array.size > 0
      else
	elem = (block.call line)
	array << elem unless elem.nil?
      end
      print prompt + ' '
    end
    puts ''
    array
  end

  def self.string desc, prompt, &block
    puts "#{desc}\n"
    print prompt + ' '
    line = STDIN.gets.chomp
    unless block.nil?
      block.call line
    else
      line
    end
  end
end

name             = ARGV[0]
header_define    = name.underscore.upcase
header_name      = name.underscore
model[:table]    = name.underscore
model[:database] = InputHandler.string "What's the key of the database to use ?", "[see conf/db.json]"
model[:type]     = InputHandler.string "Type of model ? ",  "[mongodb|sql]"
model[:fields]   = InputHandler.array  "Model fields ?", "[name, type, value]" do |line|
  array = line.split ','
  if array.size != 3
    puts "[!] You need to describe your fields like this: name, type, default value"
    nil
  else
    { name: array[0].chomp.strip, type: array[1].chomp.strip, default: array[2].chomp.strip }
  end
end

source = nil
if model[:type] == 'mongodb'
  
  initialize_fields = String.new
  decl_fields       = String.new
  model[:fields].each do |field|
    # Initialize
    initialize_fields += "    initialize_#{field[:name]}();\n"
    # Declaration
    default            = field[:default]
    default            = '"' + default + '"' if field[:type] == 'std::string'
    decl_fields       += "  MONGODB_FIELD(#{field[:type]}, #{field[:name]}, #{default})\n"
  end
  
  source = <<CPP
#ifndef  #{header_define}_HPP
# define #{header_define}_HPP

# include <crails/mongodb.hpp>
# include <crails/mongodb/model.hpp>
# include <crails/mongodb/resultset.hpp>

class #{name} : public MongoDB::Model
{
public:
  static const std::string DatabaseName(void)   { return ("#{model[:database]}"); }
  static const std::string CollectionName(void) { return ("#{model[:table]}");    }

  #{name}(MongoDB::Collection& collection, mongo::BSONObj bson_object) : Model(collection, bson_object)
  {
  }

#{decl_fields}

  void InitializeFields(void)
  {
#{initialize_fields}
  }

private:
};

#endif
CPP

elsif model[:type] == 'sql'
    
  initialize_fields = String.new
  decl_fields       = String.new
  decl_table        = String.new
  model[:fields].each do |field|
    # Initialize
    initialize_fields += "    initialize_#{field[:name]}();\n"
    # Declaration
    default            = field[:default]
    default            = '"' + default + '"' if field[:type] == 'std::string'
    decl_fields       += "  SQL_FIELD(#{field[:type]}, #{field[:name]}, #{default})\n"
    # Table Declaration
    
    sql_type    = nil
    
    type_mapper = { 'char' => 'TINYINT', 'short' => 'SMALLINT', 'int' => 'INT', 'float' => 'FLOAT',
                    'std::string' => 'VARCHAR(255)' }
    type_mapper.each do |key, value|
      reg = /#{key}/i
      if reg.match field[:type]
	sql_type = value
	break
      end
    end
    
    if sql_type.nil?
      puts "[WARNING] Couldn't find a proper sql type for #{field[:type]}. You'll need to set it yourself."
      sql_type 'unknown'
    end

    decl_table        += "    TABLE_FIELD(#{field[:name]}, \"#{sql_type}\")\n"
  end
  
  source = <<CPP
#ifndef  #{header_define}_HPP
# define #{header_define}_HPP

# include <crails/sql.hpp>
# include <crails/sql/model.hpp>
# include <crails/sql/resultset.hpp>

class #{name} : public SQL::Model
{
public:
  TABLE(#{model[:database]}, #{model[:table]})
#{decl_table}
  END_TABLE

  #{name}(SQL::Table& table) : Model(table)
  {
    InitializeFields();
  }

  #{name}(SQL::Table& table, soci::row& row) : Model(table, row)
  {
    InitializeFields();
  }

#{decl_fields}

  void InitializeFields(void)
  {
#{initialize_fields}
  }

private:
};

#endif
CPP

end

File.open "app/models/#{header_name}.hpp", 'w' do |file|
  file.write source
  puts "\033[32m[CREATE]\033[0m " + "File app/models/#{header_name}.hpp"
end
