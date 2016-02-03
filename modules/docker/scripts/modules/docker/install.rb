#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'project_model'

project        = ProjectModel.new
base_directory = "#{Dir.pwd}/docker"
source         = "#{ENV['CRAILS_SHARED_DIR']}/app_template/docker"

project.base_directory source, Dir.pwd do
  project.directory :docker do
    project.file '.gitignore', 'gitignore'
    project.file 'build'
    project.file 'shell'
    project.directory :base do
      project.file 'Dockerfile'
      project.file 'compile-crails.sh'
      project.file 'compile-cppnetlib.sh'
      project.file 'compile-boost.sh'
      project.file 'compile-magick++.sh'
      project.file 'compile-libmemcached.sh'
      project.file 'compile-odb.sh'
      project.file 'compile-mongodb-cxx-driver.sh'
      project.file 'make-application-package.sh'
    end
  end
end

FileUtils.chmod 'ug+x', [ 'docker/build', 'docker/shell' ]

