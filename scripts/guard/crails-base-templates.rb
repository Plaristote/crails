require 'guard/crails-base'
require 'erb'
require 'fileutils'

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
      FileUtils.mkdir_p "lib/renderers"
      write_file_if_changed filename, code
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

    def write_file_if_changed filename, content
      if (not File.exists? filename) || (File.read(filename) != content)
        File.open filename, 'w' do |file|
          file.write content
          puts ">> Generated file #{filename}"
        end
      else
        puts ">> No changes to file #{filename}"
      end
    end

    def write_template_to_file filename, binding_context
      template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/#{@template_type}_template.cpp.erb").read, nil, '-'
      code = template.result(binding_context)
      FileUtils.mkdir_p "lib/#{filename}"
      write_file_if_changed "lib/#{filename}/source.cpp", code
    end

    def process_lines lines
      include_lines = Array.new
      linking_lines = Array.new
      content_lines = Array.new
      part          = 0

      lines.each do | line |
        if part == 0
          if line[0] == '#' || /^\s*using\s+namespace\s+/.match(line) != nil || /^\s*typedef\s+/.match(line)
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
          type = line.scan /^(unsigned\s+)?([a-zA-Z0-9_:]+(<[a-zA-Z_0-9:\s<>*&]+>){0,1}[*&]*)/
          name = line.scan /@[a-zA-Z_]+/
          dflt = line.scan /\=(.*);/
          if not type.nil? and not type[0].nil? and not name.nil? and not name[0].nil?
            type = type.flatten[0...-1].join ''
            name = name.first[1..name.first.size]
            instance_variables << "#{type} #{name};"
            cast_params = if dflt.count == 0 then
              "(vars, \"#{name}\")"
            else
              "(vars, \"#{name}\", #{dflt.flatten.last})"
            end
            line = if is_type_a_reference? type
              "#{name}(*(Crails::cast<#{type[0...-1]}*>#{cast_params}))"
            else
              "#{name}(Crails::cast<#{type} >#{cast_params})"
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
