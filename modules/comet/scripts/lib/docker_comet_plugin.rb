class CrailsDocker < CrailsDockerBase
  def crails_compile_files
    ["compile-cheerp.sh"]
  end

  def system_dependencies
    ["libxml2-dev", "libz-dev"]
  end
end
