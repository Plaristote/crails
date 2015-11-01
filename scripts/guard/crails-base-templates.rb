require 'guard/crails-base'
require 'erb'

class String
  def underscore
    self.gsub(/::/, '/').
    gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2').
    gsub(/([a-z\d])([A-Z])/,'\1_\2').
    tr("-", "_").
    downcase
  end
end

module ::Guard
  class CrailsTemplatePlugin < CrailsPlugin
    def run_all
      run_on_modifications watched_files
    end

    def run_on_modifications(paths)
      paths.each do |path|
        self.send "compile_#{@template_type}", path
      end
      compile_renderer watched_files
    end

  protected
    def compile_renderer watched_files
      templates = Array.new

      watched_files.each do |filename|
        view_name, class_name, function_name = get_names filename
         templates << { name: view_name, function: function_name }
      end

      template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/#{@template_type}_renderer.cpp.erb").read, nil, '-'
      code = template.result(instance_eval { binding })
      filename = "lib/renderers/#{@template_type}.cpp"
      File.open filename, 'w' do |file|
        file.write code
        puts ">> Generated file #{filename}"
      end
    end

    def get_names filename
      #                              Folder          Name|Format        Extension
      view_name     = (filename.scan /(app\/views\/)?(.*)([.][a-z0-9]+)?[.]#{@extension}$/).flatten[1]
      class_name    = (view_name.split /_|-|\b|\//).map {|w| w.strip.capitalize }.join
      class_name    = class_name.gsub /[.\/]/, ''
      class_name   += @extension.capitalize
      function_name = "render_#{class_name.underscore}_#{@template_type}"
      [view_name, class_name, function_name]
    end

    def write_template_to_file filename, binding_context
      template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/#{@template_type}_template.cpp.erb").read, nil, '-'
      code = template.result(binding_context)

      # Write source file
      File.open "#{filename}.cpp", 'w' do |file|
        file.write code
        puts ">> Generated file #{filename}.cpp"
      end
    end

    def process_lines lines
      include_lines = Array.new
      linking_lines = Array.new
      content_lines = Array.new
      part          = 0

      lines.each do | line |
        if part == 0
          if line[0] == '#' || /^\s*using\s+namespace\s+/.match(line) != nil
            include_lines << line
          else
            linking_lines << line
          end
        else
          content_lines << line
        end
        if line =~ /^\/\/ END LINKING/
          part += 1
        end
      end
      [include_lines, linking_lines, content_lines]
    end

    def process_linked_variables code
      #            Name      Sep Type
      code.gsub /@([a-zA-Z_]+)\(([^)]+)\)/, '(boost::any_cast<\2 >(vars["\1"]))'
    end

    def process_linking_lines linking_lines
      instance_variables = []
      tmp_lines = linking_lines
      linking_lines = []
      variables_initialization = []
      tmp_lines.each do | line |
        if line.match /@[a-zA-Z_]+/
          type = line.scan /^(unsigned\s+)?([a-zA-Z0-9_:]+(<[a-zA-Z_0-9:]+[*&]*>){0,1}[*&]*)/
          name = line.scan /@[a-zA-Z_]+/
          if not type.nil? and not type[0].nil? and not name.nil? and not name[0].nil?
            type = type.flatten[0...-1].join ''
            name = name.first[1..name.first.size]
            instance_variables << "#{type} #{name};"
            line = if is_type_a_reference? type
              "#{name}(*(boost::any_cast<#{type[0...-1]}*>(vars[\"#{name}\"])))"
            else
              "#{name}(boost::any_cast<#{type} >(vars[\"#{name}\"]))"
            end
            variables_initialization << line
            next
          end
        end
        linking_lines << line
      end
      {
        instance_variables: instance_variables,
        linking_lines: linking_lines,
        variables_initialization: variables_initialization
      }
    end
    
    def is_type_a_reference? type
      not (type =~ /&$/).nil?
    end
  end
end
