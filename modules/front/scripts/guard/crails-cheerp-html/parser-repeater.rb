require 'crails-cheerp-html/parser-class'

module CrailsCheerpHtml
  class Repeater < CrailsCheerpHtml::Class
    class Probe
      class << self
        def matches? el
          !(el["repeat.for"].nil?)
        end

        def evaluate el, parent
          Repeater.new el, parent
        end
      end
    end

    attr_reader :repeater_name, :value_type, :value_name, :list_type, :list_name, :bind_mode, :anchor

    def initialize el, parent
      super el, parent
      value, bind_mode = extract_bind_mode_from el["repeat.for"].to_s
      parts = value.to_s.match /^\s*(\[([^\]]+)\])?\s*([^\s]+)\s+of\s+\[([^\]]+)\]\s*(.*)$/
      @typename   = "#{context.classes.first.typename}Repeatable_#{context.repeater_count}"
      @superclass = context.find_cpp_type(el.name)

      @repeater_name = "repeater_#{context.repeater_count}"
      @value_type    = parts[2] || "#{parts[4]}::value_type"
      @value_name    = parts[3]
      @list_type     = parts[4]
      @list_name     = parts[5]
      @bind_mode     = bind_mode
      @anchor        = find_anchorable_anchor(el)

      if parent.find_reference_for(@anchor[:el]).nil?
        parent.refs << (Reference.new @anchor[:el], parent)
      end

      context.repeater_count += 1
    end

    def constructor_decl
      "#{@typename}(#{parent.typename}*, #{@value_type});"
    end
  end
end
