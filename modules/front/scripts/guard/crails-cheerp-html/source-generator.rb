module CrailsCheerpHtml
  class SourceGenerator
    attr_reader :object, :source
    
    def initialize object
      @object = object
    end
    
    def generate
      @source  = generate_constructor_header
      @source += generate_constructor_body
      @source += generate_constructor_footer + "\n"
      @source += generate_method_bind_attributes + "\n"
      @source += generate_method_trigger_binding_updates   
      @source
    end

    def tag_name
      if @object.parent.nil? then @object.typename.dasherize else @object.el.name end
    end
    
    def root_ptr
      if object.parent.nil? then "this" else "root" end
    end
    
    def get_root_from_parent_code parent_symbol
      result = parent_symbol
      current_parent = object.parent
      until current_parent.parent.nil?
        result += "->parent"
        current_parent = current_parent.parent
      end
      result
    end

    def make_parent_to_root_initializer parent_symbol
      "root(#{get_root_from_parent_code(parent_symbol)})"
    end

    def generate_constructor_header
      initializers = []
      initializers << "#{object.context.template_base_type}(\"#{tag_name}\")" if object.superclass == object.context.template_base_type
      # Constructor
      result  = "HtmlTemplate::#{object.typename}::#{object.typename}("
      unless object.is_root?
        initializers << "parent(__p)"
        initializers << "signaler(__p->signaler)"
        result += "#{object.parent.typename}* __p"
        initializers << make_parent_to_root_initializer("__p")
        object.slots.each do |slot|
          initializers << "#{slot.slot_ref}(#{get_root_from_parent_code("__p")}->#{slot.slot_ref})"
        end
      else
        initializers << "root(this)"
      end
      if object.class == Repeater
        initializers << "#{object.value_name}(__i)"
        result += ", #{object.value_type} __i"
      end
      result += ")"
      result += " : " + initializers.join(", ") if initializers.count > 0
      result += "\n"
      result += "{\n"
      result
    end
    
    def generate_constructor_body
      result  = generate_element_initializers
      result += generate_binding_initializers
      result += generate_dom_constructor
      result += generate_anchors_initializers
      result
    end

    def generate_constructor_footer
      result = ""
      result += "  bind_attributes();\n"    
      result += "}\n"
      result
    end
    
    def generate_element_initializers
      result = ""
      object.refs.each do |ref|
        if ref.type == "Crails::Front::Element"
          result += "  #{ref.name} = Crails::Front::Element(\"#{ref.el.name}\");\n"
        end
      end
      result
    end
    
    def generate_binding_initializers
      result = ""
      object.bindings.each do |binding|
        ref = object.find_reference_for binding.el
        initializer = if binding.binds_to_cpp_property?
          "Crails::Front::Bindable([this]() { #{ref.name}.set_#{binding.attribute_name}(#{binding.code}); })"
        elsif binding.attribute_name == "text"
          "Crails::Front::Bindable([this]() { #{ref.name}.text(#{binding.code}); })"
        elsif binding.attribute_name == "innerhtml"
          "Crails::Front::Bindable([this]() { #{ref.name}.html(#{binding.code}); })"
        else
          "Crails::Front::Bindable(#{ref.name}, \"#{binding.attribute_name}\", [this]() -> std::string { return boost::lexical_cast<std::string>(#{binding.code}); })"
        end
        result += "  bound_attributes.push_back(#{initializer}#{binding.bind_mode});\n"
      end

      object.repeaters.each do |repeater|
        refresh_code = "#{repeater.repeater_name}.refresh(this, #{repeater.list_name});"
        initializer  = "Crails::Front::Bindable([this]() { #{refresh_code} })"
        result += "  bound_attributes.push_back(#{initializer}#{repeater.bind_mode});\n"
      end
        
      object.event_listeners.each do |event_listener|
        ref = object.find_reference_for event_listener.el
        result += "  #{ref.name}.events->on(\"#{event_listener.attribute_name}\", [this](client::Event* _event) { #{event_listener.code}; });\n"
      end
      result
    end
    
    def generate_dom_constructor
      result = ""
      source_el = if object.is_root? then object.el.first else object.el end
      html_attributes = make_attr_from_el source_el
      result += "  attr({#{html_attributes.join ','}});\n" if html_attributes.length > 0
      dom_code = dom_generator object.el, 4
      result += "  inner({#{dom_code}\n  });\n" unless dom_code.empty?
      result
    end

    def dom_generator el, indent
      result = ""
      count = 0
      el.children.each do |el|
        next unless el["_cheerp_class"].nil?
        if el.name != "text" && el.name != "comment"
          reference = object.find_reference_for(el)
          html_attributes = make_attr_from_el el

          result += "," if count > 0
          result += "\n" + (" " * indent)

          result += if reference.nil? then "Crails::Front::Element(\"#{el.name}\")" else reference.name end
          result += ".attr({#{html_attributes.join ','}})" if html_attributes.count > 0
          children_result = dom_generator el, indent + 2
          result += ".inner({#{children_result}\n#{" " * indent}})" if children_result.length > 0

          count += 1
        elsif el.name == "text" && !(el.text.strip.empty?)
          escaped_text = el.text.gsub /"/, "\\\""
          escaped_text.gsub! "\n", '\n'
          result += "," if count > 0
          result += "\n" + (" " * indent)
          result += "Crails::Front::Element(\"span\").text(\"#{escaped_text}\")"
          count += 1
        end
      end
      result
    end
    
    def generate_anchors_initializers
      result = ""
      object.repeaters.each do |repeater|
        anchor_ref = object.find_reference_for(repeater.anchor[:el])
        result += "  #{repeater.repeater_name}.set_anchor(#{anchor_ref.name}, #{anchor_symbol_to_enum repeater.anchor[:mode]});\n"
      end

      object.slots.each do |slot|
        anchor_ref = object.find_reference_for(slot.anchor[:el])
        result += "  #{slot.slot_ref}.set_anchor(#{anchor_ref.name}, #{anchor_symbol_to_enum slot.anchor[:mode]});\n"
        result += "  #{slot.slot_ref}.set_element(std::make_shared<#{slot.typename}>(this));\n"
      end
      
      object.slot_plugins.each do |slot_plugin|
        ref = object.find_reference_for(slot_plugin.on_element)
        result += "  #{ref.name}.slot_#{slot_plugin.slot_name}.set_element(std::make_shared<#{slot_plugin.typename}>(this));\n"
      end
      result
    end
    
    def generate_method_bind_attributes
      result = ""
      result += "void HtmlTemplate::#{object.typename}::bind_attributes()\n"
      result += "{\n"
      result += "  bound_attributes.enable(signaler);\n"
      object.refs.each do |ref|
        if ref.el && object.context.has_cpp_type?(ref.el)
          result += "  #{ref.name}.bind_attributes();\n"
        end
      end
      object.repeaters.each do |repeater|
        result += "  #{repeater.repeater_name}.bind_attributes();\n"
      end
      result += "}\n"
      result
    end
    
    def generate_method_trigger_binding_updates
      result = ""
      result += "void HtmlTemplate::#{object.typename}::trigger_binding_updates()\n"
      result += "{\n"
      result += "  bound_attributes.update();\n"
      object.refs.each do |ref|
        if ref.el && object.context.has_cpp_type?(ref.el)
          result += "  #{ref.name}.trigger_binding_updates();\n"
        end
      end
      object.repeaters.each do |repeater|
        result += "  #{repeater.repeater_name}.trigger_binding_updates();\n"
      end
      result += "}\n"
      result
    end
  end
end
