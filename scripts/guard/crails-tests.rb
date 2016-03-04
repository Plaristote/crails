require 'guard/crails-base'
require 'guard/crails-notifier'

module ::Guard
  class CrailsTests < Plugin
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
      end
      last_line.uncolorize
    end

    def run_tests
       last_line = run_command 'build/tests'
       html  = "<h4>crails-tests #{$?.success? ? 'passed' : 'failed'}</h4>"
       last_line.uncolorize
       html += "<div>#{last_line}</div>"
       if $?.success?
         Crails::Notifier.notify 'crails-tests passed', { html: html }, :success
       else
         Crails::Notifier.notify 'crails-tests failed', { html: html }, :failure
       end
    end
  end
end
