CMAKE_CUSTOM_DEPENDENCIES_HEADER = "# Custom dependencies (do not modify this line)"
CMAKE_CUSTOM_MODULES_HEADER = "# Add your modules here (do not modify this line)"

class CMakeLists
  def initialize
    @source_file = 'CMakeLists.txt'
    @content = File.read @source_file
  end

  def write
    File.open(@source_file, 'w') do |file|
      file.write @content
    end
    puts "\033[32m[EDITED]\033[0m " + @source_file
  end

  def add_dependency name, dependency_group = "dependencies"
    append_dependency "set(#{dependency_group} ${#{dependency_group}} #{name})"
    puts "\033[32m[CMAKE]\033[0m Adding dependency #{name}"
  end

  def add_custom_module name
    append_module "add_subdirectory(modules/#{name})"
  end

  def add_crails_module name, dependency_group = "dependencies"
    add_dependency "crails-#{name}${crails-suffix}", dependency_group
  end

  def add_crails_task name
    @content += "\nadd_subdirectory(tasks/#{name})"
    puts "\033[32m[CMAKE]\033[0m Adding task #{name}"
  end

  def add_option opt
    option_pos = @content.index /option\([^)]+\)/n
    @content.insert option_pos, "option(#{opt})\n"
    puts "\033[32m[CMAKE]\033[0m Adding option #{opt.split(' ').first}"
  end

  def add_find_package command
    find_package_pos = @content.index /find_package\([^)]+\)/n
    @content.insert find_package_pos, "find_package(#{command})\n"
  end

  def add_include_directories include_dirs
    include_dir_pos = @content.index /include_directories\([^)]*\)/n
    @content.insert include_dir_pos, "include_directories(#{include_dirs})\n"
  end

  def add_code code
    @content += "\n#{code}"
  end

private
  def append_dependency str
    append_to CMAKE_CUSTOM_DEPENDENCIES_HEADER, str
  end

  def append_module str
    append_to CMAKE_CUSTOM_MODULES_HEADER, str
  end

  def append_to header, str
    @content.gsub!(header, "#{header}\n#{str}")
  end
end

