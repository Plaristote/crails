class String
  def underscore
    self.gsub(/::/, '/').
    gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2').
    gsub(/([a-z\d])([A-Z])/,'\1_\2').
    tr("-", "_").
    downcase
  end

  def camelcase
    return self if self !~ /_/ && self =~ /[A-Z]+.*/
    split('_').map{|e| e.capitalize}.join
  end
end

class GeneratorBase
  def self.is_file_based? ; true ; end

  class << self
    attr_accessor :odb_connection

    def prepare inputs_dir, output_dir
      @output_dir = output_dir
      Includes.list ||= {}
      inputs_dir.each do |input_dir|
        load_all_models input_dir
      end
    end

    def use generator_class
      if generator_class.is_file_based?
        _use_by_files generator_class
      else
        _use_by_models generator_class
      end
    end

    def _use_by_models generator_class
      Model.list.each do |model|
        generator = generator_class.new
        next unless generator.should_generate_for(model)
        data = { bodies: [generator.generate_for(model)] }
        data[:headers] = [generator.get_headers] if generator.methods.include? :get_headers
        source = generator_class.make_file model[:filename], data
        filepath = "#{@output_dir}/" + (File.dirname model[:filename])
        filename = model[:name].underscore + generator_class.extension
        `mkdir -p #{filepath}`
        File.open "#{filepath}/#{filename}", 'w' do |f|
          f.write source
        end
      end
    end

    def _use_by_files generator_class
      files = {}
      Model.list.each do |model|
        generator = generator_class.new
        next unless generator.should_generate_for(model)
        files[model[:filename]] ||= {}
        filedata = files[model[:filename]]
        filedata[:headers] ||= []
        filedata[:bodies]  ||= []
        filedata[:bodies]  << generator.generate_for(model)
        if generator.methods.include? :get_headers
          filedata[:headers] << generator.get_headers
        end
      end
      files.each do |key,value|
        source = generator_class.make_file key, value
        `mkdir -p #{@output_dir}/#{File.dirname key}`
        File.open "#{@output_dir}/#{key[0...-3]}#{generator_class.extension}", 'w' do |f|
          f.write source
        end
      end
    end
  end

  def should_generate_for object
    true
  end

  def reset
    @tab_size = 2
    @indent = 0
    @src = ""
  end

  def _append str, linereturn = true
    @src += " " * (@indent * @tab_size)
    @src += str
    @src += "\n" if linereturn
  end

  def ptr_type type
    "std::shared_ptr<#{type}>"
  end

  def get_value value
    if value.class == Symbol
      value.to_s
    else
      value.inspect
    end
  end

  def get_type type
    type = "::#{type}" unless type.start_with? "::"
    type += " " if type[type.size - 1] == ">"
    type
  end

  def get_singular_name name
    if not (name =~ /ies$/).nil?
      name[0...name.size-3] + "y"
    elsif (name =~ /(s|x)es$/).nil?
      name[0...name.size-1]
    else
      name[0...name.size-2]
    end
  end

  def get_pluralized_name name
    if not (name =~ /holiday/i).nil?
      name + "s"
    elsif not (name =~ /y$/).nil?
      name[0...name.size-1] + "ies"
    elsif not (name =~ /(s|x)$/).nil?
      name + "es"
    else
      name + "s"
    end
  end

  def get_classname object
    "#{NAMESPACE}::#{object[:name]}"
  end

  def visibility name ;; end
  def resource_name name ;; end
  def order_by name ;; end
  def property type, name, options = {} ;; end
  def has_one type, name, options = {} ;; end
  def has_many type, name, options = {} ;; end
end
