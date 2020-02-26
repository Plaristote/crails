module CrailsCheerpHtml
  class ReferenceBase
    def has_default_value?
      false
    end
    
    def has_initializer?
      false
    end
    
    def has_getter?
      false
    end
    
    def has_setter?
      false
    end
  end

  class CppReference < ReferenceBase
    attr_reader :el, :type, :name, :default_value
    attr_accessor :initializer, :setter_enabled
    
    def initialize el, type, name, default_value = nil
      @el             = el
      @type           = type
      @name           = name
      @default_value  = default_value
      @setter_enabled = true
      if type.nil? || name.nil?
        raise ParseError.new(el, "incomplete attribute definition")
      end
      unless is_valid_cpp_variable_name?(@name)
        raise ParseError.new(el, "attribute name `#{@name}` is not a valid C++ variable name")
      end
    end
    
    def is_explicit? ; true ; end
    def is_implicit ; false ; end
    def has_default_value? ; !default_value.nil? ; end
    def has_initializer? ; !@initializer.nil? end      
    def has_getter? ; true ; end
    def has_setter? ; @setter_enabled ; end
      
    def initializer root_getter
      @initializer
    end
  end

  class Reference < ReferenceBase
    attr_reader :el, :type, :name

    def initialize el, parent, mode = nil
      el["_cheerp_ref"] = self
      @el     = el
      @parent = parent
      @type   = @parent.context.find_cpp_type el.name
      set_mode mode
    end

    def set_mode mode
      if (mode.nil? && !@el["ref"].nil?) || (mode == :explicit)
        @mode = :explicit
        @name = @el["ref"].to_s
        unless is_valid_cpp_variable_name?(@name)
          raise ParseError.new(el, "ref `#{@name}` is not a valid c++ variable name")
        end
      else
        @mode = :implicit
        @name = "element_#{@parent.context.generate_new_ref_name}"
      end
    end

    def is_explicit? ; @mode == :explicit ; end
    def is_implicit? ; @mode == :implicit ; end
  end
  
  class RemoteReference < Reference    
    def type
      "#{@type}&"
    end

    def has_initializer?
      true
    end

    def initializer root_getter
      "#{root_getter}->#{name}"
    end
  end

  class ThisReference
    def name
      "(*this)"
    end
  end
end

