cmake_minimum_required(VERSION 2.8)

project(crails-docker)

file(GLOB_RECURSE docker_template       app/docker/*)
file(GLOB         scripts_docker_module scripts/modules/docker/*.rb)

install(FILES ${docker_template}       DESTINATION share/crails/app_template/docker)
install(FILES ${scripts_docker_module} DESTINATION share/crails/scripts/modules/docker)
