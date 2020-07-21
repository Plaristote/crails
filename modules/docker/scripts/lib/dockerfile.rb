#!/usr/bin/ruby

class Dockerfile
  def initialize
    @source_file = 'docker/base/Dockerfile'
    @content = File.read @source_file
  end

  def write
    File.open(@source_file, 'w') do |file|
      file.write @content
    end
    puts "\033[32m[EDITED]\033[0m " + @source_file
  end

  def add_compile_file file
    commands = "\nCOPY #{file} #{file}\nRUN bash ./#{file}"
    prepend_to "# END Dependencies", commands
  end

  def add_crails_compile_file file
    commands = "\nCOPY #{file} #{file}\nRUN bash ./#{file}"
    prepend_to "# END Crails dependencies", commands
  end

  def add_system_dependency package
    @content.gsub!("nodejs", "nodejs \\\n  #{package}")
  end

private
  def prepend_to footer, str
    @content.gsub!(footer, "#{str}\n#{footer}")
  end
end
