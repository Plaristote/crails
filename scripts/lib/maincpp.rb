MAINCPP_INITIALIZER_HEADER = "// Initializers"
MAINCPP_FINALIZER_HEADER   = "// Finalizers"

class SourceEditor
  def initialize
    @content = File.read @source_file
  end

  def write
    File.open(@source_file, 'w') do |file|
      file.write @content
    end
    puts "\033[32m[EDITED]\033[0m " + "File #{@source_file}"
  end
  
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
    @content.gsub!(
      header,
      "#{header}\n#{str}"
    )
  end
end

class MainCppEditor < SourceEditor
  def initialize
    @initializer_header = "// Initializers"
    @finalizer_header   = "// Finalizers"
    @source_file        = "app/main.cpp"
    @known_header       = "crails/server.hpp"
    super
  end
end

class RenderersCppEditor < SourceEditor
  def initialize
    @initializer_header = "// Append renderers"
    @source_file        = "config/renderers.cpp"
    @known_header       = "crails/renderer.hpp"
    super
  end
end

class GuardfileEditor < SourceEditor
  def initialize
    @source_file = "Guardfile"
    super
  end

  def add_task task_name, code
    @initializer_header = "group :#{task_name} do"
    add_initializer code
  end
end