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

    def compile
      FileUtils.rm_rf @output
      @config = if File.exists?(@config_file) then JSON.parse(File.read "config/front.json") else nil end
      begin
        watched_files.each do |file|
          generator = ::CrailsCheerpHtml::Generator.new @output, @input, file
          generator.set_config @config unless @config.nil?
          header_file = generator.compiled_header_path
          source_file = generator.compiled_source_path

          FileUtils.mkdir_p File.dirname(header_file)
          code = generator.generate
          File.open header_file, 'w' do |f| f.write code[:header] end
          File.open source_file, 'w' do |f| f.write code[:source] end
        end
      rescue StandardError => e
        puts e.message
        e.backtrace.each do |line| puts line end
        set_exit_success -1
      end
    end
  end
end
