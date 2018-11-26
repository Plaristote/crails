NAMESPACE = "ModelData"

class Model
  class << self
    attr_accessor :current_file

    def add name, definition = [], &block
      @list ||= []
      @list << { name: name, filename: current_file, block: block,
                 classname: definition.first, header: definition.last }
    end

    def list
      @list || []
    end
  end
end

class Includes
  class << self
    attr_accessor :list
    attr_accessor :headers
  end
end

def add_include path, include_in_header = false
  if include_in_header
    Includes.headers ||= {}
    Includes.headers[Model.current_file] ||= []
    Includes.headers[Model.current_file] << path
  else
    Includes.list ||= {}
    Includes.list[Model.current_file] ||= []
    Includes.list[Model.current_file] << path
  end
end

def collect_includes_for filename, is_header = false
  base = if is_header then Includes.headers else Includes.list end
  return [] if base.nil?
  pre  = "#include"
  pre  = "# include" if is_header
  base = base[filename]
  ((base || []).collect {|a| "#{pre} \"#{a}\""}).uniq
end

def load_all_models input_dir
  Dir["#{input_dir}/*.rb"].each do |file|
    Model.current_file = file
    load file
  end
  Model.list
end
