require 'guard/crails-base'
require 'guard/crails-notifier'
require 'pathname'

module ::Guard
  class CrailsCheerp < CrailsPlugin
    def initialize arg
      @input  = arg[:input]  || "front"
      @output = arg[:output] || "public/assets/application.js"
      @atonce = arg[:at_once]
      super
    end

    def command_base
      command = ""
      command = "#{cc} #{cxx_flags} "
      command
    end

    def run_all
      if not File.exists? cheerp_path
        puts "[crails-cheerp] cheerp not found at `#{cheerp_path}`. You may set the path using the cheerp_path option."
        return
      end

      timer_start = Time.now

      command = command_base
      command += "-g " if not options[:sourcemap_output].nil?
      command += "-cheerp-preexecute " if options[:preexecute]
      include_paths.each do |include_path|
        command += "-I'#{include_path}' "
      end
      files_to_link = []

      puts "[crails-cheerp] compiling..."
      item_count = source_files.count.to_f
      source_files.each_with_index do |src, index|
        if @atonce
          command += src + ' '
          next
	end

	source_path    = Pathname.new(src)
	compile_path   = Pathname.new(cheerp_compiled_path + '/' + src + '.bc')

	compile_path.parent.mkpath unless compile_path.parent.directory?
	should_compile = (not compile_path.file?) || (source_path.mtime > compile_path.mtime)

	if should_compile
          compile_command = command + " -o #{compile_path.to_s} -c #{source_path.to_s}"
	  puts "[#{(index / item_count * 100).ceil.to_s.rjust(2, '0')}%] #{source_path.to_s.green}"
          print `#{compile_command}`
	  if !($?.success?)
            Crails::Notifier.notify get_project_name, "Cheerp compilation failed", image: :failed
	    return
	  end
	end
	files_to_link << compile_path.to_s
      end
      timer_unit_compilation = Time.now

      unless @atonce
        command = command_base
        command += "-cheerp-preexecute " if options[:preexecute]
      end

      if not options[:sourcemap_output].nil?
        command += "-cheerp-sourcemap='#{options[:sourcemap_output]}' "
        command += "-cheerp-sourcemap-standalone " if options[:sourcemap_standalone]
      end

      compile_binary_command = "#{command} -o #{@output} #{files_to_link.join(' ')}"
      puts "+ #{compile_binary_command}"
      puts `#{compile_binary_command}`
      success = $?.success?
      timer_all_compilation = Time.now
      all_time     = (timer_all_compilation - timer_start).ceil
      compile_time = (timer_unit_compilation - timer_start).ceil
      link_time    = (timer_all_compilation - timer_unit_compilation).ceil
      puts "[crails-cheerp] compiled in #{all_time}s (linking: #{link_time}s, compilation: #{compile_time}s)"
      if !($?.success?)
        Crails::Notifier.notify get_project_name, "Cheerp compiled in #{all_time}s", image: :success
      end
    end

    def source_files
      lib = Dir["#{ENV['CRAILS_SHARED_DIR']}/front/**/*.cpp"]
      src = []
      if @input.class == Proc
        src = @input.call
      elsif @input.class == Array
        @input.each do |input_folder|
          src += Dir["#{input_folder}/**/*.cpp"]
        end
      else
        throw "no input folder found for crails-cheerp" unless File.exists? @input
        src = Dir["#{@input}/**/*.cpp"]
      end
      lib + src
    end

    def cheerp_compiled_path
      ".tmp/cheerp"
    end

    def cheerp_path
      options[:cheerp_path] || "/opt/cheerp"
    end

    def cc
      "#{cheerp_path}/bin/clang++"
    end

    def cxx_flags
      flags = "-target cheerp -D__CHEERP_CLIENT__"
      flags += "-frtti" if options[:with_rtti] == true
      flags += options[:cxx_flags] if not options[:cxx_flags].nil?
      flags
    end

    def include_paths
      paths = ["#{cheerp_path}/include/client"]
      paths += options[:include_paths] unless options[:include_paths].nil?
      paths
    end
  end
end

