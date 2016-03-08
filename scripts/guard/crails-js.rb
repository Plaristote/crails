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
  class CrailsJs < CrailsPlugin
    def initialize options = {}
      options[:watchers] << ::Guard::Watcher.new(%r{^#{options[:input]}/(.+\.(js|coffee|ts))$})
      @uglifier_options = options[:uglifier] || {}
      super options
    end

    def run_all
      begin
        @file_cache = {}
        starts_at = Time.now.to_f
        options[:targets].each do |target|
          file_starts_at = Time.now.to_f
          @included_files = []
          text        = compile_file target
          target      = extension_to_js target
          output_file = options[:output] + '/' + target
          FileUtils.mkdir_p options[:output]
          File.open(output_file, 'w') do |f|
            js = "(function() {\n#{text}\n})();".force_encoding('UTF-8')
            js = Uglifier.compile(js, @uglifier_options) unless developer_mode?
            f.write js
          end
          puts ">> Generated #{output_file} in #{(Time.now.to_f - file_starts_at).round 2}s"
        end
        message = {
          console: "Finished crailsjs job in #{(Time.now.to_f - starts_at).round 2} seconds.",
          html: "<h4>crails-js success</h4><div>Finished crails-js job in #{(Time.now.to_f - starts_at).round 2} seconds."
        }
        Crails::Notifier.notify 'crails-js', message, :success
      rescue Exception => e
        message = {
          console: "Catched exception: #{e.message}\n!! crailsjs couldn't perform its duties",
          html: "<h4>crails-js failure</h4><div>Catched exception: #{e.message}"
        }
        Crails::Notifier.notify 'crails-js', message, :failure
      end
    end

    def run_on_modifications(paths)
      run_all
    end

  private
    def extension_to_js filename
      filename.split('.')[0...-1].join('.') + '.js'
    end

    def clean_required_path filepath
      Pathname.new(filepath).cleanpath.to_s
    end

    def include_file file
      @included_files << file
      @file_cache[file] = compile_file file if @file_cache[file].nil?
      @file_cache[file]
    end

    def compile_file file
      current_dir       = file.split('/')[0...-1].join('/')
      full_path         = (options[:input] + '/' + file).gsub(/\/+/, '/')
      source            = File.read full_path, encoding: 'BINARY'
      if HAS_COFFEESCRIPT && file.match(/\.coffee$/) != nil
        source = compile_coffeescript source
      elsif HAS_TYPESCRIPT && file.match(/\.ts$/) != nil
        source = compile_typescript source, current_dir
      end
      resolve_linking source, current_dir
    end

    def compile_coffeescript source
      source          = source.gsub /^#=([^\n]+)/, '`//=\1`'
      begin
        source        = CoffeeScript.compile source, bare: true
      rescue ExecJS::RuntimeError => e
        puts_error "CoffeeScript compilation failed for `#{file}`: #{e.message}"
        raise "compilation error"
      end
      source
    end

    def compile_typescript source, current_dir
      begin
        source        = resolve_linking source, current_dir # TypeScript needs the linking to be done before compilation
        source        = TypeScript::Node.compile source
      rescue ExecJS::RuntimeError => e
        puts_error "TypeScript compilation failed for `#{file}`: #{e.message}"
        raise "compilation error"
      end
      source
    end

    def resolve_linking source, current_dir
      comment_character = '//'
      file_content      = ''
      source.split("\n").each do |line|
        matches = line.match(/^\s*#{comment_character}=\s+(require|require_tree|include)\s+([^;]+)/)
        if matches.nil?
          file_content += line + "\n"
        else
          required_path = clean_required_path current_dir + '/' + matches[2].strip
          if matches[1] == 'require_tree'
            path = "#{options[:input]}/#{required_path}/**/*"
            files = Dir.glob("#{path}.coffee") + Dir.glob("#{path}.js")
            files.each do |filepath|
              filepath = clean_required_path filepath[options[:input].size..-1]
              unless @included_files.include? filepath
                file_content += (include_file filepath) + "\n"
              end
            end
          elsif not @included_files.include? required_path
            file_content     += (include_file required_path) + "\n"
          end
        end
      end
      file_content
    end
  end
end
