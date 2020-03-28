require 'guard/crails-base-templates'
require 'guard/crails-notifier'

module ::Guard
  class CrailsArchive < CrailsTemplatePlugin
    def initialize arg
      super arg
      @base_path     = "app/views/"
      @template_type = "archive"
      @extension     = "arch.cpp"
    end

    def compile_archive filename
    end
  end
end

