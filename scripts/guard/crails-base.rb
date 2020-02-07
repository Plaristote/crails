require 'pty'
require 'fileutils'

module ::Guard
  class CrailsPlugin < Plugin
    class << self
      attr_accessor :exit_success
    end

    def set_exit_success value
      CrailsPlugin.exit_success = value if @exit_success.nil? || value != 0
    end

    def watched_files
      files          = Dir["#{@base_path}**/*"]
      selected_files = []
      @watchers.each do |watcher|
        matcher = @file_matcher || watcher.pattern
        selected_files += files.select {|x| not matcher.match(x).nil? }
      end
      selected_files
    end

    def crails_include_path
      require_cmakecache
      source = "#{build_path}/CMakeFiles/server.dir/flags.make"
      flags_make = File.read source
      includes_define = flags_make.match(/^CXX_INCLUDES\s*=\s*([^\n]+)$/)[1]
      system_include_paths = ["/usr/include", "/usr/local/include"]
      custom_include_paths = unless includes_define.nil?
        includes_define.split(" ").map {|entry| entry.gsub(/^-I/, "")}
      else
        []
      end
      (system_include_paths + custom_include_paths).each do |path|
        return path if File.exists? "#{path}/crails/server.hpp"
      end
      nil
    end

    def platform
      case RbConfig::CONFIG['host_os']
      when /mswin|msys|mingw|cygwin|bccwin|wince|emc/
        :windows
      when /darwin|mac os/
        :macosx
      when /linux/
        :linux
      when /solaris|bsd/
        :unix
      else
        throw 'Cannot determine the host os'
      end
    end

    def dynlib_extension
      case platform
      when :windows
        'dll'
      when :macosx
        'dylib'
      else
        'so'
      end
    end

    def get_project_name
      get_cmake_variable 'CMAKE_PROJECT_NAME:STATIC'
    end

    def get_cmake_variable variable
      require_cmakecache
      `cat #{cmakecache_path} | grep #{variable} | cut -d= -f2`.strip!
    end

    def run_command command
      PTY.spawn(command) do |stdout, stdin, pid|
        begin
          stdout.each { |line| print line }
        rescue Errno::EIO
        end
        Process.wait(pid)
      end
    end

    def run_cmake
      cmakelists_path = Dir.pwd
      FileUtils.mkdir_p build_path
      Dir.chdir build_path do
        run_command "cmake #{cmakelists_path}"
      end
      $?.success?
    end

    def compiler
      compiler_path = get_cmake_variable 'CMAKE_CXX_COMPILER:FILEPATH'
      compiler_path = '/usr/bin/g++' if compiler_path.nil?
      compiler_path = if File.symlink? compiler_path
        File.readlink compiler_path
      else
        compiler_path
      end
      compiler_path.split('/').last
    end

    def developer_mode?
      (get_cmake_variable 'DEVELOPER_MODE:BOOL') == "ON"
    end

    def build_path
      if ENV['CRAILS_BUILD_PATH'].nil?
        'build'
      else
        ENV['CRAILS_BUILD_PATH']
      end
    end

    def require_cmakecache
      run_cmake unless File.exists? cmakecache_path
    end

    def cmakecache_path
      build_path + "/CMakeCache.txt"
    end
  end
end
