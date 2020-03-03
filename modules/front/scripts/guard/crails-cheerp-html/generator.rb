require 'crails-cheerp-html/parser-class'
require 'crails-cheerp-html/parser-repeater'
require 'crails-cheerp-html/parser-slot'
require 'crails-cheerp-html/utils'
require 'crails-cheerp-html/header-generator'
require 'crails-cheerp-html/source-generator'
require 'nokogiri'
require 'pathname'

module CrailsCheerpHtml
  class LoadError < StandardError
    attr_reader :message
    def initialize
      @message = "Load error: #{message}"
    end
  end

  class ParseError < StandardError
    attr_reader :el, :message

    def initialize el, message
      @el = el
      @message = message
    end
    
    def line
      el.line
    end
  end
  
  class HtmlParseError < StandardError
    attr_reader :message, :line
    
    def initialize error
      @message = error.message
      @line    = error.line
    end
  end

  class Generator
    def initialize output, input, file
      @output   = output
      @input    = input
      @filepath = file
    end

    def set_config config
      @config = config
    end
    
    def relative_filepath
      @filepath[@input.length..-1]
    end

    def compiled_header_path
      @output + relative_filepath.delete_suffix(File.extname relative_filepath) + ".hpp"
    end

    def compiled_source_path
      @output + relative_filepath.delete_suffix(File.extname relative_filepath) + ".cpp"
    end
  
    def append_classes_predeclaration
      result = ""
      Context.classes.each do |object|
        next if object.should_skip?
        result += "  class #{object.typename};\n"
      end
      result
    end
    
    def load_includes head
      head.css("include").each do |include_attribute|
        unless include_attribute["require"].nil?
          type = include_attribute["require"].to_s
          tag  = if include_attribute["tag-name"].nil?
            parts = type.split("<")
            parts[0].gsub! "::", "_"
            parts.join("<").dasherize
          else
            include_attribute["tag-name"].to_s
          end
          Context.element_types[tag] = type
        end
      end
    end

    def prepare_includes head
      result = ""
      included_paths = []
      head.css("include").each do |include_attribute|
        included_paths << include_attribute["src"].to_s
      end
      included_paths.uniq.each do |include_path|
        result += "# include \"#{include_path}\"\n"
      end
      @config["elements"].each do |element_data|
        if Context.referenced_types.include? element_data["require"]
          result += "# include \"#{element_data["include"]}\"\n"
        end
      end if !@config.nil? && !@config["elements"].nil?
      result
    end

    def html5_tags
      ["article", "aside", "details", "figcaption", "figure", "footer", "header", "main", "mark", "nav", "section", "summary", "time"]
    end
    
    def whitelisted_tags
      ["svg"] + (@config["whitelisted-tags"] || [])
    end
    
    def internal_tags
      ["include", "attribute", "slot"]
    end

    def is_error_fatal? error
      matches = error.message.match(/^[0-9]+:[0-9]+: ERROR: Tag (.+) invalid$/)
      unless matches.nil?
        tagName = matches[1]
        return false if (internal_tags + html5_tags + whitelisted_tags).include? tagName
        return (if Context.element_types[tagName].nil? then true else false end)
      end
      return false if error.message.end_with?("ERROR: htmlParseEntityRef: no name") ||
                      error.message.end_with?("ERROR: Expected a doctype token$")
      true
    end

    def generate
      src = File.read @filepath
      src.gsub! /<template/i,  "<html"
      src.gsub! /<\/template/i, "</html>"

      document = Nokogiri::HTML.parse(src) do |config| config.strict end
      
      html = document.xpath("html").first
      head = document.xpath("//head")
      body = document.xpath("//body")
      raise LoadError.new "could not load html template `#{@filepath}`" if html.nil?
      raise LoadError.new "missing body element in `#{@filepath}`" if body.first.nil?

      Context.reset
      Context.filename = @filepath
      if !@config.nil? && !@config["elements"].nil?
        Context.load_global_element_types @config["elements"]
      end
      
      load_includes head
      
      fatal_error = nil
      document.errors.each do |error|
        if is_error_fatal? error
          puts "[crails-cheerp-html] #{@filepath}:#{error.line}:#{error.column}: #{error.message}"
          fatal_error ||= error
          error_count += 1
        end
      end
      raise HtmlParseError.new fatal_error unless fatal_error.nil?

      main_element = CrailsCheerpHtml::Class.new body
      main_element.typename = File.basename(@filepath, ".html").to_s.camelize
      main_element.superclass = html["extends"] unless html["extends"].nil?
      main_element.probe

      main_element.inline_code = ""
      head.css("script").each do |script|
        main_element.inline_code += script.text.strip + "\n"
      end

      head.css("attribute").each do |attribute|
        main_element.refs << (CppReference.new attribute, attribute["type"], attribute["name"], attribute["value"])
      end

      define_name = "__CRAILS_FRONT_HTML_#{main_element.typename.upcase}__"
      header_code = ""
      source_code = ""
      Context.classes.reverse.each do |object|
        next if object.should_skip?
        header_code += HeaderGenerator.new(object).generate + "\n"
        source_code += SourceGenerator.new(object).generate + "\n"
      end

      includes_str = prepare_includes head
      
      header = <<HEADER
#ifndef  #{define_name}
# define #{define_name}

# include <crails/front/bindable.hpp>
# include <crails/front/repeater.hpp>
# include <crails/front/slot_element.hpp>
# include <crails/front/comment_element.hpp>
# include <crails/front/custom_element.hpp>
# include <crails/front/signal.hpp>
#{includes_str}

namespace HtmlTemplate
{
#{append_classes_predeclaration}
#{header_code}
}

#endif
HEADER

      source = <<SOURCE
#include <boost/lexical_cast.hpp>
#include "#{compiled_header_path}"

#{source_code}
SOURCE

      { header: header, source: source }
    end
  end
end
