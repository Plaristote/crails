require 'guard/crails-base'
require 'json'

module ::Guard
  class CrailsOdb < CrailsPlugin
    def initialize options = {}
      super
      @backends        = options[:backends] || get_cmake_variable('SQL_BACKEND')
      @output_dir      = options[:output] || "lib/odb"
      @include_prefix  = options[:include_prefix] || "app/models"
      @table_prefix    = options[:table_prefix]
      @cpp_version     = options[:std] || "c++11"
      @default_pointer = options[:default_pointer]
      @hxx_prologue    = options[:hxx_prologue]
      @ixx_prologue    = options[:ixx_prologue]
      @cxx_prologue    = options[:cxx_prologue]
      @schema_prologue = options[:schema_prologue]
      @requires        = ['crails/safe_ptr.hpp']
      @requires       += options[:requires] unless options[:requires].nil?
    end

    def run_all
      run_on_modifications watched_files
    end

    def run_on_modifications(paths)
      files = fetch_odb_files_in paths
      if files.count > 0
        compile_models files
        fix_ixx_includes
        generate_schema
      else
        puts "[crails-odb] Nothing to compile"
      end
    end

  private
    def fetch_odb_files_in paths
      odb_files = []
      paths.each do |path|
        content = File.read path
        odb_files << path if content =~ /^\s*#\s*pragma\s+db/
      end
      odb_files
    end

    def generate_hxx_prologue
      code = ''
      @requires.each do |requirement|
        code += "#include \\\"#{requirement}\\\"\n"
      end
      code += @hxx_prologue unless @hxx_prologue.nil?
      code
    end

    def generate_include_prefix path
      parts = path.split '/'
      parts[0...-1].join('/')
    end

    # TODO find how the ODB compiler can do this part for us
    def fix_ixx_includes
     Dir["lib/odb/*"].each do |file_name|
       text = File.read(file_name)
       new_contents = text.gsub /#include\s+["<][^">]*(hxx|ixx)[">]/ do |str|
         unless str.index(@include_prefix).nil?
           res = str.scan /["<](.*)(hxx|ixx)[">]/
           res = res.flatten.join('').inspect
           res = res[1...-1]
           "#include \"#{res.split('/').last}\""
         else
           str
         end
       end

       File.open(file_name, "w") {|file| file.puts new_contents }
      end
    end

    def odb_options_schema
      odb_options("tasks/odb_migrate", nil) + " --generate-schema-only --at-once --input-name application "
    end

    def odb_options output_dir, prefix_path
      options  = "-I. "
      options += "--std #{@cpp_version} "
      options += "--schema-format separate " 
      options += "--hxx-prologue \"#{generate_hxx_prologue}\" "
      options += "--cxx-prologue '#{@cxx_prologue}'" unless @cxx_prologue.nil?
      options += "--ixx-prologue '#{@ixx_prologue}'" unless @ixx_prologue.nil?
      options += "--schema-prologue '#{@schema_prologue}'" unless @schema_prologue.nil?
      options += "--output-dir \"#{output_dir}\" "
      options += "--include-prefix \"#{prefix_path}\" " unless prefix_path.nil?
      options += "--table-prefix \"#{@table_prefix}\" " unless @table_prefix.nil?
      options += "--default-pointer \"#{@default_pointer}\" " unless @default_pointer.nil?
      options += "-d " + (@backends.uniq.join " -d ") + " "
      options += "--multi-database dynamic -d common " if @backends.count > 1
      options + "--generate-query "
    end

    def compile_models paths
      paths_by_prefix = {}
      paths.each do |path|
        paths_by_prefix[generate_include_prefix path] ||= []
        paths_by_prefix[generate_include_prefix path] << path
      end

      paths_by_prefix.each do |key,value|
        cmd = "odb #{odb_options @output_dir, key} #{value.join ' '}"
        puts "+ #{cmd}"
        `#{cmd}`
      end
    end

    def generate_schema
      paths = fetch_odb_files_in watched_files
      cmd   = "odb #{odb_options_schema} #{paths.join ' '}"
      puts "+ #{cmd}"
      `#{cmd}`
    end
  end
end
