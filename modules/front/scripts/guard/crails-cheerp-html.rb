require 'guard/crails-base'
require 'guard/crails-notifier'
require 'pathname'
require 'nokogiri'
require 'fileutils'

class String
  def camelize
    parts = self.split(/_+|-+|\W+/)
    (parts.map do |part| part.capitalize end).join
  end

  def dasherize
    tmp = self.gsub(/([A-Z]+)/, '_\1')
    parts = tmp.split(/_+|-+|\W+/)
    parts.select! {|item| item != ""}
    (parts.map do |part| part.downcase end).join '-'
  end
end

class CrailsCheerpHtmlGenerator
  def initialize output
    @output = output
  end

  def compiled_header_path_for filepath
    @output + filepath.delete_suffix(File.extname filepath) + ".hpp"
  end

  def generate filepath
    src = File.read filepath
    src.gsub! "<template",  "<html"
    src.gsub! "</template>", "</html>"

    document = Nokogiri::HTML.parse(src)
    @html = document.xpath("html")
    @head = document.xpath("//head")
    @body = document.xpath("//body")

    @inline_ref_count = 0
    @named_elements = {}
    @element_types = {}
    @ref_types = {}

    klassname = File.basename(filepath, ".html").to_s.camelize
    superclass = if @html.attribute("extends").nil? then "Crails::Front::Element" else @html.attribute("extends").value end
    define_name = "__CRAILS_FRONT_HTML_#{klassname}__"

    result  = "#ifndef #{define_name}\n"
    result += "#define #{define_name}\n\n"
    
    result += "#include <crails/front/bindable.hpp>\n"
    result += "#include <crails/front/element.hpp>\n"
    result += "#include <crails/front/signal.hpp>\n"
    result += append_includes + "\n"
    result += "namespace HtmlTemplate\n"
    result += "{\n"
    result += "  class #{klassname} : public #{superclass}\n"
    result += "  {\n"
    result += "    Crails::Front::BoundAttributes bound_attributes;\n"
    result += "  protected:\n"
    result += "    Crails::Signal<std::string> signaler;\n"
    result += append_attributes
    result += append_refs
    result += "  private:\n"
    result += append_inline_refs
    result += "  public:\n"
    result += "    #{klassname}()\n"
    result += "    {\n"
    result += append_constructor
    result += "    }\n\n"
    result += "    void bind_attributes()\n"
    result += "    {\n"
    result += "      bound_attributes.enable(signaler);\n"
    result += append_bind_attributes
    result += "    }\n\n"
    result += "    void trigger_binding_updates()\n"
    result += "    {\n"
    result += "      bound_attributes.update();\n"
    result += append_bind_updates
    result += "    }\n\n"
    result += append_code
    result += "  };\n"
    result += "}\n\n"
    result += "#endif"
    result
  end

  def append_bind_attributes parent = nil
    parent = @body if parent.nil?
    result = ""
    parent.children.each do |el|
      if !el["ref"].nil? && @element_types.keys.include?(el.name)
        result += "      #{el["ref"]}.bind_attributes();\n"
      end
      append_bind_attributes el
    end
    result
  end

  def append_bind_updates parent = nil
    parent = @body if parent.nil?
    result = ""
    parent.children.each do |el|
      if !el["ref"].nil? && @element_types.keys.include?(el.name)
        result += "      #{el["ref"]}.trigger_binding_updates();\n"
      end
      append_bind_attributes el
    end
    result
  end

  def append_code
    result = ""
    @head.css("script").each do |script|
      result += "    " + script.text.strip + "\n"
    end
    result
  end

  def append_includes
    result = ""
    @head.css("include").each do |include_attribute|
      include_path = include_attribute["src"]
      unless include_attribute["require"].nil?
        include_attribute["require"].to_s.split(";").each do |type|
          parts = type.split("<")
          parts[0].gsub! "::", "_"
          html_type = parts.join("<").dasherize
          @element_types[html_type] = type
        end
      end
      result += "#include \"#{include_path}\"\n"
    end
    result
  end

  def append_attributes
    result = ""
    @head.css("attribute").each do |attribute|
      type = attribute["type"]
      name = attribute["name"]
      result += "    #{type} #{name};\n"
    end
    result
  end

  def append_refs parent = nil
    parent = @body if parent.nil?
    result = ""
    parent.children.each do |el|
      unless el.attributes["ref"].nil?
        type = if @element_types.keys.include? el.name
                 @element_types[el.name]
               else
                 "Crails::Front::Element"
               end
        result += "    #{type} #{el.attributes["ref"]};\n"
        @named_elements[el.path] = el.attributes["ref"]
        @ref_types[el.attributes["ref"]] = type
      end
      result += append_refs el
    end
    result
  end

  def has_trigger_attributes? el
    result = false
    el.attributes.each do |key,value|
      result = true if key.end_with?(".trigger")
    end
    result
  end

  def append_inline_refs parent = nil
    parent = @body if parent.nil?
    result = ""
    parent.children.each do |el|
      if el["ref"].nil? && (@element_types.keys.include?(el.name) || has_trigger_attributes?(el))
        varname   = "element_#{@inline_ref_count}"
        type      = @element_types[el.name] || "Crails::Front::Element"
        el["ref"] = varname
        result   += "    #{type} #{varname};\n"
        @ref_types[varname] = type
        @named_elements[el.path] = varname
        @inline_ref_count += 1
      end
    end
    result
  end

  def append_constructor
    result = ""
    result += "      std::map<std::string, Crails::Front::Element> element_cache;\n\n"

    result += append_constructor_refs @body
    result += append_bound_attributes @body
    result += "      inner({#{append_children @body, 8}\n    });\n"
    result
  end

  def append_constructor_refs parent
    result = ""
    parent.children.each do |el|
      if (!el["ref"].nil?) && (@ref_types[el["ref"]].nil?)
        result += "      #{el["ref"]} = Crails::Front::Element(\"#{el.name}\");\n"
      end
      result += append_constructor_refs el
    end
    result
  end

  def append_bound_attributes parent
    result = ""
    parent.children.each do |el|
      el_is_cached = false
      el_is_ref    = !(el.attributes["ref"].nil?)
      attributes_to_clean_up = []
      el.attributes.each do |key, value|
        if key.end_with? ".trigger"
          if !el_is_ref && !el_is_cached
            result += "      element_cache.emplace(\"#{el.path}\", Crails::Front::Element(\"#{el.name}\"));\n"
            el_is_cached = true
            @named_elements[el.path] = "element_cache[\"#{el.path}\"]"
          end
          result += "      #{@named_elements[el.path]}.events->on(\"#{key.delete_suffix(".trigger")}\", [this](client::Event* _event) { #{value}; });\n"
          attributes_to_clean_up << key
        elsif key.end_with? ".bind"
          if !el_is_ref && !el_is_cached
            result += "      element_cache.emplace(\"#{el.path}\", Crails::Front::Element(\"#{el.name}\"));\n"
            el_is_cached = true
            @named_elements[el.path] = "element_cache[\"#{el.path}\"]"
          end
          attribute_name = key.split(".").first

          is_cpp = attribute_name.start_with?("cpp::")
          attribute_name = attribute_name[5..-1] if is_cpp

          bind_mode_match = value.to_s.match(/\s*&\s*(throttle|signal):([a-zA-Z0-9_-]+)$/)
          if bind_mode_match.nil?
            bind_mode = ""
          else
            value = value.to_s.delete_suffix bind_mode_match[0]
            bind_mode_enum = case bind_mode_match[1]
                             when 'signal' then "SignalBind"
                             when 'throttle' then "ThrottleBind"
                             else "StaticBind"
                             end
            bind_mode = ".use_mode(Crails::Front::Bindable::#{bind_mode_enum}, \"#{bind_mode_match[2]}\")"
          end

          initializer = unless is_cpp
            "Crails::Front::Bindable(#{@named_elements[el.path]}, \"#{attribute_name}\", [this]() -> std::string { return boost::lexical_cast<std::string>(#{value}); })"
          else
            "Crails::Front::Bindable([this]() { #{@named_elements[el.path]}.set_#{attribute_name}(#{value}); })"
          end

          result += "      bound_attributes.push_back(#{initializer}#{bind_mode});\n"
          attributes_to_clean_up << key
        end
      end
      attributes_to_clean_up.each do |key| el.remove_attribute(key) end
      el.children.each do |child|
        result += append_bound_attributes child
      end
    end
    result
  end

  def append_children parent, indent
    result = ""
    count = 0
    parent.children.each do |el|
      if el.name != "text"
        is_ref = false
        hard_attributes  = []
        el.attributes.each do |key, value|
          if key == "ref"
            is_ref = true
          elsif !(key.end_with? ".bind")
            hard_attributes << "{\"#{key}\",\"#{value}\"}"
          end
        end

        result += "," if count > 0
        result += "\n" + (" " * indent)

        result += if @named_elements[el.path].nil?
          "Crails::Front::Element(\"#{el.name}\")"
        else
          @named_elements[el.path]
        end

        result += ".attr({#{hard_attributes.join ','}})" if hard_attributes.count > 0
        children_result = append_children el, indent + 2
        result += ".inner({#{children_result}\n#{" " * indent}})" if children_result.length > 0

        count += 1
      elsif !(el.text.strip.empty?)
        escaped_text = el.text.gsub /"/, "\\\""
        result += "\n" + (" " * indent)
        result += "Crails::Front::Element(\"span\").text(\"#{escaped_text}\")"
        count += 1
      end
    end
    result
  end
end


module ::Guard
  class CrailsCheerpHtml < CrailsPlugin
    def initialize arg
      @input  = arg[:input]  || "front"
      @output = arg[:output] || "lib/cheerp-html"
      super
    end

    def run_all
      compile
    end

    def compile
      FileUtils.rm_rf @output
      watched_files.each do |file|
        generator   = CrailsCheerpHtmlGenerator.new @output
        output_file = generator.compiled_header_path_for file[@input.length..-1]

        FileUtils.mkdir_p File.dirname(output_file)
        File.open output_file, 'w' do |f|
          f.write generator.generate(file)
        end
      end
    end
  end
end
