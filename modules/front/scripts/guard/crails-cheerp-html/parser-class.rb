require 'crails-cheerp-html/parser-context'
require 'crails-cheerp-html/parser-reference'
require 'crails-cheerp-html/parser-binding'

module CrailsCheerpHtml
  class Class < Context
    attr_accessor :typename, :inline_code
    attr_reader :el, :parent, :children, :superclass, :refs, :slots, :slot_plugins, :repeaters, :bindings, :event_listeners

    def initialize el, parent = nil
      context.classes << self
      parent.children << self unless parent.nil?
      el["_cheerp_class"] = self if el.class != Nokogiri::XML::NodeSet
      @el              = el
      @superclass      = context.template_base_type
      @parent          = parent
      @refs            = []
      @bindings        = []
      @slots           = []
      @slot_plugins    = []
      @repeaters       = []
      @event_listeners = []
      @children        = []
    end

    def constructor_decl
      "#{@typename}(#{if parent.nil? then "" else "#{parent.typename}*" end});"
    end

    def find_reference_for el
      @refs.each do |ref|
        return ref if ref.el == el
      end
      nil
    end
    
    def is_root?
      @parent.nil?
    end
    
    def collect_children
      children = []
      context.classes.each do |object|
        children << object if object.parent == self
      end
      children
    end

    def recursively_collect_children
      children = collect_children
      descendants = []
      children.each do |child|
        descendants += child.recursively_collect_children
      end
      children + descendants
    end

    def root
      current_class = @parent
      current_class = current_class.parent until current_class.parent.nil?
      current_class
    end

    def probe
      probe_subtypes
      probe_slots_plugins
      probe_references
      probe_bindings
    end
    
    def probe_subtypes root = nil
      root = @el if root.nil?
      root.children.each do |el|
        if Slot::Probe.matches? el
          @slots << Slot.new(el, self)
          @slots.last.probe
        elsif Repeater::Probe.matches? el
          @repeaters << Repeater.new(el, self)
          @repeaters.last.probe
        else
          probe_subtypes el unless context.has_cpp_type?(el)
        end
      end
    end

    def probe_references root = nil
      root = @el if root.nil?
      root.children.each do |el|
        next unless el["_cheerp_class"].nil?
        next unless find_reference_for(el).nil?
        if !(el["ref"].nil?)
          @refs << Reference.new(el, self, :explicit)
        elsif context.has_cpp_type?(el)
          @refs << Reference.new(el, self, :implicit)
        end
        probe_references el unless context.has_cpp_type?(el)
      end
    end
    
    def probe_bindings root = nil
      root = @el if root.nil?
      root.children.each do |el|
        next unless el["_cheerp_class"].nil?
        el.attributes.each do |key, value|
          if key.end_with? ".trigger"
            @event_listeners << EventListener.new(el, self, key, value)
          elsif key.end_with? ".bind"
            @bindings << Binding.new(el, self, key, value)
          end
        end
        probe_bindings el unless context.has_cpp_type?(el)
      end
    end
    
    def probe_slots_plugins root = nil
      root = @el if root.nil?
      root.children.each do |el|
        next unless el["_cheerp_class"].nil?
        if context.has_cpp_type?(el)
          el.children.each do |candidate|
            unless candidate["slot"].nil?
              @slot_plugins << (SlotPlugin.new candidate, self, el)
              @slot_plugins.last.probe
            else
              candidate.remove # Non-slot children for cpp custom elements must be ignored
            end
          end
        else
          probe_slots_plugins el
        end
      end
    end

  end
end
