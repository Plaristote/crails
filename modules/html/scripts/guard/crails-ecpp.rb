require 'guard/crails-base-templates'
require 'guard/crails-notifier'

module ::Guard
  class CrailsEcpp < CrailsTemplatePlugin
    def initialize arg
      super arg
      @template_type = "html"
      @extension     = "ecpp"
    end

    def compile_html filename
      context_global = 1
      context_balise = 2

      file_content  = (File.open filename).read
      view_name, class_name, function_name = get_names filename

      lines    = file_content.split "\n"
      include_lines, linking_lines, content_lines = process_lines lines
      contents = content_lines.join "\n"
      code     = String.new
      i        = 0
      context  = context_global
      out_var  = "html_stream"

      code = "#{out_var} << \""

      while i < contents.length
        if context == context_global
          if contents[i] == '"'
            code += '\\"'
            i    += 1
          elsif contents[i..i+2] == '<%='
            code += "\" << "
            context = context_balise
            i    += 3
          elsif contents[i..i+1] == '<%'
            code += "\";\n"
            context = context_balise
            i    += 2
          elsif contents[i] == "\n"
            code += "\\n"
            i    += 1
          else
            code += contents[i]
            i    += 1
          end
        elsif context == context_balise
          if contents[i..i+1] == '%>'
            code   += ";\n#{out_var} << \""
            context = context_global
            i      += 2
          else
            code += contents[i]
            i    += 1
          end
        else
          i += 1
        end
      end

      if context == context_global
        code += '"'
      end
      code += ';'

      instance_variables, linking_lines = process_linking_lines linking_lines
      code = process_linked_variables code

      code = code.gsub /\)\s*yields([^a-zA-Z_(\[\]])/, ', [this]() -> string { std::stringstream html_stream; \1'
      code = code.gsub /\byend([^a-zA-Z_(\[\]])/, 'return (html_stream.str()); })\1'
      code = code.gsub /\bdo([^a-zA-Z_(\[\]])/, '{\1'
      code = code.gsub /\bend([^a-zA-Z_(\[\]])/, '; }\1'

      write_template_to_file(filename, instance_eval { binding })
    end
  end
end
