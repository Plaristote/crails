CMAKE_CUSTOM_DEPENDENCIES_HEADER = "# Custom dependencies (do not modify this line)"

class CMakeLists
  def initialize
    @content = File.read 'CMakeLists.txt'
  end

  def write
    File.open(@source_file, 'w') do |file|
      file.write @content
    end
    puts "\033[32m[EDITED]\033[0m " + "CMakeLists.txt"
  end

  def add_dependency name
    append "set(dependencies ${dependencies} #{name})"
    puts "\033[32m[CMAKE]\033[0m Added dependency #{name}"
  end

  def add_crails_module name
    add_dependency "crails-#{name}${crails-suffix}"
  end

  def add_crails_task name
    @content += "\nadd_subdirectory(tasks/#{name})"
  end

  def add_find_package command
    find_package_pos = @content.index /find_package\([^)]+\)/n
    @content.insert find_package_pos, "#{command}\n"
  end

  def add_include_directories include_dirs
    include_dir_pos = @content.index /include_directories\([^)]\)/n
    @content.insert include_dir_pos, "include_directories(#{include_dirs})"
  end

  private
    def append str
      @content.gsub!(
        CMAKE_CUSTOM_DEPENDENCIES_HEADER,
        "#{CMAKE_CUSTOM_DEPENDENCIES_HEADER}\n#{str}"
      )
    end
  end
end


