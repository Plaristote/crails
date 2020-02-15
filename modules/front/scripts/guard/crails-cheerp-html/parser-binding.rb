require 'crails-cheerp-html/utils'

module CrailsCheerpHtml  
  class EventListener
    attr_reader :el, :attribute_name, :code, :is_cpp

    def initialize el, parent, key, value
      @el             = el
      @parent         = parent
      @is_cpp         = key.start_with?("cpp::")
      @attribute_name = key.delete_suffix(".trigger")
      @attribute_name = @attribute_name[5..-1] if is_cpp
      @code           = value

      if @parent.find_reference_for(el).nil?
        @parent.refs << (Reference.new(el, @parent))
      end
    end
  end

  class Binding
    attr_reader :el, :attribute_name, :is_cpp, :bind_mode, :code
    
    def initialize el, parent, key, value
      @el     = el
      @parent = parent

      @attribute_name   = key.delete_suffix(".bind")
      @is_cpp           = attribute_name.start_with?("cpp::")
      @attribute_name   = attribute_name[5..-1] if is_cpp
      @code, @bind_mode = extract_bind_mode_from value

      if @parent.find_reference_for(el).nil?
        @parent.refs << (Reference.new(el, @parent))
      end
    end
    
    def binds_to_cpp_property?
      @is_cpp
    end
  end
end
