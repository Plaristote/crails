require 'guard/crails-base'
require 'guard/crails-notifier'
require 'guard/crails-server'
require 'pty'

module ::Guard
  class CrailsCmake < Plugin
    def run_all
      compile
    end

    def run_on_modifications(paths)
      run_all
    end

  private
    def run_command command
      PTY.spawn(command) do |stdout, stdin, pid|
        begin
          stdout.each { |line| print line }
        rescue Errno::EIO
        end
        Process.wait(pid)
      end
    end

    def compile
      Crails::Notifier.notify 'Crails Guard', 'Your crails server is recompiling right now'
      Dir.chdir 'build' do
        puts ">> Make server"
        run_command 'cmake ..'
        run_command 'make'  if $?.success?
        if $?.success?
          puts ">> Run tests"
          Dir.chdir '..' do
            run_command 'build/tests'
            if $?.success?
              restart_server
            else
              Crails::Notifier.notify 'Crails Guard', 'Tests are broken'
            end
          end
        else
          Crails::Notifier.notify 'Crails Guard', 'Your crails server failed to build'
        end
      end
    end

    def restart_server
      server = CrailsServer.instance
      server.restart_server if not server.nil?
    end
  end
end

