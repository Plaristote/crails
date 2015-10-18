require 'guard/crails-base-templates'
require 'guard/crails-notifier'

module ::Guard
  class CrailsCjson < CrailsTemplatePlugin
    def initialize arg
      super arg
      @template_type = "json"
      @extension     = "cjson"
    end

    def compile_json filename
      file_content  = (File.open filename).read
      view_name, class_name, function_name = get_names filename

      lines = file_content.split /\r?\n/
      include_lines, linking_lines, content_lines = process_lines lines
      lines = process_linking_lines linking_lines
      code = process_linked_variables(content_lines.join "\n")
      write_template_to_file(filename, instance_eval { binding })
    end
  end
end
