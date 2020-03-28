cmake_minimum_required(VERSION 3.0)

project(crails-front)

include_directories(.)

file(GLOB crails_server_src       src/shared/*.cpp src/server/*.cpp)
file(GLOB crails_client_src       src/shared/*.cpp src/client/*.cpp
                                  ../../src/utils/string_semantics.cpp
                                  ../../src/utils/string_split.cpp)
file(GLOB guard_front_plugins     scripts/guard/*.rb)
file(GLOB metarecord_generators   scripts/metarecord/generators/*.rb)
file(GLOB scripts_front_templates scripts/guard/templates/*.erb)
file(GLOB scripts_front_module    scripts/modules/front/*.rb)
file(GLOB lib_scripts             scripts/lib/*.rb)
file(GLOB_RECURSE docker_template app/docker/*)
file(GLOB_RECURSE front_templates app/front/*)

add_library(crails-front${crails-suffix} SHARED ${crails_server_src})

install(DIRECTORY crails                  DESTINATION include)
install(FILES ${crails_client_src}        DESTINATION share/crails/app_template/front)
install(FILES ${scripts_front_templates}  DESTINATION share/crails/guard/templates)
install(FILES ${scripts_front_module}     DESTINATION share/crails/scripts/modules/front)
install(FILES ${lib_scripts}              DESTINATION share/crails/scripts/lib)
install(FILES ${guard_front_plugins}      DESTINATION share/crails/guard)
install(FILES ${metarecord_generators}    DESTINATION share/crails/metarecord/generators)
install(FILES ${docker_template}          DESTINATION share/crails/app_template/docker)
install(FILES ${front_templates}          DESTINATION share/crails/app_template/front)
install(TARGETS crails-front${crails-suffix} LIBRARY DESTINATION lib)
