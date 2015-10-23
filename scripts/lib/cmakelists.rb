CMAKE_CUSTOM_DEPENDENCIES_HEADER = "# Custom dependencies (do not modify this line)"

class CMakeLists
  class << self
    def add_dependency name
      append "set(dependencies ${dependencies} #{name})"
      puts "[CMAKE] Added dependency #{name}"
    end

    def add_crails_module name
      add_dependency "crails-#{name}${crails-suffix}"
    end

    def add_crails_task name
      File.open('CMakeLists.txt', 'a') do |file|
        file << "\ninclude_subdirectory(tasks/#{name})"
      end
      puts "\033[32m[EDITED]\033[0m " + "File CMakeLists.txt"
    end

  private
    def append str
      content = File.read 'CMakeLists.txt'
      content.gsub!(
        CMAKE_CUSTOM_DEPENDENCIES_HEADER,
        "#{CMAKE_CUSTOM_DEPENDENCIES_HEADER}\n#{str}"
      )
      File.open('CMakeLists.txt', 'w') do |file|
        file.write content
      end
      puts "\033[32m[EDITED]\033[0m " + "File CMakeLists.txt"
    end
  end
end


