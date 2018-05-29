require 'guard/crails-base'
require 'guard/crails-notifier'

module ::Guard
  class CrailsCheerp < CrailsPlugin
    def run_all
      if not File.exists? cheerp_path
        puts "[crails-cheerp] cheerp not found at `#{cheerp_path}`. You may set the path using the cheerp_path option."
        return
      end

      command = ""
      command = "#{cc} #{cxx_flags} -o #{options[:output]} "
      if not options[:sourcemap_output].nil?
        command += "-g -cheerp-sourcemap='#{options[:sourcemap_output]}' "
      end
      include_paths.each do |include_path|
        command += "-I'#{include_path}' "
      end
      source_files.each do |src|
        command += src + ' '
      end
      puts "[crails-cheerp] compiling..."
      puts "+ #{command}"
      puts `#{command}`
    end

    def source_files
      input = options[:input] || "front"
      throw "no input folder found for crails-cheerp" unless File.exists? input
      lib = Dir["#{ENV['CRAILS_SHARED_DIR']}/front/**/*.cpp"]
      src = Dir["#{input}/**/*.cpp"]
      lib + src
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

