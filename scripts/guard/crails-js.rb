HAS_COFFEESCRIPT = Gem::Specification::find_all_by_name('coffee-script').any?
HAS_TYPESCRIPT   = Gem::Specification::find_all_by_name('typescript-node').any?

require 'guard/crails-base'
require 'guard/crails-notifier'
require 'fileutils'
require 'pathname'
require 'uglifier'
require 'coffee-script'   if HAS_COFFEESCRIPT
require 'typescript-node' if HAS_TYPESCRIPT

module ::Guard
  class JavascriptSource
    attr_reader :source, :compiled

    def initialize crailsjs, filepath
      @crailsjs  = crailsjs
      @filepath  = filepath
      @filename  = filepath.split('/').last
      @directory = Pathname.new(filepath + "/..").cleanpath.to_s
    end

    def dependencies
      array = []
      @crailsjs.linker @source, @directory do |type, data|
        array << data if type == :include
      end
      array
    end

    def current_dir
      @filepath.split('/')[0...-1].join('/')
    end

    def full_path
      (@crailsjs.options[:input] + '/' + @filepath).gsub(/\/+/, '/')
    end

    def compile
      @source           = File.read full_path, encoding: @crailsjs.encoding
      @last_compilation = File.mtime(full_path)
      puts "[crailsjs] Compiling #{full_path}..."
      if HAS_COFFEESCRIPT && @filename.match(/\.coffee$/) != nil
        @source = @crailsjs.compile_coffeescript @source, @filepath
      elsif HAS_TYPESCRIPT && @filename.match(/\.ts$/) != nil
        @source = @crailsjs.compile_typescript @source, @filepath, current_dir
      end
      @compiled = @crailsjs.resolve_linking @source, current_dir
    end

    def should_compile?
      if @source.nil? || @last_compilation < File.mtime(full_path)
        return true
      end
      dependencies.each do |dependency|
        javascript_cache = @crailsjs.file_cache[dependency]
        return true if javascript_cache.nil? || javascript_cache.should_compile?
      end
      return false
    end
  end

  class CrailsJs < CrailsPlugin
    attr_accessor :file_cache, :encoding

    def initialize options = {}
      options[:watchers] << ::Guard::Watcher.new(%r{^#{options[:input]}/(.+\.(js|coffee|ts))$})
      @force_uglify     = options[:enable_uglify]
      @uglifier_options = options[:uglifier] || { enclose: true }
      @encoding         = options[:encoding] || 'UTF-8'
      super options
    end

    def js_header
      value = if developer_mode? then 'development' else 'production' end
      "var crailsEnv = '#{value}';"    
    end

    def run_all
      begin
        @file_cache ||= {}
        starts_at = Time.now.to_f
        options[:targets].each do |target|
          file_starts_at = Time.now.to_f
          @included_files = []
          text        = include_file target
          target      = extension_to_js target
          output_file = options[:output] + '/' + target
          FileUtils.mkdir_p options[:output]
          must_uglify = @force_uglify == true || (not developer_mode?)
          File.open(output_file, "w:#{@encoding}") do |f|
            js = text.force_encoding(@encoding)
	    js = js_header + "\n" + js
            if must_uglify
              original_file = output_file + '.original.js'
              File.open(original_file, "w:#{@encoding}") {|f| f.write js}
              map_file       = output_file + '.map'
              map_options    = @uglifier_options.dup
	      source_url     = original_file.gsub /.*\/public/, ''
	      source_map_url = map_file.gsub /.*\/public/, ''
	      map_options[:source_filename] = source_url
	      map_options[:source_map_url]  = source_map_url
              js, map   = Uglifier.new(map_options).compile_with_map(js)
	      File.open(map_file, "w:#{@encoding}") {|f| f.write map }
            end
            f.write js
          end
          puts ">> Generated #{output_file} in #{(Time.now.to_f - file_starts_at).round 2}s"
        end
        message =  "Compiled javascript in #{(Time.now.to_f - starts_at).round 2} seconds."
        Crails::Notifier.notify get_project_name, message, image: :success
        :success
      rescue Exception => e
        message = "Failed to compile javascript:\n#{e.message}",
        Crails::Notifier.notify get_project_name, message, image: :failed
        :failure
      end
    end

    def run_on_modifications(paths)
      run_all
    end

    def extension_to_js filename
      filename.split('.')[0...-1].join('.') + '.js'
    end

    def clean_required_path filepath
      Pathname.new(filepath).cleanpath.to_s
    end

    def include_file file
      @included_files << file
      @file_cache[file] = JavascriptSource.new self, file if @file_cache[file].nil?
      @file_cache[file].compile if @file_cache[file].should_compile?
      @file_cache[file].compiled
    end

    def compile_coffeescript source, filepath
      source   = source.gsub /^#=([^\n]+)/, '`//=\1`'
      begin
        source = CoffeeScript.compile source, bare: true
      rescue ExecJS::RuntimeError => e
        puts "CoffeeScript compilation failed for `#{filepath}`: #{e.message}"
        raise "compilation error"
      end
      source
    end

    def compile_typescript source, filepath, current_dir
      begin
        source = TypeScript::Node.compile source
      rescue ExecJS::RuntimeError => e
        puts "TypeScript compilation failed for `#{filepath}`: #{e.message}"
        raise "compilation error"
      end
      source
    end

    def make_clean_required_path current_dir, required_path
      required_path = clean_required_path options[:input] + '/' + current_dir + '/' + required_path
      if required_path.start_with?(options[:input])
        required_path = required_path[options[:input].size..required_path.size]
      else
       can_match = true
        previous_parts = ""
        result = ""
        options[:input].split('/').each do |part|
          if can_match && required_path.start_with?("#{previous_parts}#{part}")
            previous_parts += "#{part}/"
          else
            can_match = false
            result += "../"
          end
        end
        required_path = '/' + result + required_path
      end
      required_path
    end

    def linker source, current_dir, &block
      comment_character = '//'
      file_content      = ''
      source.split("\n").each do |line|
        matches = line.match(/^\s*#{comment_character}=\s+(require|require_tree|include)\s+([^;]+)/)
        if matches.nil?
          block.call :line, line
        else
          required_path = make_clean_required_path current_dir, matches[2].strip
          if matches[1] == 'require_tree'
            path = "#{options[:input]}/#{required_path}/**/*"
	    path = path.gsub /\/+/, '/'
            files = Dir.glob("#{path}.coffee") + Dir.glob("#{path}.js")
            files.each do |filepath|
              filepath = filepath[options[:input].size..-1]
              unless @included_files.include? filepath
                block.call :include, filepath
              end
            end
          elsif (matches[1] == 'include') || (not @included_files.include? required_path)
            block.call :include, required_path
          end
        end
      end
      file_content
    end

    def resolve_linking source, current_dir
      file_content = ''
      linker source, current_dir do |type, data|
        if type == :line
          file_content += data + "\n"
        elsif type == :include
          file_content += include_file data
        end
      end
      file_content
    end
  end
end
