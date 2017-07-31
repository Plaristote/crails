require 'guard/crails-base'
require 'guard/crails-notifier'
require 'guard/crails-server'

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
      message = {
        console: 'Your crails server is recompiling right now',
        html: "<h4>crails-cmake: #{get_project_name}</h4><div>Recompiling right now...</div>"
      }
      Crails::Notifier.notify 'crails-cmake', message
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
        message = {
          console: 'Your crails server built successfully',
          html: "<h4>crails-cmake success</h4><div>All targets for <b>#{get_project_name}</b> built successfully.<br/>In #{duration}s.</div>"
        }
        Crails::Notifier.notify 'crails-cmake', message, :success
      else
        message = {
          console: 'Your crails server failed to build',
          html: "<h4>crails-cmake failure</h4><div>Failed to build some targets for <b>#{get_project_name}</b>.</div>"
        }
        Crails::Notifier.notify 'crails-cmake', message, :failure
      end
    end

    def restart_server
      server = CrailsServer.instance
      server.restart_server if not server.nil?
    end
  end
end

