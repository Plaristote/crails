$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/model'
require 'cppgen/generator_base'
require 'cppgen/validations'

class QueryGenerator < GeneratorBase
  def reset
    super
    @required_params = []
  end

  def generate_for object
    @headerfile = object[:header]
    unless @headerfile.nil?
      @modelname = object[:name].underscore
      @klassname = get_classname(object)
      @finalklass = object[:classname]
      reset
      self.instance_eval &object[:block]
      "# include \"#{@headerfile}\"\n" + @src
    else
      @src
    end
  end

  def resource_name name
  end

  def property type, name, options = {}
  end

  def validation type, name, data
  end

  def has_one type, name, options = {}
  end

  def has_many type, name, options = {}
    type = get_type type
    if options[:joined] != false
      _join_based_has_many type, name, options
    end
  end
  
  def _join_based_has_many type, name, options
    tptr = ptr_type type
    list_type = "std::list<#{tptr} >"
    singular_name = get_singular_name name

    outer_name = (name.split(/-|_|\s/).each do |i| i.capitalize! end).join
    query_object = "#{@finalklass.split("::").join}By#{outer_name}"

    _append <<CPP
  #pragma db view pointer(std::shared_ptr) object(#{type}) object(#{@finalklass} inner)
  struct #{query_object}
  {
    std::string get_database_name() const { return #{type}().get_database_name(); }
    ODB::id_type get_id() const { return self ? self->get_id() : 0; }
    std::shared_ptr<#{@finalklass}> self;
    operator #{@finalklass}() const { return *self; }

    #pragma db view pointer(std::shared_ptr) object(#{type}) object(#{@finalklass} inner)
    struct Count
    {
      #pragma db column("count(" + #{@finalklass}::id + ")")
      size_t value;
    };
  };
CPP
  end

  class << self
    def extension ; ".queries.hpp" ; end

    def make_file filename, data
      base = "lib/" + filename[0...-3]
      include = base + ".hpp"
      file_define = "_#{filename[0...-3].upcase.gsub "/", "_"}_QUERIES_HPP"
      source = <<CPP
#ifndef  #{file_define}
# define #{file_define}
#{(collect_includes_for filename).join "\n"}
#{data[:bodies].join "\n"}
#endif
CPP
    end
  end
end

