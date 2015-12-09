require 'guard/crails-base'
require 'coffee-script'
require 'fileutils'
require 'pathname'
require 'uglifier'

module ::Guard
  class CrailsJs < CrailsPlugin
    def initialize options = {}
      options[:watchers] << ::Guard::Watcher.new(%r{^#{options[:input]}/(.+\.(js|coffee))$})
      @uglifier_options = options[:uglifier] || {}
      super options
    end

    def run_all
      begin
        options[:targets].each do |target|
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
          puts ">> Generated #{output_file}"
        end
      rescue Exception => e
        puts_error "Catched exception: #{e.message}"
        puts_error "!! crailsjs couldn't perform its duties"
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

    def puts_error str
      puts "\033[31m#{str}\033[0m"
    end

    def compile_file file
      @included_files << file
      current_dir       = file.split('/')[0...-1].join('/')
      full_path         = (options[:input] + '/' + file).gsub(/\/+/, '/')
      source            = File.read full_path, encoding: 'BINARY'
      if file.match(/\.coffee$/) != nil
        source          = source.gsub /^#=([^\n]+)/, '`//=\1`'
        begin
          source        = CoffeeScript.compile source, bare: true
        rescue ExecJS::RuntimeError => e
          puts_error "CoffeeScript compilation failed for `#{file}`: #{e.message}"
          raise "compilation error"
        end
      end
      comment_character = "//"
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
                file_content += (compile_file filepath) + "\n"
              end
            end
          elsif not @included_files.include? required_path
            file_content     += (compile_file required_path) + "\n"
          end
        end
      end
      file_content
    end
  end
end
