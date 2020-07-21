class CrailsDocker < CrailsDockerBase
  def crails_compile_files
    ["compile-odb.sh"]
  end
  
  def system_dependencies
    ["libpq-dev"]
  end
end
