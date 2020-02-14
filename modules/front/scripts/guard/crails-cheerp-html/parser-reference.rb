module CrailsCheerpHtml
  class CppReference
    attr_reader :el, :type, :name, :default_value
    
    def initialize el, type, name, default_value = nil
      @el            = el
      @type          = type
      @name          = name
      @default_value = default_value
      unless is_valid_cpp_variable_name?(@name)
        raise ParseError.new(el, "attribute name `#{@name}` is not a valid C++ variable name")
      end
    end
    
    def is_explicit? ; true ; end
    def is_implicit ; false ; end
    def has_default_value? ; !default_value.nil? ; end
  end

  class Reference
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
    def has_default_value?; false ; end
  end
  
  class RemoteReference < Reference    
    def type
      "#{@type}&"
    end
  end
end

