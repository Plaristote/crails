require 'guard/crails-base'
require 'guard/crails-notifier'
require 'guard/crails-server-watcher'

module ::Guard
  class CrailsServer < Plugin
    class << self
      attr_accessor :instance
    end

    attr_accessor :server

    def initialize options = {}
      options[:port] ||= 3001
      @server       = Crails::Server.new
      @server.port  = options[:port]
      @run_on_start = options[:run_on_start] == true
      super options
    end

    def start
      CrailsServer.instance = self
      start_server if @run_on_start == true
    end

    def run_all
      unless File.exists?('server.pid')
        start_server
      else
        puts ">> Not starting server: 'server.pid' file exists."
      end
    end

    def stop
      stop_server if File.exists?('server.pid')
    end

    def restart_server
      stop_server if File.exists?('server.pid')
      start_server
    end

  private
    def start_server
      @server.watch
    end

    def stop_server
      @server.interrupt
      File.delete 'server.pid' rescue ''
      Crails::Notifier.notify 'Crails Guard', 'Server stopped'
    end
  end
end
