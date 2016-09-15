require 'pty'

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
        selected_files += files.select {|x| x =~ matcher}
      end
      selected_files
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

    def cmakecache_path
      build_path + "/CMakeCache.txt"
    end
  end
end
