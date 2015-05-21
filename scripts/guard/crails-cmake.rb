require 'guard/crails-base'
require 'notifier'

module ::Guard
  class CrailsCmake < Plugin
    def run_all
      compile
    end

    def run_on_modifications(paths)
      run_all
    end

  private
    def compile
      `terminal-notifier -title 'Crails Guard' -message 'Your crails server is recompiling right now...'`
      Dir.chdir 'build' do
        puts ">> Make server"
        puts `cmake ..`
        puts `make`  if $?.success?
        if $?.success?
          puts ">> Run tests"
          puts `tests`
          if $?.success?
            restart_server
          else
            `terminal-notifier -title 'Crails Guard' -message 'Tests are broken'`
          end
        else
          `terminal-notifier -title 'Crails Guard' -message 'Your crails server failed to build'`
        end
      end
    end

    def restart_server
      puts "TODO restart server here"
    end
  end
end

