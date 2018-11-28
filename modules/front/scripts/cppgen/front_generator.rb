$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/model'
require 'cppgen/generator_base'

class FrontGenerator < GeneratorBase
  class << self
    def extension ; ".hpp" ; end
    def is_file_based? ; false ; end

    def make_file filename, data
      data[:bodies].join "\n"
    end
  end

  def reset
    super
    @state = nil
    @relations = {}
    @fields = []
    @id_type = "unsigned long"
  end

  def generate_for object
    reset
    @class_name = object[:name] + "Data"
    defname = "#{object[:name].upcase}_DATA_HPP"
    _append "#ifndef  #{defname}"
    _append "# define #{defname}"
    _append ""
    _append "# include <crails/front/mvc/model.hpp>"
    _append "# include <crails/datatree.hpp>"
    _append ""
    generate_class object
    _append ""
    _append "#endif"
    @src
  end

  def generate_class object
    _append "struct #{@class_name} : public Crails::Front::Model"
    _append "{"
    _append "public:"
    @indent += 1
    @state = :public_scope
    _append "#{@id_type} get_id() const { return id; }"
    self.instance_eval &object[:block]
    @src += "\n"
    generate_serializers object
    generate_jsonifiers object
    @indent -= 1
    _append "protected:"
    @indent += 1
    @state = :field_scope
    self.instance_eval &object[:block]
    @indent -= 1
    _append "  #{@id_type} id;"
    _append "};"
  end

  def generate_serializers object
    indent = @indent * 2 + 2
    @state = :serialize
    _append "void serialize(OArchive& archive)"
    _append "{"
    indent.times do @src += " " end
    @src += "archive & id"
    self.instance_eval &object[:block]
    @src += ";\n"
    _append "}\n"
    _append "void serialize(IArchive& archive)"
    _append "{"
    indent.times do @src += " " end
    @src += "archive & id"
    self.instance_eval &object[:block]
    @src += ";\n"
    _append "}\n"
  end

  def generate_jsonifiers object
    @state = :to_json
    _append "std::string to_json() const"
    _append "{"
    @indent += 1
    _append "DataTree data;\n"
    self.instance_eval &object[:block]
    _append "return data.to_json();"
    @indent -= 1
    _append "}\n"
    @state = :from_json
    _append "void from_json(Data data)"
    _append "{"
    self.instance_eval &object[:block]
    _append "}\n"
  end

  def property type, name, options = {}
    return if not options[:client].nil? and options[:client][:ignore] == true
    if @state == :field_scope
      definition = "#{type} #{name}"
      definition += "= #{options[:default]}" unless options[:default].nil?
      _append "#{definition};"
    elsif @state == :public_scope
      _append "#{type} get_#{name}() const { return #{name}; }"
      if options[:read_only] != true
        _append "void set_#{name}(#{type} _value) { #{name} = _value; }"
      end
    elsif @state == :serialize
      @src += " & #{name}"
    elsif @state == :to_json
      _append "data[\"#{name}\"] = #{name};"
    elsif @state == :from_json
      if options[:default].nil?
        _append "if (data[\"#{name}\"].exists())"
        _append "  #{name} = data[\"#{name}\"].as<#{type}>();"
      else
        _append "#{name} = data[\"#{name}\"].defaults_to<#{type}>(#{options[:default]});"
      end
    end
  end

  def has_one type, name, options = {}
    if @state == :field_scope
      definition = "#{@id_type} #{name}_id"
      _append "#{definition};"
    elsif @state == :public_scope
      _append "#{@id_type} get_#{name}_id() const { return #{name}_id; }"
      _append "void set_#{name}_id(#{@id_type} value) { #{name}_id = value; }"
    elsif @state == :serialize
      @src += " & #{name}_id"
    elsif @state == :to_json
      _append "data[\"#{name}_id\"] = #{name}_id;"
    elsif @state == :from_json
      _append "if (data[\"#{name}_id\"].exists())"
      _append "  #{name}_id = data[\"#{name}_id\"].as<#{@id_type}>();"
    end
  end

  def has_many type, name, options = {}
    singular_name = get_singular_name name
    if @state == :field_scope
      definition = "std::vector<#{@id_type}> #{singular_name}_ids"
      _append "#{definition};"
    elsif @state == :public_scope
      _append "const std::vector<#{@id_type}> get_#{singular_name}_ids() const { return #{singular_name}_ids; }"
      _append "void set_#{singular_name}_ids(const std::vector<#{@id_type}> value) { #{singular_name}_ids = value; }"
    elsif @state == :serialize
      @src += " & #{singular_name}_ids"
    elsif @state == :to_json
      _append "data[\"#{singular_name}_ids\"].from_vector(#{singular_name}_ids);"
    elsif @state == :from_json
      _append "if (data[\"#{singular_name}_ids\"].is_array())"
      _append "  #{singular_name}_ids = data[\"#{singular_name}_ids\"].to_vector<#{@id_type}>();"
    end
  end

  def resource_name name
    if @state == :public_scope
      _append "static std::string get_resource_name() { return \"#{name}\"; }"
    end
  end
end
