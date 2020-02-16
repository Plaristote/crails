require 'guard/crails-base'
require 'guard/crails-notifier'
require 'fileutils'
require 'json'
require 'crails-cheerp-html/generator'

module ::Guard
  class CrailsCheerpHtml < CrailsPlugin
    def initialize arg
      @input  = arg[:input]  || "front"
      @output = arg[:output] || "lib/cheerp-html"
      @config_file = "config/front.json"
      super
    end

    def run_all
      compile
    end
    
    def write_if_content_changed file, content
      if !File.exists?(file) || File.read(file) != content
        File.open file, 'w' do |f| f.write content end
        puts "[crails-cheerp-html] Generated #{file}"
      end
    end
    
    def purge_old_files whitelist
      Dir["#{@output}/**/*"].each do |file|
        next if File.directory? file
        File.delete file unless whitelist.include? file
      end
    end

    def compile
      @config = if File.exists?(@config_file) then JSON.parse(File.read "config/front.json") else nil end
      current_filename = nil
      begin
        generated_files = []
        watched_files.each do |file|
          current_filename = file
          generator = ::CrailsCheerpHtml::Generator.new @output, @input, file
          generator.set_config @config unless @config.nil?
          header_file = generator.compiled_header_path
          source_file = generator.compiled_source_path

          FileUtils.mkdir_p File.dirname(header_file)
          code = generator.generate
          write_if_content_changed header_file, code[:header]
          write_if_content_changed source_file, code[:source]
          generated_files << header_file << source_file
        end
        purge_old_files generated_files
      rescue ::CrailsCheerpHtml::ParseError => e
        puts "[crails-cheerp-html] parse error at #{current_filename}:#{e.line}: #{e.message}"
      rescue StandardError => e
        puts e.message
        e.backtrace.each do |line| puts line end
        set_exit_success -1
      end
    end
  end
end
