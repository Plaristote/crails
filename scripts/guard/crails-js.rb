require 'coffee-script'
require 'fileutils'
require 'pathname'

module ::Guard
  class CrailsJs < Plugin
    def initialize options = {}
      options[:watchers] << ::Guard::Watcher.new(%r{^#{options[:input]}/(.+\.(js|coffee))$})
      super options
    end

    def run_all
      @included_files = []
      options[:targets].each do |target|
        text        = compile_file target
        target      = extension_to_js target
        output_file = options[:output] + '/' + target
        FileUtils.mkdir_p options[:output]
        File.open(output_file, 'w') do |f|
          f.write "(function() {\n#{text}\n})();"
        end
        puts ">> Generated #{output_file}"
      end
    end

    def run_on_modifications(paths)
      run_all
    end

  private
    def extension_to_js filename
      filename.split('.')[0...-1].join('.') + '.js'
    end

    def compile_file file
      @included_files << file
      current_dir       = file.split('/')[0...-1].join('/')
      full_path         = (options[:input] + '/' + file).gsub(/\/+/, '/')
      source            = File.read full_path, encoding: 'BINARY'
      if file.match(/\.coffee$/) != nil
        source          = source.gsub /^#=([^\n]+)/, '`//=\1`'
        source          = CoffeeScript.compile source, bare: true
      end
      comment_character = "//"
      file_content      = ''
      source.split("\n").each do |line|
        matches = line.match(/^\s*#{comment_character}=\s+(require|require_tree|include)\s+([^;]+)/)
        if matches.nil?
          file_content += line + "\n"
        else
          required_path = Pathname.new(current_dir + '/' + matches[2].strip).cleanpath.to_s
          puts "Required path: #{required_path}"
          if matches[1] == 'require_tree'
            path = "#{options[:input]}/#{required_path}/**/*"
            files = Dir.glob("#{path}.coffee") + Dir.glob("#{path}.js")
            puts files.inspect
            files.each do |filepath|
              filepath = filepath[options[:input].size..-1]
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
