require 'guard/crails-base'
require 'guard/crails-notifier'

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
  class CrailsCjson < CrailsPlugin
    def run_all
      run_on_modifications watched_files
    end

    def run_on_modifications(paths)
      paths.each do |path|
        compile_ecpp path
        compile_renderer watched_files
      end
    end

  private
    def compile_renderer watched_files
      templates = Array.new

      watched_files.each do |filename|
        view_name, class_name, function_name = get_names filename
         templates << { name: view_name, function: function_name }
      end

      template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/json_renderer.cpp.erb").read, nil, '-'
      code = template.result(instance_eval { binding })
      File.open ".json_templates.cpp", 'w' do |file|
        file.write code
        puts ">> Generated file .json_templates.cpp"
      end
    end

    def get_names filename
      view_name     = (filename.scan /(app\/views\/)?(.*)[.][a-z0-9]+[.]ecpp$/).flatten[1]
      class_name    = (view_name.split /_|-|\b|\//).map {|w| w.strip.capitalize }.join
      class_name    = class_name.gsub /[.\/]/, ''
      function_name = "render_#{class_name.underscore}_json"
      [view_name, class_name, function_name]
    end

    def compile_ecpp filename
      context_global = 1
      context_balise = 2

      file_content  = (File.open filename).read
      view_name, class_name, function_name = get_names filename

      lines = file_content.split "\n"

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

      contents = content_lines.join "\n"
      code     = String.new
      i        = 0
      context  = context_global
      out_var  = "html_stream"

      instance_variables = []
      code = contents

      tmp_lines = linking_lines
      linking_lines = Array.new
      tmp_lines.each do | line |
        if line.match /@[a-zA-Z_]+/
          type = line.scan /^([a-zA-Z0-9_]+(<[a-zA-Z_0-9]+[*&]*>){0,1}[*&]*)/
          name = line.scan /@[a-zA-Z_]+/
          if not type.nil? and not type[0].nil? and not name.nil? and not name[0].nil?
            type = type.first.first
            name = name.first[1..name.first.size]
            instance_variables << "#{type} #{name};"
            line = if not (type =~ /^SmartPointer</).nil?
              # If the type is a SmartPointer, dereference
              "#{name} = #{type}(*((#{type}*)*(vars[\"#{name}\"]))); // Smart Pointer"
            elsif not (type =~ /&$/).nil?
              # If the type is a reference, dereference
              "#{name} = *((#{type[0...-1]}*)*(vars[\"#{name}\"])); // Reference"
            else
              "#{name} = (#{type})*(vars[\"#{name}\"]);"
            end
          end
        end
        linking_lines << line
      end

      #                   Type      Sep Name
      code = code.gsub /@([a-zA-Z_]+)\(([^)]+)\)/, '((\2)*(vars["\1"]))'

      template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/json_template.cpp.erb").read, nil, '-'
      code = template.result(instance_eval { binding })

      # Write source file
      File.open "#{filename}.cpp", 'w' do |file|
        file.write code
        puts ">> Generated file #{filename}.cpp"
      end
    end
  end
end
