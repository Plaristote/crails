MAINCPP_INITIALIZER_HEADER = "// Initializers"
MAINCPP_FINALIZER_HEADER   = "// Finalizers"

class MainCpp
  class << self
    def add_initializer line
      append_to MAINCPP_INITIALIZER_HEADER, "  #{line}"
    end

    def add_finalizer line
      append_to MAINCPP_FINALIZER_HEADER, "  #{line}"
    end

    def add_include name
      append_to(
        "#include <crails/server.hpp>",
        "#include <#{name}>"
      )
    end

  private
    def append_to header, str
      content = File.read 'app/main.cpp'
      content.gsub!(
        header,
        "#{header}\n#{str}"
      )
      File.open('app/main.cpp', 'w') do |file|
        file.write content
      end
    end
  end
end
