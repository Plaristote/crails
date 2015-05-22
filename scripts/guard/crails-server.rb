require 'guard/crails-base'
require 'guard/crails-notifier'

module ::Guard
  class CrailsServer < Plugin
    class << self
      attr_accessor :instance
    end

    attr_accessor :port

    def initialize options = {}
      options[:port] ||= 3001
      @port = options[:port]
      super options
    end

    def start
      CrailsServer.instance = self
      start_server
    end

    def restart_server
      stop_server unless @pid.nil?
      start_server
    end

  private
    def start_server
      @pid = fork do
        Crails::Notifier.notify 'Crails Guard', "Server starting now on port #{@port} (pid #{Process.pid})"
        $stdout.reopen("logs/out.#{Process.pid}.log", "w")
        $stderr.reopen("logs/err.#{Process.pid}.log", "w")
        exec "build/server -p #{@port}"
      end
    end

    def stop_server
      Process.kill('INT', @pid)
      Process.wait(@pid)
      Crails::Notifier.notify 'Crails Guard', 'Server stopped'
    end
  end
end
