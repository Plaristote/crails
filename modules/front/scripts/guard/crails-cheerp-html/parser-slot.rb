require 'crails-cheerp-html/parser-class'

module CrailsCheerpHtml
  class SlotPlugin < CrailsCheerpHtml::Class
    attr_reader :slot_name, :on_element

    def initialize el, parent, on_element
      super el, parent
      @typename   = "#{context.classes.first.typename}SlotPlugin_#{context.slot_count}"
      @slot_name  = el["slot"].to_s
      @on_element = on_element
      context.slot_count += 1
    end
  end
  
  class Slot < CrailsCheerpHtml::Class
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

      if parent.find_reference_for(@anchor[:el]).nil?
        parent.refs << (Reference.new @anchor[:el], parent)
      end

      context.slot_count += 1
    end
  end
end
