#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'cmakelists'

cmake = CMakeLists.new
cmake.add_crails_module 'crud'

cmake.write
