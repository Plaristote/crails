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
      @server      = Crails::Server.new
      @server.port = options[:port]
      super options
    end

    def start
      CrailsServer.instance = self
      start_server
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
      Crails::Notifier.notify 'Crails Guard', 'Server stopped'
    end

    def server_pid
      File.read('server.pid').to_i
    end
  end
end
