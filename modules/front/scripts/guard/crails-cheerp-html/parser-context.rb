module CrailsCheerpHtml
  class Context
    class << self
      attr_accessor :slot_count, :repeater_count, :element_types
      attr_reader :classes

      def reset
        @slot_count = @repeater_count = 0
        @implicit_ref_count = 0
        @classes = []
        @element_types = {}
      end
      
      def template_base_type
        "Crails::Front::Element"
      end

      def has_cpp_type? el
        @element_types.keys.include? el.name
      end

      def find_cpp_type name
        if @element_types.keys.include? name
          @element_types[name]
        else
          "Crails::Front::Element"
        end
      end

      def generate_new_ref_name
        @implicit_ref_count += 1
        "implicit_reference_#{@implicit_ref_count}"
      end
      
      def load_global_element_types data
        data.each do |element_data|
          tag_name = element_data["tagName"] || element_data["require"].dasherize
          @element_types[tag_name] = element_data["require"]
        end
      end
    end

    def context
      Context
    end
  end
end
