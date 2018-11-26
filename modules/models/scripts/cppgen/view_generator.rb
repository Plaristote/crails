$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/model'
require 'cppgen/generator_base'

class ViewGenerator < GeneratorBase
  def self.is_file_based? ; false ; end

  def reset
    @src = ""
    @declarations = []
    super
  end

  def should_generate_for object
    (not object[:header].nil?) && (not object[:classname].nil?)
  end

  def generate_for object
    reset
    @declarations << "#include \"#{object[:header]}\"\n"
    @declarations << "#{object[:classname]}& @model;"
    property "ODB::id_type", "id"
    self.instance_eval &object[:block]
    @src
  end

  def get_headers
    @declarations.join "\n"
  end

  def property type, name, options = {}
    return if (not options[:client].nil?) && options[:client][:ignore] == true
    if type == "DataTree" || type == "LocaleString"
      _append "json(#{name.inspect}, model.get_#{name}().as_data());"
    else
      _append "json(#{name.inspect}, model.get_#{name}());"
    end
  end

  def has_one type, name, options = {}
    return if (not options[:client].nil?) && options[:client][:ignore] == true
    if options[:db].nil? || options[:db][:joined] == false
      _append "if (model.get_#{name}())"
      _append "  json(\"#{name}_id\", model.get_#{name}()->get_id());"
    else
      _append "json(\"#{name}_id\", model.get_#{name}_id());"
    end
  end

  def has_many type, name, options = {}
    return if (not options[:client].nil?) && options[:client][:ignore] == true
    _append "{"
    @indent += 1
    id_attr = (get_singular_name name) + "_ids"
    id_collector = if options[:db].nil? || options[:db][:joined] == false
                     "collect_ids_from(model.get_#{name}())"
                   else
                     "model.get_#{id_attr}()"
                   end
    _append "const auto relation_ids = #{id_collector};"
    _append "json_array(#{id_attr.inspect}, relation_ids.begin(), relation_ids.end());"
    @indent -= 1
    _append "}"
  end

  class << self
    def extension ; ".cjson" ; end

    def make_file filename, data
      source = ""
      source += "#include \"app/models/helpers.hpp\"\n"
      #source += ((Includes.list[filename] || []).collect {|a| "#include \"#{a}\""}).join "\n"
      source += (collect_includes_for filename).join("\n")
      source += "\n" + (data[:headers].join "\n")
      source += "\n// END LINKING\n"
      source += "\n" + (data[:bodies].join "\n")
      source
    end
  end
end
