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
  def initialize output, input, file
    @output   = output
    @input    = input
    @filepath = file
  end

  def relative_filepath
    @filepath[@input.length..-1]
  end

  def compiled_header_path
    @output + relative_filepath.delete_suffix(File.extname relative_filepath) + ".hpp"
  end

  def compiled_source_path
    @output + relative_filepath.delete_suffix(File.extname relative_filepath) + ".cpp"
  end

  def generate
    src = File.read @filepath
    src.gsub! "<template",  "<html"
    src.gsub! "</template>", "</html>"

    document = Nokogiri::HTML.parse(src)
    @html = document.xpath("html")
    @head = document.xpath("//head")
    @body = document.xpath("//body")
    @element_types = {}

    @repeater_count = 0

    @classes = [
      {
        typename: (File.basename(@filepath, ".html").to_s.camelize),
        extends:  if @html.attribute("extends").nil? then "Crails::Front::Element" else @html.attribute("extends").value end,
        el:       @body
      }
    ]

    find_subtypes @body, @classes.last

    source  = "#include <boost/lexical_cast.hpp>\n"
    source += "#include \"#{compiled_header_path}\"\n"

    define_name = "__CRAILS_FRONT_HTML_#{@classes.last[:typename].upcase}__"
    header  = "#ifndef #{define_name}\n"
    header += "#define #{define_name}\n\n" 
    header += "#include <crails/front/bindable.hpp>\n"
    header += "#include <crails/front/element.hpp>\n"
    header += "#include <crails/front/signal.hpp>\n"
    header += append_includes + "\n"
    header += "namespace HtmlTemplate\n"
    header += "{\n"
    header += append_classes_predeclaration
    @classes.each do |klass|
      @inline_ref_count = 0
      @named_elements   = {}
      @ref_types        = {}
      header += "\n"
      header += generate_class_header klass
      source += "\n"
      source += generate_class_source klass
    end
    header += "}\n\n"
    header += "#endif"
    header

    { header: header, source: source }
  end

  def append_classes_predeclaration
    result = ""
    @classes.each do |klass|
      result += "  class #{klass[:typename]};\n"
    end
    result
  end

  def get_subtype_for el
    @classes.each {|klass| return klass if klass[:el] == el}
    nil
  end

  def find_subtypes root, parent
    root.children.each do |el|
      if !el["repeat.for"].nil?
        @repeater_count += 1
        descriptor = {
          typename: "#{@classes.last[:typename]}Repeater_#{@repeater_count}",
          extends:  "Crails::Front::Element",
          parent:   parent,
          el:       el
        }
        @classes.prepend descriptor
        find_subtypes el, descriptor
      else
        find_subtypes el, parent
      end
    end
  end

  def generate_class_header klass
    superclass = if klass[:extends].nil? then "Crails::Front::Element" else klass[:extends] end
    result  = ""
    result += "  class #{klass[:typename]} : public #{superclass}\n"
    result += "  {\n"
    result += "    Crails::Front::BoundAttributes bound_attributes;\n"
    result += "  protected:\n"
    result += "    Crails::Signal<std::string> signaler;\n"
    result += "    #{klass[:parent][:typename]}* parent;\n" unless klass[:parent].nil?
    result += append_attributes if klass[:el] == @body
    result += append_refs klass[:el]
    result += "  private:\n"
    result += append_inline_refs klass[:el]
    result += "  public:\n"
    if klass[:parent].nil?
      result += "    #{klass[:typename]}();\n"
    else
      result += "    #{klass[:typename]}(#{klass[:parent][:typename]}*);\n"
    end
    result += "    void bind_attributes();\n"
    result += "    void trigger_binding_updates();\n"
    result += append_code if klass[:el] == @body
    result += "  };\n"
    result
  end

  def generate_class_source klass
    result = ""
    if klass[:parent].nil?
      result += "HtmlTemplate::#{klass[:typename]}::#{klass[:typename]}()\n"
    else
      result += "HtmlTemplate::#{klass[:typename]}::#{klass[:typename]}(#{klass[:parent][:typename]}* p) : parent(p)\n"
    end
    result += "{\n"
    result += append_constructor klass[:el]
    result += "}\n\n"
    result += "void HtmlTemplate::#{klass[:typename]}::bind_attributes()\n"
    result += "{\n"
    result += "  bound_attributes.enable(signaler);\n"
    result += append_bind_attributes klass[:el]
    result += "}\n\n"
    result += "void HtmlTemplate::#{klass[:typename]}::trigger_binding_updates()\n"
    result += "{\n"
    result += "  bound_attributes.update();\n"
    result += append_bind_updates klass[:el]
    result += "}\n"
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

  def append_constructor root
    result = ""
    result += "  std::map<std::string, Crails::Front::Element> element_cache;\n\n"

    result += append_constructor_refs root
    result += append_bound_attributes root
    result += "  inner({#{append_children root, 4}\n  });\n"
    result
  end

  def append_constructor_refs parent
    result = ""
    parent.children.each do |el|
      next unless get_subtype_for(el).nil?
      if (!el["ref"].nil?) && (@ref_types[el["ref"]].nil?)
        result += "  #{el["ref"]} = Crails::Front::Element(\"#{el.name}\");\n"
      end
      result += append_constructor_refs el
    end
    result
  end

  def append_bound_attributes parent
    result = ""
    parent.children.each do |el|
      next unless get_subtype_for(el).nil?
      el_is_cached = false
      el_is_ref    = !(el.attributes["ref"].nil?)
      attributes_to_clean_up = []
      el.attributes.each do |key, value|
        if key.end_with? ".trigger"
          if !el_is_ref && !el_is_cached
            result += "  element_cache.emplace(\"#{el.path}\", Crails::Front::Element(\"#{el.name}\"));\n"
            el_is_cached = true
            @named_elements[el.path] = "element_cache[\"#{el.path}\"]"
          end
          result += "  #{@named_elements[el.path]}.events->on(\"#{key.delete_suffix(".trigger")}\", [this](client::Event* _event) { #{value}; });\n"
          attributes_to_clean_up << key
        elsif key.end_with? ".bind"
          if !el_is_ref && !el_is_cached
            result += "  element_cache.emplace(\"#{el.path}\", Crails::Front::Element(\"#{el.name}\"));\n"
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

          result += "  bound_attributes.push_back(#{initializer}#{bind_mode});\n"
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
      next unless get_subtype_for(el).nil?
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
        generator   = CrailsCheerpHtmlGenerator.new @output, @input, file
        relative_filepath = file[@input.length..-1]
        header_file = generator.compiled_header_path
        source_file = generator.compiled_source_path

        FileUtils.mkdir_p File.dirname(header_file)
        code = generator.generate
        File.open header_file, 'w' do |f| f.write code[:header] end
        File.open source_file, 'w' do |f| f.write code[:source] end
      end
    end
  end
end
