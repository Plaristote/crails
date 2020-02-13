require 'crails-cheerp-html/parser-class'
require 'crails-cheerp-html/parser-repeater'
require 'crails-cheerp-html/parser-slot'
require 'crails-cheerp-html/utils'
require 'crails-cheerp-html/header-generator'
require 'crails-cheerp-html/source-generator'
require 'nokogiri'
require 'pathname'

module CrailsCheerpHtml
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
        result += "  class #{object.typename};\n"
      end
      result
    end

    def prepare_includes head
      result = ""
      head.css("include").each do |include_attribute|
        include_path = include_attribute["src"]
        unless include_attribute["require"].nil?
          include_attribute["require"].to_s.split(";").each do |type|
            parts = type.split("<")
            parts[0].gsub! "::", "_"
            html_type = parts.join("<").dasherize
            Context.element_types[html_type] = type
          end
        end
        result += "# include \"#{include_path}\"\n"
      end
      @config["elements"].each do |element_data|
        result += "# include \"#{element_data["include"]}\"\n"
      end if !@config.nil? && !@config["elements"].nil?
      result
    end

    def generate
      src = File.read @filepath
      src.gsub! /<template/i,  "<html"
      src.gsub! /<\/template/i, "</html>"

      document = Nokogiri::HTML.parse(src)
      html = document.xpath("html")
      head = document.xpath("//head")
      body = document.xpath("//body")

      Context.reset
      if !@config.nil? && !@config["elements"].nil?
        Context.load_global_element_types @config["elements"]
      end

      includes_str = prepare_includes head

      main_element = CrailsCheerpHtml::Class.new body
      main_element.typename = File.basename(@filepath, ".html").to_s.camelize
      main_element.probe
      
      main_element.inline_code = ""
      head.css("script").each do |script|
        main_element.inline_code += script.text.strip + "\n"
      end

      head.css("attribute").each do |attribute|
        main_element.refs << (CppReference.new attribute["type"], attribute["name"], attribute["value"])
      end
      
      define_name = "__CRAILS_FRONT_HTML_#{main_element.typename.upcase}__"
      
      header_code = ""
      source_code = ""
      Context.classes.each do |object|
        header_code += HeaderGenerator.new(object).generate + "\n"
        source_code += SourceGenerator.new(object).generate + "\n"
      end
      
      header = <<HEADER
#ifndef  #{define_name}
# define #{define_name}

# include <crails/front/bindable.hpp>
# include <crails/front/repeater.hpp>
# include <crails/front/slot_element.hpp>
# include <crails/front/element.hpp>
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
