require 'guard/crails-notifier'
require 'date'

module Crails
  class Server
    attr_accessor :port
    attr_accessor :watcher_pid

    def watch
      Thread.new do
        spawn
      end
    end

    def spawn
      puts "Spawning"
      start_time = Time.now
      @pid       = fork do
        launch
      end
      File.open('server.pid', 'w') {|f| f.write @pid}
      Process.wait @pid
      File.delete('server.pid') rescue ''
      if $?.exitstatus != 0 && @interrupting != true
        elapsed_time = Time.now - start_time
        if elapsed_time > 5
          Crails::Notifier.notify 'Crails Guard', "Server returned with exit status '#{$?.exitstatus}' !"
          spawn
        else
          Crails::Notifier.notify 'Crails Guard', "Server died in #{elapsed_time.ceil}s with exit status '#{$?.exitstatus}'. The server will wait for your cue to restart (`crailsserver` in guard's console)."
        end
      end
      @interrupting = false
      @pid          = nil
    end

    def interrupt
      return if @pid.nil?
      puts "Interrupting server #{@pid}"
      @interrupting = true
      Process.kill('INT', @pid)
      Process.wait(@pid)
    end

    def launch
      Crails::Notifier.notify 'Crails Guard', "Server starting now on port #{@port} (pid #{Process.pid})"
      $stdout.reopen("logs/out.log", "a")
      $stderr.reopen("logs/err.log", "a")
      exec "build/server -p #{@port}"
    end
  end
end

