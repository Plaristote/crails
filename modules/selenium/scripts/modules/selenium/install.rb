#!/usr/bin/ruby

$: << "#{ENV['CRAILS_SHARED_DIR']}/scripts/lib"

require 'net/http'
require 'cmakelists'

selenium_minor_version = "3.14"
selenium_version       = selenium_minor_version + ".0"
selenium_package_name  = "selenium-server-standalone"

Net::HTTP.start "selenium-release.storage.googleapis.com", 443, use_ssl: true do |http|
  response = http.get "#{selenium_minor_version}/#{selenium_package_name}-#{selenium_version}.jar"
  open "spec/#{selenium_package_name}.jar", 'w' do |file|
    file.write response.body
  end
end

cmake = CMakeLists.new
cmake.add_dependency 'curl', 'tests_dependencies' # dependency of webdriverxx
cmake.add_crails_module 'selenium', 'tests_dependencies'
cmake.write

puts "/!\\ This module depends on libcurl and webdriverxx (https://github.com/durdyev/webdriverxx)"
