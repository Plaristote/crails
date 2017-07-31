require 'guard/crails-base'
require 'guard/crails-notifier'

module ::Guard
  class CrailsCmake < CrailsPlugin
    def run_all
      compile
    end

    def run_on_modifications(paths)
      run_all
    end

  private
    def compile
      Crails::Notifier.notify get_project_name, "Compiling..."
      success = false
      duration = nil
      success = run_cmake
      if success
        Dir.chdir build_path do
          puts ">> Make server"
          starts_at = Time.now.to_f
          run_command 'make'
          ends_at = Time.now.to_f
          duration = (ends_at - starts_at).round 2
          success = $?.success?
        end
      end
      if success
        Crails::Notifier.notify 'crails-cmake', "Compiled in #{duration}s", image: :success
      else
        Crails::Notifier.notify get_project_name, "Compilation failed", image: :failed
      end
    end
  end
end
