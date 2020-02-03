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
    header += "#include <crails/front/repeater.hpp>\n"
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
      @root_element     = klass[:el]
      @named_elements[klass[:el].path] = "(*this)" unless klass[:el] == @body
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

  def find_repeater_anchor el
    it = el
    loop do
      it = it.previous
      break if it.nil? || it.name != "text"
    end
    unless it.nil?
      { el: it, mode: :append }
    else
      it = el
      loop do
        it = it.next
        break if it.nil? || it.name != "text"
      end
      unless it.nil?
        { el: it, mode: :prepend }
      else
        { el: el.parent, mode: :children }
      end
    end
  end

  def find_subtypes root, parent
    root.children.each do |el|
      if !el["repeat.for"].nil?
        @repeater_count += 1
        value, bind_mode = extract_bind_mode_from el["repeat.for"].to_s
        parts = value.to_s.match /^\s*(\[([^\]]+)\])?\s*([^\s]+)\s+of\s+\[([^\]]+)\]\s*(.*)$/
        if parts.nil?
          throw "invalid repeater at #{relative_filepath}:#{el.line}"
        end
        descriptor = {
          behavior: :repeater,
          typename: "#{@classes.last[:typename]}Repeatable_#{@repeater_count}",
          extends:  if @element_types.keys.include?(el.name) then @element_types[el.name] else "Crails::Front::Element" end,
          parent:   parent,
          el:       el,
          repeater: {
            ref:       "repeater_#{@repeater_count}",
            type:      parts[2] || "#{parts[4]}::value_type",
            name:      parts[3],
            list_type: parts[4],
            list:      parts[5],
            bind_mode: bind_mode,
            anchor:    find_repeater_anchor(el)
          }
        }
        @classes.prepend descriptor
        find_subtypes el, descriptor
      else
        find_subtypes el, parent
      end
    end
  end

  def append_repeaters_declarations root
    result = ""
    root.children.each do |el|
      subtype = get_subtype_for(el)
      if !subtype.nil? && subtype[:behavior] == :repeater
        element_type = "Crails::Front::Element"
        result += "    Crails::Front::Repeater<#{subtype[:repeater][:list_type]}, #{subtype[:typename]}> #{subtype[:repeater][:ref]};\n"
      elsif subtype.nil?
        result += append_repeaters_declarations el
      end
    end
    result
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
    result += "    #{klass[:repeater][:type]} #{klass[:repeater][:name]};\n" if klass[:behavior] == :repeater
    result += append_attributes if klass[:el] == @body
    result += append_refs klass[:el]
    result += append_repeaters_declarations klass[:el]
    result += "  private:\n"
    result += append_inline_refs klass[:el]
    result += "  public:\n"
    if klass[:parent].nil?
      result += "    #{klass[:typename]}();\n"
    elsif klass[:behavior] == :repeater
      result += "    #{klass[:typename]}(#{klass[:parent][:typename]}*, #{klass[:repeater][:type]});\n"
    else
      result += "    #{klass[:typename]}(#{klass[:parent][:typename]}*);\n"
    end
    result += "    void bind_attributes();\n"
    result += "    void trigger_binding_updates();\n"
    result += append_code if klass[:el] == @body
    result += "  };\n"
    result
  end

  def get_repeaters_for klass
    results = []
    @classes.each do |other_klass|
      results << other_klass if other_klass[:parent] == klass && other_klass[:behavior] == :repeater
    end
    results
  end

  def append_repeater_initializers klass
    result = ""
    repeaters = get_repeaters_for klass
    repeaters.each do |repeater|
      anchor_name = @named_elements[repeater[:repeater][:anchor][:el].path]
      anchor_mode = case repeater[:repeater][:anchor][:mode]
                    when :append   then "AppendAnchor"
                    when :prepend  then "PrependAnchor"
                    when :children then "ChildrenAnchor"
                    end
      result += "  #{repeater[:repeater][:ref]}.set_anchor(#{anchor_name}, Crails::Front::#{anchor_mode});\n"
    end
    result
  end

  def generate_class_source klass
    result = ""
    initializers = []
    if klass[:extends] == "Crails::Front::Element" && klass[:el] != @body
      initializers << "Crails::Front::Element(\"#{klass[:el].name}\")"
    end
    if klass[:parent].nil?
      result += "HtmlTemplate::#{klass[:typename]}::#{klass[:typename]}()"
    elsif klass[:behavior] == :repeater
      initializers << "parent(__p)" << "#{klass[:repeater][:name]}(__i)"
      result += "HtmlTemplate::#{klass[:typename]}::#{klass[:typename]}(#{klass[:parent][:typename]}* __p, #{klass[:repeater][:type]} __i)"
    else
      initializers << "parent(__p)"
      result += "HtmlTemplate::#{klass[:typename]}::#{klass[:typename]}(#{klass[:parent][:typename]}* __p)"
    end
    result += ": " + initializers.join(", ") if initializers.length > 0
    result += "\n"
    result += "{\n"
    result += append_constructor klass[:el]
    result += append_repeater_initializers klass
    result += "}\n\n"
    result += "void HtmlTemplate::#{klass[:typename]}::bind_attributes()\n"
    result += "{\n"
    result += "  bound_attributes.enable(signaler);\n"
    result += append_bind_attributes klass[:el]
    result += append_bind_repeaters klass
    result += "}\n\n"
    result += "void HtmlTemplate::#{klass[:typename]}::trigger_binding_updates()\n"
    result += "{\n"
    result += "  bound_attributes.update();\n"
    result += append_bind_updates klass[:el]
    result += append_bind_repeaters_update klass
    result += "}\n"
    result
  end

  def append_bind_repeaters_update klass
    result = ""
    get_repeaters_for(klass).each do |repeater|
      result += "  #{repeater[:repeater][:ref]}.trigger_binding_updates();\n"
    end
    result
  end

  def append_bind_repeaters klass
    result = ""
    get_repeaters_for(klass).each do |repeater|
      result += "  #{repeater[:repeater][:ref]}.bind_attributes();\n"
    end
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
      next unless get_subtype_for(el).nil?
      unless el["ref"].nil?
        type = if @element_types.keys.include? el.name
                 @element_types[el.name]
               else
                 "Crails::Front::Element"
               end
        result += "    #{type} #{el["ref"]};\n"
        @named_elements[el.path] = el["ref"]
        @ref_types[el["ref"]] = type
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

  def add_inline_ref el
    varname   = "element_#{@inline_ref_count}"
    type      = @element_types[el.name] || "Crails::Front::Element"
    el["ref"] = varname
    @ref_types[varname] = type
    @named_elements[el.path] = varname
    @inline_ref_count += 1
    return "    #{type} #{varname};\n"
  end

  def append_inline_refs parent = nil
    parent = @body if parent.nil?
    result = ""
    parent.children.each do |el|
      subtype = get_subtype_for(el)
      if subtype.nil?
        result += add_inline_ref el if (@named_elements[el.path].nil? && el["ref"].nil? &&
                                        (@element_types.keys.include?(el.name) || has_trigger_attributes?(el)))
        result += append_inline_refs el
      elsif subtype[:behavior] == :repeater
        anchor = subtype[:repeater][:anchor][:el]
        result += add_inline_ref anchor if @named_elements[anchor.path].nil?
      end
    end
    result
  end

  def make_attr_from_el el
    hard_attributes  = []
    el.attributes.each do |key, value|
      if key != "ref" && !(key.end_with? ".bind") && !(key.end_with? ".trigger") && !(key.end_with? ".for")
        hard_attributes << "{\"#{key}\",\"#{value}\"}"
      end
    end
    hard_attributes
  end

  def append_constructor root
    result = ""
    result += "  std::map<std::string, Crails::Front::Element> element_cache;\n\n"
    result += append_constructor_refs root
    result += append_bound_attributes_for root unless root == @body
    result += append_bound_attributes root
    unless root == @body
      hard_attributes = make_attr_from_el root
      result += "  attr({#{hard_attributes.join ','}});\n" if hard_attributes.length > 0
    end
    result += "  inner({#{append_children root, 4}\n  });\n"
    result += "  bind_attributes();\n"
    result
  end

  def append_constructor_refs parent
    result = ""
    parent.children.each do |el|
      next unless get_subtype_for(el).nil?
      if (!el["ref"].nil?) && (@ref_types[el["ref"]].nil? || (@ref_types[el["ref"]] == "Crails::Front::Element"))
        result += "  #{el["ref"]} = Crails::Front::Element(\"#{el.name}\");\n"
      end
      result += append_constructor_refs el
    end
    result
  end

  def extract_bind_mode_from value
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
    [value, bind_mode]
  end

  def append_bound_attributes_for el
    result = ""
    el_is_cached = el == @root_element
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

        attribute_name   = key.split(".").first
        is_cpp           = attribute_name.start_with?("cpp::")
        attribute_name   = attribute_name[5..-1] if is_cpp
        value, bind_mode = extract_bind_mode_from value

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
    result
  end

  def append_bound_attributes parent
    result = ""
    parent.children.each do |el|
      subtype = get_subtype_for(el)
      if subtype.nil?
        result += append_bound_attributes_for el
        el.children.each do |child|
          result += append_bound_attributes child
        end
      elsif subtype[:behavior] == :repeater
        refresh_code = "#{subtype[:repeater][:ref]}.refresh(this, #{subtype[:repeater][:list]});"
        initializer  = "Crails::Front::Bindable([this]() { #{refresh_code} })"
        result += "  bound_attributes.push_back(#{initializer}#{subtype[:repeater][:bind_mode]});\n"
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
        is_ref = !el["ref"].nil?
        hard_attributes  = make_attr_from_el el

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
