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
      raise ParseError.new(el, "invalid repeater definition: `#{value}`") if parts.nil?
      @typename   = "#{context.classes.first.typename}Repeatable_#{context.repeater_count}"
      @superclass = context.find_cpp_type(el.name, fallback: context.template_base_subtype)

      @repeater_name = "repeater_#{context.repeater_count}"
      @value_type    = parts[2] || "#{parts[4]}::value_type"
      @value_name    = parts[3]
      @list_type     = parts[4]
      @list_name     = parts[5]
      @bind_mode     = bind_mode
      
      unless is_valid_cpp_variable_name?(@value_name)
        raise ParseError.new(el, "invalid variable name `#{@value_name}`") 
      end

      context.repeater_count += 1
    end

    def constructor_decl
      "#{@typename}(#{parent.typename}*, #{@value_type});"
    end
    
    def is_anchorable?
      true
    end
    
    def blocks_remote_references?
      true
    end
    
    def probe
      super
      probe_bindings_for el
    end
  end
end
