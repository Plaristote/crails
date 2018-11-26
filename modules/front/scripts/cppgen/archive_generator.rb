$: << (File.dirname(__FILE__) + "/..")
require 'cppgen/model'
require 'cppgen/generator_base'

class ArchiveGenerator < GeneratorBase
  def self.is_file_based? ; false ; end

  def reset
    @src = ""
    @declarations = []
    super
  end

  def should_generate_for_object
    (not object[:header].nil?) && (not object[:classname].nil?)
  end

  def generate_for object
    reset
    _append "#include <crails/front/archive.hpp>"
    _append "#include <crails/renderer.hpp>"
    _append "#include \"#{object[:header]}\"\n"
    _append "void #{object[:classname]}::serialize(IArchive& archive)"
    _append "{"
    @src += "  archive & id"
    self.instance_eval &object[:block]
    @src += ";\n"
    _append "}\n"

    _append "void #{object[:classname]}::serialize(OArchive& archive)"
    _append "{"
    @src += "  archive & id"
    self.instance_eval &object[:block]
    @src += ";\n"
    _append "}"

    generate_archive_views object
  end

  def generate_archive_views object
    @src += "\n"
    ptr_type = "#{object[:classname]}*"
    funcname_prefix = "render_#{object[:classname].underscore}"
    funcname = "#{funcname_prefix}_show_archive"
    _append "std::string #{funcname}(const Crails::Renderer* renderer, Crails::SharedVars& vars)"
    _append "{"
    @indent += 1
    _append "auto* model = Crails::cast<#{ptr_type}>(vars, \"model\");"
    _append "OArchive archive;\n"
    _append "model->serialize(archive);"
    _append "return archive.as_string();"
    @indent -= 1
    _append "}\n"

    funcname = "#{funcname_prefix}_index_archive"
    _append "std::string #{funcname}(const Crails::Renderer* renderer, Crails::SharedVars& vars)"
    _append "{"
    @indent += 1
    _append "auto* models = Crails::cast<std::vector<#{object[:classname]}>*>(vars, \"models\");"
    _append "OArchive archive;"
    _append "unsigned long size = models->size();\n"
    _append "archive & size;"
    _append "for (auto& model : *models)"
    _append "  model.serialize(archive);"
    _append "return archive.as_string();"
    @indent -= 1
    _append "}"
  end

  def property type, name, options = {}
    @src += " & #{name}"
  end

  def has_one type, name, options = {}
    if options[:joined] != false
      puts "WARNING: unsupported joined has_one in cppgen/archive_generator"
    else
      @src += " & #{name}_id"
    end
  end

  def has_many type, name, options = {}
    singular_name = get_singular_name name
    if options[:joined] != false
      puts "WARNING: unsupported joined has_many in cppgen/archive_generator"
    else
      @src += " & #{singular_name}_ids"
    end
  end

  class << self
    def extension ; ".archive.cpp" ; end

    def make_file filename, data
      source = ""
      source += (collect_includes_for filename).join("\n")
      source += "\n" + (data[:bodies].join "\n")
      source
    end
  end
end
