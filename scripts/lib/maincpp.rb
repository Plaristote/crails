MAINCPP_INITIALIZER_HEADER = "// Initializers"
MAINCPP_FINALIZER_HEADER   = "// Finalizers"

class SourceEditor
  def add_initializer line
    append_to @initializer_header, "  #{line}"
  end

  def add_finalizer line
    append_to @finalizer_header, "  #{line}"
  end

  def add_include name
    append_to(
      "#include <#{@known_header}>",
      "#include <#{name}>"
    )
  end
private
  def append_to header, str
    content = File.read @source_file
    content.gsub!(
      header,
      "#{header}\n#{str}"
    )
    File.open(@source_file, 'w') do |file|
      file.write content
    end
  end
end

class MainCppEditor < SourceEditor
  def initialize
    @initializer_header = "// Initializers"
    @finalizer_header   = "// Finalizers"
    @source_file        = "app/main.cpp"
    @known_header       = "crails/server.hpp"
  end
end

class RenderersCppEditor < SourceEditor
  def initialize
    @initializer_header = "// Append renderers"
    @source_file        = "config/renderers.cpp"
    @known_header       = "crails/renderer.hpp"
  end
end
