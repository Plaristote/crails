module CrailsCheerpHtml
  class HeaderGenerator
    attr_reader :object, :source

    def initialize object
      @object = object
    end

    def signaler_definition
      # Signaler def. Must be a reference if the object isn't the root object
      signaler_type  = "Crails::Signal<std::string>"
      signaler_type += "&" unless object.parent.nil?
      "#{signaler_type} signaler;"
    end
    
    def generate_getters_setters
      result = ""
      object.refs.each do |ref|
        if ref.class == CppReference
          result += "      virtual void set_#{ref.name}(const #{ref.type}& __v) { #{ref.name} = __v; }\n"
          result += "      const #{ref.type}& get_#{ref.name}() const { return #{ref.name}; }\n"
        end
      end
      result
    end

    def generate_properties
      protected_properties_def = ""
      public_properties_def    = ""
      private_properties_def   = ""
      if object.is_root?
        private_properties_def += "      #{object.typename}* root;\n"
      else
        public_properties_def  += "      #{object.parent.typename}* parent;\n"
        private_properties_def += "      #{object.root.typename}* root;\n"
      end
      object.refs.each do |ref|
        suffix = " = #{ref.default_value}" if ref.has_default_value?
        if ref.is_explicit?
          protected_properties_def += "      #{ref.type} #{ref.name}#{suffix};\n"
        elsif ref.is_implicit?
          private_properties_def   += "      #{ref.type} #{ref.name}#{suffix};\n"
        end
      end
      object.repeaters.each do |repeater|
        protected_properties_def += "      Crails::Front::Repeater<#{repeater.list_type}, #{repeater.typename}> #{repeater.repeater_name};\n" 
      end
      ## BEGIN SLOTS
      # Slots emanate from the root object.
      if object.is_root?
        all_slots = object.slots
        object.recursively_collect_children.each do |child|
          all_slots += child.slots
        end
        all_slots.each do |slot|
          public_properties_def += "      Crails::Front::SlotElement #{slot.slot_ref};\n"
        end
      else
        object.slots.each do |slot|
          public_properties_def += "      Crails::Front::SlotElement& #{slot.slot_ref};\n"
        end
      end
      ## END SLOTS
      [public_properties_def, protected_properties_def, private_properties_def]
    end

    def generate
      public_properties_def, protected_properties_def, private_properties_def = generate_properties

<<HEADER
    class #{object.typename} : public #{object.superclass}
    {
      Crails::Front::BoundAttributes bound_attributes;
    protected:
#{protected_properties_def}
    private:
#{private_properties_def}
    public:
      #{signaler_definition}
#{public_properties_def}

      #{object.constructor_decl}

      void bind_attributes();
      void trigger_binding_updates();
      
#{generate_getters_setters}

      #{object.inline_code}
    };
HEADER
    end
  end
end
