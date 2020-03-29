require 'guard/crails-base'
require 'guard/crails-notifier'

module ::Guard
  class CrailsTests < CrailsPlugin
    def run_all
      run_tests
    end
  private
    def run_command command
      last_line = nil
      success = true
      PTY.spawn(command) do |stdout, stdin, pid|
        begin
          stdout.each {|line|
            print line
            last_line = line
          }
        rescue Errno::EIO
        end
        Process.wait(pid)
        success = $?.success?
      end
      last_line.uncolorize
      if success then :success else :failure end
    end

    def run_tests
       starts_at = Time.now.to_f
       command   = find_test_binary
       last_line = run_command command
       ends_at   = Time.now.to_f
       image     = $?.success? ? 'success' : 'failed'
       message   = image + "\n"
       message  += last_line.uncolorize + "\n"
       message  += "In #{(ends_at - starts_at).round 2}s"
       Crails::Notifier.notify get_project_name, message, image: image
    end

    def find_test_binary
      [ "#{build_path}/tests", 'bin/tests' ].each do |candidate|
        return candidate if File.exists? candidate
      end
      throw "cannot find binary for crails-tests"
    end
  end
end
