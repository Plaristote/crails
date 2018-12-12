$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/edit_generator'

class EditWithFrontGenerator < EditGenerator
  def _append_macro str
    @src ||= ""
    @src += str + "\n"
  end

  def generate_json_methods object
    super
    _append_macro "#ifdef #{DataWithFrontGenerator.client_define}"
    _append "void #{@klassname}::from_json(Data data)"
    _append "{"
    @indent += 1
    _append "id = data[\"id\"].defaults_to<ODB::id_type>(ODB_NULL_ID);"
    _append "edit(data);"
    @indent -= 1
    _append "}"
    _append_macro "#endif"
  end

  def validation type, name, data
    if data[:uniqueness] == true
      _append_macro "#ifndef #{DataWithFrontGenerator.client_define}"
      _append validate_uniqueness type, name
      _append_macro "#endif"
      data[:uniqueness] = false
    end
    super type, name, data
  end

  def has_one_getter type, name, options
    type = get_type type
    tptr = ptr_type type
    _append_macro "#ifndef #{DataWithFrontGenerator.client_define}"
    super
    _append_macro "#else"
    _append "#{tptr} #{@klassname}::get_#{name}()"
    _append "{"
    _append "  #{tptr} model = std::make_shared<#{type}>();"
    _append "  model->set_id(get_#{name}_id());"
    _append "  model->fetch();"
    _append "  return model;"
    _append "}"
    _append_macro "#endif" 
  end

  def has_many_fetch type, name, options
    tptr = ptr_type type
    singular_name = get_singular_name name
 
    _append_macro "#ifndef #{DataWithFrontGenerator.client_define}"
    super type, name, options
    _append_macro "#else"
    _append "Crails::Front::Promise #{@klassname}::fetch_#{name}()"
    _append "{"
    @indent += 1
    _append "std::vector<Crails::Front::Promise> promises;\n"
    _append "for (auto id : #{singular_name}_ids)"
    _append "{"
    @indent += 1
    _append "#{tptr} model;\n"
    _append "model->set_id(id);"
    _append "promises.push_back(model->fetch());"
    _append "#{name}.push_back(model);"
    _append "#{name}_fetched = true;"
    @indent -= 1
    _append "}"
    _append "return Crails::Front::Promise::all(promises);"
    @indent -= 1
    _append "}"
    _append_macro "#endif" 
  end

  def property type, name, options = {}
    if options[:ready_only] == true && rendering_edit?
      options[:read_only] = false
      _append_macro "#ifdef #{DataWithFrontGenerator.client_define}"
      super
      _append_macro "#endif"
    else
      super
    end
  end

  def has_many type, name, options = {}
    if options[:read_only] == true && rendering_edit?
      options[:read_only] = false
      _append_macro "#ifdef #{DataWithFrontGenerator.client_define}"
      super
      _append_macro "#endif"
    else
      super
    end
  end

  def has_one type, name, options = {}
    if options[:read_only] == true && rendering_edit?
      options[:read_only] = false
      _append_macro "#ifdef #{DataWithFrontGenerator.client_define}"
      super
      _append_macro "#endif"
    else
      super
    end
  end

  class << self
    def generate_includes
<<CPP
#ifndef #{DataWithFrontGenerator.client_define}
#{super}
#else
# include <crails/front/mvc/helpers.hpp>
#endif
CPP
    end

    def sourcefile_to_destfile sourcefile
      base      = super sourcefile
      basepath  = Pathname.new base
      parentdir = basepath.dirname.to_s + "/shared"
      "#{parentdir}/#{basepath.basename}"
    end
  end
end
