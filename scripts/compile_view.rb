#!/usr/bin/ruby

CXT_GLOBAL  = 1
CXT_BALISE  = 2
CXT_STRING  = 3

def compile_view filename
  file_content = (File.open filename).read
  view_name    = (filename.scan /(.*)[.][a-z0-9]+[.]ecpp$/).flatten.first
  class_name   = (view_name.split /_|-|\b|\//).map {|w| w.strip.capitalize }.join
  class_name   = class_name.gsub /[.\/]/, ''

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
  context  = CXT_GLOBAL
  out_var  = "html_stream"

  code = "#{out_var} << \""

  while i < contents.length
    if context == CXT_GLOBAL
      if contents[i] == '"'
        code += '\\"'
        i    += 1
      elsif contents[i..i+2] == '<%='
        code += "\" << "
        context = CXT_BALISE
        i    += 3
      elsif contents[i..i+1] == '<%'
        code += "\";\n"
        context = CXT_BALISE
        i    += 2
      elsif contents[i] == "\n"
        code += "\\n"
        i    += 1
      else
        code += contents[i]
        i    += 1
      end
    elsif context == CXT_BALISE
      if contents[i..i+1] == '%>'
        code   += ";\n#{out_var} << \""
        context = CXT_GLOBAL
        i      += 2
      else
        code += contents[i]
        i    += 1
      end
    else
      i += 1
    end
  end

  if context == CXT_GLOBAL
    code += '"'
  end
  code += ';'

  instance_variables = []
  
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

  code = code.gsub /\)\s*yields([^a-zA-Z_(\[\]])/, ', [this]() -> string { std::stringstream html_stream; \1'
  code = code.gsub /\byend([^a-zA-Z_(\[\]])/, 'return (html_stream.str()); })\1'
  code = code.gsub /\bdo([^a-zA-Z_(\[\]])/, '{\1'
  code = code.gsub /\bend([^a-zA-Z_(\[\]])/, '; }\1'

  code = <<CPP
  #include <sstream>
  #include <iostream>
  #include <crails/shared_vars.hpp>

  #{include_lines.join "\n"}

  using namespace std;

  extern "C"
  {
    std::string generate_view(Crails::SharedVars&);
    std::string render_view(const std::string& name, Crails::SharedVars&);
  }
  
  class #{class_name}
  {
  public:
    #{class_name}(Crails::SharedVars& vars) : vars(vars)
    {
      #{linking_lines.join "\n"}
    }
    
    std::string render(void)
    {
      #{code}
      return (#{out_var}.str());
    }

  private:
    Crails::SharedVars& vars;
    std::stringstream   #{out_var};
    #{instance_variables.join "\n"}
  };

  std::string generate_view(Crails::SharedVars& vars)
  {
    #{class_name} view(vars);
    
    return (view.render());
  }
CPP

  # Needs recompiling ?
  old_cpp = (File.open filename + '.cpp').read rescue ""
  return 0 if old_cpp == code

  # Then write source file
  File.open "#{filename}.cpp", 'w' do |file|
    file.write code
    puts ">> Generated file #{filename}.cpp"
    puts "(?) With g++, compile using:"
    cmd = "g++ -Wall -g -fPIC -shared #{filename}.cpp -o #{filename}.so -I. -std=c++11"
    puts cmd
    system cmd
  end
  return 1
end

if ARGV[0].nil?
  files = Dir.glob "app/views/**/*.ecpp"

  files.each do | file |
    compile_view file
  end
else
  exit (compile_view ARGV[0])
end
