require 'crails-cheerp-html/parser-class'

module CrailsCheerpHtml
  class SlotBase < CrailsCheerpHtml::Class
    def find_remote_reference_holder
      p = @parent
      until p.is_root?
        return nil if p.blocks_remote_references?
        p = p.parent
      end
      p
    end

    def create_reference el, mode
      ref_root = nil 
      if (mode == :implicit) || (ref_root = find_remote_reference_holder).nil?
        super el, mode
      else
        ref_root.refs << Reference.new(el, ref_root, mode)
        @refs << RemoteReference.new(el, self, mode)
      end
    end
  end
  
  class SlotPlugin < SlotBase
    attr_reader :slot_name, :on_element

    def initialize el, parent, on_element
      super el, parent
      @typename   = "#{context.classes.first.typename}SlotPlugin_#{context.slot_count}"
      @slot_name  = el["slot"].to_s
      @on_element = on_element
      context.slot_count += 1
    end
  end
  
  class Slot < SlotBase
    class Probe
      class << self
        def matches? el
          el.name == "slot"
        end

        def evaluate el, parent
          Slot.new el, parent
        end
      end
    end

    attr_reader :slot_name, :slot_ref, :anchor

    def initialize el, parent
      super el, parent
      @typename   = "#{context.classes.first.typename}Slot_#{context.slot_count}"

      @slot_name = el["name"].to_s
      @slot_ref  = "slot_#{@slot_name}"
      @anchor    = find_anchorable_anchor(el)

      unless is_valid_cpp_variable_name?(@slot_name)
        raise ParseError.new(el, "slot name `#{@slot_name}` is not a valid C++ variable name")
      end

      context.slot_count += 1
    end
    
    def is_anchorable?
      true
    end
  end
end
