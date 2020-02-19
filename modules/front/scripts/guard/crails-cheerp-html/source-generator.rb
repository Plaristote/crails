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
      if @object.parent.nil?
        if @object.el.first["tag-name"].nil?
          @object.typename.dasherize
        else
          @object.el.first["tag-name"]
        end
      else
        @object.el.name
      end
    end

    def root_ptr
      if object.parent.nil? then "this" else "root" end
    end
    
    def get_root_from_parent_code parent_symbol
      if object.is_root?
        "this"
      else
        result = parent_symbol
        current_parent = object.parent
        until current_parent.parent.nil?
          result += "->parent"
          current_parent = current_parent.parent
        end
        result
      end
    end

    def make_parent_to_root_initializer parent_symbol
      "root(#{get_root_from_parent_code(parent_symbol)})"
    end

    def generate_constructor_header
      initializers = []      
      parent_symbol = "__p"
      root_getter   = get_root_from_parent_code parent_symbol
      
      case object.superclass
      when object.context.template_base_type then
        initializers << "#{object.context.template_base_type}(\"#{tag_name}\")"
#      when object.context.template_base_subtype then
#        initializers << "#{object.context.template_base_subtype}(\"#{tag_name}\", #{parent_symbol}->signaler)"
      end

      # Constructor
      result  = "HtmlTemplate::#{object.typename}::#{object.typename}("
      unless object.is_root?
        initializers << "parent(#{parent_symbol})"
        initializers << "signaler(#{parent_symbol}->signaler)" unless object.implements_ibindable_view?
        result += "#{object.parent.typename}* #{parent_symbol}"
        initializers << make_parent_to_root_initializer(parent_symbol)
        object.slots.each do |slot|
          initializers << "#{slot.slot_ref}(#{root_getter}->#{slot.slot_ref})"
        end
      else
        initializers << "root(this)"
      end
      (object.refs.select{|r| r.has_initializer?}).each do |ref|
        initializers << "#{ref.name}(#{ref.initializer root_getter})"
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
        unless event_listener.is_cpp
          result += "  #{ref.name}.events->on(\"#{event_listener.attribute_name}\", [this](client::Event* _event) { #{event_listener.code}; });\n"
        else
          result += "  #{ref.name}.signaler.connect([this](std::string signal_name)\n"
          result += "  {\n"
          result += "    if (signal_name == \"#{event_listener.attribute_name}\")\n"
          result += "    {\n"
          result += "      #{event_listener.code};\n"
          result += "    }\n"
          result += "  });\n"
        end
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
        sub_object = object.find_class_for el
        if !sub_object.nil? && sub_object.is_anchorable?
          result += "," if count > 0
          result += "\n" + (" " * indent)
          result += sub_object.anchor_name
          count += 1
        elsif !(el["_cheerp_class"].nil?)
          next
        elsif el.name != "text" && el.name != "comment"
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
        result += "  #{repeater.repeater_name}.set_anchor(#{repeater.anchor_name}, #{anchor_symbol_to_enum :append});\n"
      end

      object.slots.each do |slot|
        result += "  #{slot.slot_ref}.set_anchor(#{slot.anchor_name}, #{anchor_symbol_to_enum :append});\n"
        result += "  #{slot.slot_ref}.set_element(std::make_shared<#{slot.typename}>(this));\n"
      end

      object.slot_plugins.each do |slot_plugin|
        ref = object.find_reference_for(slot_plugin.on_element)
        initializer = if slot_plugin.has_ref?
          "root->#{slot_plugin.el["ref"]}"
        else
          "std::make_shared<#{slot_plugin.typename}>(#{slot_plugin.constructor_params})"
        end
        result += "  #{ref.name}.slot_#{slot_plugin.slot_name}.set_element(#{initializer});\n"
      end
      result
    end
    
    def inherits_binding_methods?
      !([object.context.template_base_type, object.context.template_base_subtype].include? object.superclass)
    end
    
    def generate_method_bind_attributes
      result = ""
      result += "void HtmlTemplate::#{object.typename}::bind_attributes()\n"
      result += "{\n"
      if inherits_binding_methods?
        result += "  #{object.superclass}::bind_attributes();\n"
      else
        result += "  bound_attributes.enable(signaler);\n"
      end
      object.refs.each do |ref|
        if ref.el && object.context.has_cpp_type?(ref.el)
          result += "  #{ref.name}.bind_attributes();\n"
          result += "  signaler.connect([this](std::string _event) { #{ref.name}.signaler.trigger(_event); });\n"
        end
      end
      object.repeaters.each do |repeater|
        result += "  #{repeater.repeater_name}.bind_attributes();\n"
      end
      object.slots.each do |slot|
        result += "  #{slot.slot_ref}.get_element()->bind_attributes();\n"
      end
      result += "}\n"
      result
    end
    
    def generate_method_trigger_binding_updates
      result = ""
      result += "void HtmlTemplate::#{object.typename}::trigger_binding_updates()\n"
      result += "{\n"
      if inherits_binding_methods?
        result += "  #{object.superclass}::trigger_binding_updates();\n"
      else
        result += "  bound_attributes.update();\n"
      end
      object.refs.each do |ref|
        if ref.el && object.context.has_cpp_type?(ref.el)
          result += "  #{ref.name}.trigger_binding_updates();\n"
        end
      end
      object.repeaters.each do |repeater|
        result += "  #{repeater.repeater_name}.trigger_binding_updates();\n"
      end
      object.slots.each do |slot|
        result += "  #{slot.slot_ref}.get_element()->trigger_binding_updates();\n"
      end
      result += "}\n"
      result
    end
  end
end
