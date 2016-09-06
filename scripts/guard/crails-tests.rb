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
      PTY.spawn(command) do |stdout, stdin, pid|
        begin
          stdout.each {|line|
            print line
            last_line = line
          }
        rescue Errno::EIO
        end
        Process.wait(pid)
	set_exit_success $?.exitstatus
      end
      last_line.uncolorize
    end

    def run_tests
       starts_at = Time.now.to_f
       command   = find_test_binary
       last_line = run_command command
       ends_at   = Time.now.to_f
       html  = "<h4>crails-tests #{$?.success? ? 'passed' : 'failed'}</h4>"
       last_line.uncolorize
       html += "<div>#{last_line}<br/>In #{(ends_at - starts_at).round 2}s</div>"
       if $?.success?
         Crails::Notifier.notify 'crails-tests passed', { html: html }, :success
       else
         Crails::Notifier.notify 'crails-tests failed', { html: html }, :failure
       end
    end

    def find_test_binary
      [ 'build/tests', 'bin/tests' ].each do |candidate|
        return candidate if File.exists? candidate
      end
      throw "cannot find binary for crails-tests"
    end
  end
end
