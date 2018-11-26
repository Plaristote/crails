$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/model'
require 'cppgen/generator_base'

class DataGenerator < GeneratorBase
  attr_accessor :forward_declarations

  def reset
    super
    @forward_declarations = []
    @src = {}
  end

  def _append str
    @src[@current_visibility] ||= ""
    @src[@current_visibility] += " " * (@indent * @tab_size)
    @src[@current_visibility] += str + "\n"
  end

  def get_headers
    forward_source = ""
    @forward_declarations.each do |type|
      parts = type.split('::')
      last_part = parts.last
      if parts.length > 1
        parts[0...parts.size-1].each do |part|
          forward_source += "namespace #{part} { "
        end
        forward_source += "class #{last_part};"
        (parts.size - 1).times do forward_source += " } " end
        forward_source += "\n"
      else
        forward_source += "class #{last_part};\n"
      end
    end
    forward_source
  end

  def generate_for object
    reset
    @forward_declarations << object[:classname] unless object[:classname].nil?
    @current_visibility = :_preblock
    @indent += 1
    _append "#pragma db object abstract"
    _append "struct #{object[:name]}"
    _append "{"
    @indent += 1
    _append "friend class odb::access;"
    _append "#pragma db transient"
    _append "DataTree errors;"
    @current_visibility = :public
    edit_resource_declaration
    prepare_order_by
    self.instance_eval &object[:block]
    @current_visibility = :_postblock
    @indent -= 1
    _append "};"
    @indent -= 1
    render
  end

  def edit_resource_declaration
    _append "virtual std::vector<std::string> find_missing_parameters(Data) const;"
    _append "virtual void                     edit(Data);"
    _append "virtual bool                     is_valid();"
    _append "virtual void                     on_dependent_destroy(ODB::id_type);\n"
  end

  def prepare_order_by
    _append "template<typename QUERY>"
    _append "static QUERY default_order_by(QUERY query)"
    _append "{"
    _append "  return query; // order_by"
    _append "}\n"
  end

  def render
    @current_visibility = :_result
    result = @src[:_preblock]
    [:public, :protected, :private].each do |key|
      unless @src[key].nil?
        result += "  #{key}:\n"
        result += @src[key]
      end
    end
    result += @src[:_postblock]
    result
  end

  def visibility value
    @current_visibility = value
  end

  def with_visibility value, &block
    old_visibility = @current_visibility
    visibility value
    self.instance_eval &block
    visibility old_visibility
  end

  def resource_name name
    visibility :public
    _append "static const std::string scope;"
    _append "static const std::string view;"
  end

  def order_by name
    @src[:public] = @src[:public].gsub /^(\s*).*\/\/ order_by$/, '\1' + "return query + \"ORDER BY\" + QUERY::#{name};"
  end

  def datatree_property name, options
    with_visibility :public do
      virt = if options[:allow_override] == true then "virtual " else "" end
      _append "#{virt}DataTree& get_#{name}() { return #{name}; }"
      _append "#{virt}const DataTree& get_#{name}() const { return #{name}; }"
      _append "#{virt}void set_#{name}(Data value) { this->#{name}.clear(); this->#{name}.as_data().merge(value); }"
    end
  end

  def property type, name, options = {}
    if type == 'DataTree'
      datatree_property name, options
    else
       with_visibility :public do
       virt = if options[:allow_override] == true then "virtual " else "" end
        _append "#{virt}#{type} get_#{name}() const { return #{name}; }"
        _append "#{virt}void set_#{name}(#{type} value) { this->#{name} = value; }"
      end
    end
    make_pragma_db options[:db] unless options[:db].nil?
    if options[:default].nil?
      _append "#{type} #{name};"
    else
      _append "#{type} #{name} = #{get_value options[:default]};"
    end
  end

  def make_pragma_db options, authorized_options = []
    is_option_available = lambda {|a|
      (not options[a].nil?) && (authorized_options.size == 0 || authorized_options.include?(a))
    }
    src  = ""
    src += "transient " if is_option_available.call(:transient) && options[:transient] == true
    src += "type(\"#{options[:type]}\") "          if is_option_available.call :type
    src += "column(\"#{options[:column]}\") "      if is_option_available.call :column
    src += "default(#{get_value options[:default]}) " if is_option_available.call :default
    src += "null " if is_option_available.call(:null) && options[:null] == true
    _append "#pragma db #{src}" if src.size > 0
  end

  def has_one type, name, options = {}
    @forward_declarations << type
    type = get_type type
    tptr = ptr_type type
    virt = if options[:allow_override] == true then "virtual " else "" end
    if options[:joined] != false
      with_visibility :public do
        _append "#{virt}#{tptr} get_#{name}() const { return #{name}; }"
        _append "#{virt}void set_#{name}(#{tptr} v) { this->#{name} = v; }"
        _append "#{virt}ODB::id_type get_#{name}_id() const;"
      end
      _append "#{tptr} #{name};"
    else
      with_visibility :public do
        _append "#{virt}#{tptr} get_#{name}();"
        _append "#{virt}void set_#{name}(#{tptr} v);"
        _append "ODB::id_type get_#{name}_id() const { return #{name}_id; }"
        _append "void set_#{name}_id(ODB::id_type v) { #{name}_id = v; }"
      end
      make_pragma_db options[:db] unless options[:db].nil?
      _append "ODB::id_type #{name}_id;"
    end
    with_visibility :public do
      _append <<CPP

    template<typename ARRAY>
    static void collect_#{name}(ARRAY& array, std::map<ODB::id_type, #{tptr} >& results)
    {
      for (auto model : array)
      {
        if (results.find(model.get_#{name}_id()) == results.end())
          results[model.get_#{name}_id()] = model.get_#{name}();
      }
    }

CPP
    end
  end

  def has_many type, name, options = {}
    @forward_declarations << type
    type = get_type type
    tptr = ptr_type type
    list_type = "std::list<#{tptr} >"
    singular_name = get_singular_name name
    with_visibility :public do
      virt = if options[:allow_override] == true then "virtual " else "" end
      _append "#{virt}bool update_#{name}(Data);"
      _append "#{virt}void add_#{singular_name}(#{tptr});"
      _append "#{virt}void remove_#{singular_name}(const #{type}&);"
      _append "#{virt}void collect_#{name}(std::map<ODB::id_type, #{tptr} >&);"
    end
    if options[:joined] != false
      _join_based_has_many list_type, name, options
    else
      _id_based_has_many list_type, name, options
    end
  end

  def _join_based_has_many list_type, name, options
    with_visibility :public do
      _append "const #{list_type}& get_#{name}() const { return #{name}; }"
    end
    _append "#{list_type} #{name};"
  end

  def _id_based_has_many list_type, name, options
    singular_name = get_singular_name name
    store_type = "std::vector<ODB::id_type>"
    with_visibility :public do
      _append "const #{store_type}& get_#{singular_name}_ids() const { return #{singular_name}_ids; }"
      _append "const #{list_type}& get_#{name}();"
      _append "void set_#{singular_name}_ids(const #{store_type}& val) { #{singular_name}_ids = val; #{name}_fetched = false; }"
    end
    with_visibility :private do
      _append "void fetch_#{name}();"
    end
    options[:db]        ||= {}
    options[:db][:type] ||= "INTEGER[]"
    make_pragma_db options[:db], [:type, :column, :null]
    _append "#{store_type} #{singular_name}_ids;"
    make_pragma_db transient: true
    _append "#{list_type} #{name};"
    make_pragma_db transient: true
    _append "bool #{name}_fetched = false;"
  end

  class << self
    def extension ; ".hpp" ; end

    def make_file filename, data
      file_define = "_#{filename[0...-3].upcase.gsub "/", "_"}_HPP"
      source = <<CPP
#ifndef  #{file_define}
# define #{file_define}

# include <vector>
# include <list>
# include <map>
# include <string>
# include <crails/datatree.hpp>
# include <crails/odb/id_type.hpp>
#{collect_includes_for(filename, true).join "\n"}
namespace odb { class access; }
#{data[:headers].join ""}
namespace #{NAMESPACE}
{
#{data[:bodies].join "\n"}}

#endif
CPP
    end
  end
end
