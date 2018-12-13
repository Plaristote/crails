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

    def run_all
      compile
    end

  private
    def cheerp_path
      options[:cheerp_path] || "/opt/cheerp"
    end

    def make_symbolic_links
      prefix = ""
      p1 = Pathname.new(File.expand_path(@output)).parent
      p2 = FileUtils.pwd
      while p1.to_s != p2.to_s
        p1 = p1.parent
	prefix += "../"
      end

      built_path = "#{front_build_path}/application.js"
      if File.exists? built_path
        `unlink "#{@output}"`
        `ln -s "#{prefix}#{built_path}" "#{@output}"`
        if File.exists? "#{built_path}.map"
          `unlink "#{@output}.map"`
          `ln -s "#{prefix}#{built_path}.map" "#{@output}.map"`
        end
      end
    end

    def compile
      Crails::Notifier.notify get_project_name, "Compiling Front..."
      duration = nil
      success = run_front_cmake
      if success
        Dir.chdir front_build_path do
          puts ">> Make client"
          starts_at = Time.now.to_f
          run_command 'make'
          ends_at = Time.now.to_f
          duration = (ends_at - starts_at).round 2
          success = $?.success?
        end
      end
      if success
        make_symbolic_links
        Crails::Notifier.notify 'crails-cheerp', "Compiled in #{duration}s", image: :success
      else
        Crails::Notifier.notify 'crails-cheerp', "Compilation failed", image: :failed
      end
    end

    def run_front_cmake
      cmakelists_path = Dir.pwd + "/front"
      opts = []
      opts << "-DCMAKE_TOOLCHAIN_FILE=#{cheerp_path}/share/cmake/Modules/CheerpToolchain.cmake"
      opts << "-DDEVELOPER_MODE=#{developer_mode? ? "ON" : "OFF"}"
      FileUtils.mkdir_p front_build_path
      Dir.chdir front_build_path do
        run_command "cmake #{opts.join ' '} #{cmakelists_path}"
      end
      $?.success?
    end

    def front_build_path
      "#{build_path}/front"
    end
  end
end
