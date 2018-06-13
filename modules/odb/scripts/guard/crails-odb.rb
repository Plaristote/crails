require 'guard/crails-base'
require 'json'
require 'fileutils'

module ::Guard
  class CrailsOdb < CrailsPlugin
    def initialize options = {}
      super
      @input_name       = "application"
      @file_matcher     = options[:matcher]
      @base_path        = options[:input] || "app/models/" 
      @at_once          = options[:at_once]
      @embed_schema     = options[:embed_schema]
      @output_dir       = options[:output] || "lib/odb"
      @temporary_output = options[:temporary_output] || "./.tmp/odb"
      @include_prefix   = options[:include_prefix] || "app/models"
      @table_prefix     = options[:table_prefix]
      @cpp_version      = options[:std] || "c++11"
      @default_pointer  = options[:default_pointer]
      @hxx_prologue     = options[:hxx_prologue]
      @ixx_prologue     = options[:ixx_prologue]
      @cxx_prologue     = options[:cxx_prologue]
      @schema_prologue  = options[:schema_prologue]
      @generate_session = options[:generate_session]
      @defines          = options[:defines]
      @requires         = ['crails/safe_ptr.hpp', 'crails/odb/traits.hpp', 'config/odb.hpp']
      @requires        += options[:requires] unless options[:requires].nil?
    end

    def run_all
      if run_cmake
        run_on_modifications watched_files
      end
    end

    def run_on_modifications(paths)
      with_tmp_dir do
        files = fetch_odb_files_in paths
        files = watched_files if @at_once == true
        if files.count > 0
          compile_models files
          generate_schema
        else
          puts "[crails-odb] Nothing to compile"
        end
      end
    end

  private
    def with_tmp_dir &block
      `mkdir -p '#{@temporary_output}'`
      block.call
      `rm -Rf '#{@temporary_output}'`
    end

    def get_active_backends
      backends        = [:pgsql, :mysql, :oracle, :sqlite]
      active_backends = []
      backends.each do |backend|
        value = get_cmake_variable("ODB_WITH_#{backend.to_s.upcase}")
        active_backends << backend if value == "ON"
      end
      active_backends
    end

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
     Dir["#{@temporary_output}/*"].each do |file_name|
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

    def odb_options output_dir, prefix_path
      backends = get_active_backends
      options  = "-I. "
      options += "--std #{@cpp_version} "
      options += "--schema-format separate "
      options += "--hxx-prologue \"#{generate_hxx_prologue}\" "
      if @at_once == true
        options += "--at-once "
        options += "--input-name #{@input_name} "
      end
      options += "--cxx-prologue '#{@cxx_prologue}' " unless @cxx_prologue.nil?
      options += "--ixx-prologue '#{@ixx_prologue}' " unless @ixx_prologue.nil?
      options += "--schema-prologue '#{@schema_prologue}' " unless @schema_prologue.nil?
      options += "--output-dir \"#{output_dir}\" "
      options += "--include-prefix \"#{prefix_path}\" " unless prefix_path.nil?
      options += "--table-prefix \"#{@table_prefix}\" " unless @table_prefix.nil?
      options += "--default-pointer \"#{@default_pointer}\" " unless @default_pointer.nil?
      options += "-d " + (backends.uniq.join " -d ") + " "
      options += "--multi-database dynamic -d common " if backends.count > 1
      if @defines.class == Array
        options += (@defines.map {|d| "-D#{d}"}).join ' '
        options += ' '
      elsif @defines.class == Hash
        options += (@defines.map {|k,v| "-D#{k}=#{v}"}).join ' '
        options += ' '
      end
      options += "--generate-session " if @generate_session == true
      options + "--generate-query "
    end

    def compile_models paths
      paths_by_prefix = {}
      paths.each do |path|
        paths_by_prefix[generate_include_prefix path] ||= []
        paths_by_prefix[generate_include_prefix path] << path
      end

      paths_by_prefix.each do |key,value|
        cmd = "odb #{odb_options @temporary_output, key} #{value.join ' '}"
        puts "+ #{cmd}"
        `#{cmd}`
      end

      fix_ixx_includes
      apply_new_version @output_dir
    end

    def generate_schema
      paths = fetch_odb_files_in watched_files

      schema_path = if @embed_schema then @output_dir else "tasks/odb_migrate" end
      odb_options_schema = odb_options(@temporary_output, nil) + " --generate-schema-only --at-once --input-name #{@input_name} "

      cmd   = "odb #{odb_options_schema} #{paths.join ' '}"
      puts "+ #{cmd}"
      `#{cmd}`
      apply_new_version schema_path
    end

    def apply_new_version target_dir
      FileUtils.mkdir_p target_dir
      Dir["#{@temporary_output}/*"].each do |generated_file|
        filename = generated_file.split('/').last
        last_generated_file = "#{target_dir}/#{filename}"
        if File.exists? last_generated_file
          if File.read(generated_file) == File.read(last_generated_file)
            FileUtils.rm generated_file
            next
          end
        end
        FileUtils.mv generated_file, last_generated_file
        puts "[crailsodb] updated #{last_generated_file}"
      end
    end
  end
end
