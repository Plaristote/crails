require 'guard/crails-base-templates'
require 'guard/crails-notifier'

module ::Guard
  class CrailsCjson < CrailsTemplatePlugin
    def initialize
      @template_type = "json"
      @extension     = "ejson"
    end

    def compile_ecpp filename
      file_content  = (File.open filename).read
      view_name, class_name, function_name = get_names filename

      lines = file_content.split "\n"
      include_lines, linking_lines, content_lines = process_lines lines
      instance_variables, linking_lines = process_linking_lines linking_lines
      code = process_linked_variables(contents_line.join "\n")
      write_template_to_file(instance_eval { binding })
    end
  end
end
