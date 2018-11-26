$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/model'
require 'cppgen/generator_base'

class DestroyPreparator < GeneratorBase
  attr_accessor :data

  def prepare
    @data = {}
    Model.list.each do |model|
      @model = model
      self.instance_eval &model[:block]
    end
  end

  def has_many type, name, options = {}
    unless options[:dependent].nil?
      @data[type] ||= []
      @data[type] << ({
        type: "has_many", class: @model[:classname],
	field: name, options: options
      })
    end
  end

  def has_one type, name, options = {}
    unless options[:dependent].nil?
      @data[type] ||= []
      @data[type] << ({
        type: "has_one", class: @model[:classname],
	field: name, options: options
      })
    end
  end
end

class DestroyGenerator < GeneratorBase
  class << self
    attr_accessor :destroy_data

    def my_prepare
      destroy_preparator = DestroyPreparator.new
      destroy_preparator.prepare
      @destroy_data = destroy_preparator.data
    end

    def extension ; ".destroy.cpp" ; end

    def make_file filename, data
      include = "lib/" + filename[0...-2] + "hpp"
      source  = "#include \"#{include}\"\n"
      source += "#include \"lib/#{filename[0...-2]}queries.hpp\"\n"
      source += "#include \"app/models/helpers.hpp\"\n"
      source += "#include <crails/odb/any.hpp>\n"
      source += "#include <#{GeneratorBase.odb_connection[:include]}>\n"
      source += "#include \"lib/odb/application-odb.hxx\"\n"
      source += (collect_includes_for filename).join "\n"
      source += "\n" + (data[:bodies].join "\n")
    end
  end
      
  def generate_for object
    reset
    DestroyGenerator.my_prepare if DestroyGenerator.destroy_data.nil?
    @finalclass = object[:classname]
    @klassname = get_classname(object)
    _append "void #{@klassname}::on_dependent_destroy(ODB::id_type self_id)"
    _append "{"
    unless object[:classname].nil?
      @indent += 1
      depended_by = DestroyGenerator.destroy_data[object[:classname]]
      if depended_by.class == Array
        _append "auto& database = *#{GeneratorBase.odb_connection[:object]}::instance;"
        depended_by.each do |relation|
          _append "// #{relation[:class]}"
          _append "{"
          @indent += 1
          if relation[:type] == "has_one"
            one_to_many relation
          elsif relation[:type] == "has_many"
            many_to_many relation
          end
          @indent -= 1
          _append "}\n"
        end
      end
      @indent -= 1
    end
    _append "}"
    @src
  end

  def one_to_many relation
    _append "typedef odb::query<::#{relation[:class]}> Query;"
    _append "odb::result<::#{relation[:class]}> models;"
    _append "Query query;\n"

    if relation[:options][:joined] != false
      _append "query = Query::#{relation[:field]}->id == self_id;"
    else
      _append "query = Query::#{relation[:field]}_id == self_id;"
    end
    _append "database.find<::#{relation[:class]}>(models, query);"
    _append "for (auto model : odb::to_vector<::#{relation[:class]}>(models))"
    if relation[:options][:dependent] == :destroy
      _append "  database.destroy(model);"
    elsif relation[:options][:dependent] == :unlink
      _append "{"
      if relation[:options][:joined] != false
        _append "  model.set_#{relation[:field]}(nullptr);"
      else
        _append "  model.set_#{relation[:field]}_id(0);"
      end
      _append "  database.save(model);"
      _append "}"
    end
  end

  def many_to_many relation
    if relation[:options][:joined] != false
      outer_name = (relation[:field].split(/-|_|\s/).each do |i| i.capitalize! end).join
      view_klass = "#{relation[:class].split("::").join}By#{outer_name}"
      klassname  = @klassname.split("::").last
      singular_name = get_singular_name relation[:field]
      _append "typedef odb::query<#{view_klass}> Query;"
      _append "odb::result<#{view_klass}> models;"
      _append "Query query;\n"
      _append "query = Query::#{klassname}::id == self_id;"
      _append "database.find<#{view_klass}>(models, query);"
      _append "for (auto model : odb::to_vector<#{relation[:class]}, #{view_klass}>(models))"
      _append "{"
      @indent += 1
      if relation[:options][:dependent] == :destroy
	_append "if (model.get_#{relation[:field]}().size() == 1)"
	_append "  database.destroy(model);"
	_append "else"
	_append "{"
        _append "  model.remove_#{singular_name}(*static_cast<#{@finalclass}*>(this));"
	_append "  database.save(model);"
	_append "}"
      else relation[:options][:dependent] == :unlink
        _append "model.remove_#{singular_name}(*static_cast<#{@finalclass}*>(this));"
	_append "database.save(model);"
      end
      @indent -= 1
      _append "}"
    else
      _append "typedef odb::query<#{relation[:class]}> Query;"
      _append "std::vector<ODB::id_type> ids = { self_id };"
      _append "odb::result<#{relation[:class]}> models;"
      _append "Query query;\n"
      id_field = "#{get_singular_name relation[:field]}_ids"
      _append "query = Query::#{id_field} + \"@>\" + ODB::array_to_string(ids, \"int\");"
      _append "database.find<#{relation[:class]}>(models, query);"
      _append "for (auto model : odb::to_vector<#{relation[:class]}>(models))"
      _append "{"
      @indent += 1
      if relation[:options][:dependent] == :destroy
        _append "auto id_list = model.get_#{id_field}();\n"
        _append "if (id_list.size() == 1)"
        _append "  database.destroy(model);"
        _append "else"
        _append "{"
        _append "  id_list.erase(std::find(id_list.begin(), id_list.end(), self_id));"
        _append "  model.set_#{id_field}(id_list);"
        _append "  database.save(model);"
        _append "}"
      elsif relation[:options][:dependent] == :unlink
        _append "auto id_list = model.get_#{id_field}();\n"
        _append "id_list.erase(std::find(id_list.begin(), id_list.end(), self_id));"
        _append "model.set_#{id_field}(id_list);"
        _append "database.save(model);"
      end
      @indent -= 1
      _append "}"
    end
  end
end
