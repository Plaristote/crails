require 'coffee-script'
require 'fileutils'

module ::Guard
  class CrailsJs < Plugin
    def initialize options = {}
      options[:watchers] << ::Guard::Watcher.new(%r{^#{options[:input]}/(.+\.(js|coffee))$})
      super options
    end

    def run_all
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
      current_dir       = file.split('/')[0...-1].join('/')
      full_path         = (options[:input] + '/' + file).gsub(/\/+/, '/')
      source            = File.read full_path, encoding: 'BINARY'
      source            = CoffeeScript.compile source, bare: true if file.match(/\.coffee$/) != nil
      comment_character = "//"
      file_content      = ''
      source.split("\n").each do |line|
        matches = line.match(/^\s*#{comment_character}=\s+(require|include)\s+([^;]+)/)
        if matches.nil?
          file_content += line + "\n"
        else
          required_filename = current_dir + '/' + matches[2]
          file_content     += (compile_file required_filename.strip) + "\n"
        end
      end
      file_content
    end
  end
end
