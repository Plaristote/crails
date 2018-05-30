cmake_minimum_required(VERSION 3.0)

project(crails-front)

include_directories(.)

file(GLOB crails_front            src/*.cpp)
file(GLOB crails_front_js         src/*.cpp ../../src/http.cpp)
file(GLOB crails_back             backend_src/*.cpp)
file(GLOB guard_front_plugins     scripts/guard/*.rb)
file(GLOB scripts_front_templates scripts/guard/templates/*.erb)
file(GLOB scripts_front_module    scripts/modules/front/*.rb)

add_library(crails-front${crails-suffix} SHARED ${crails_front} ${crails_back})

install(DIRECTORY crails                  DESTINATION include)
install(DIRECTORY src/front               DESTINATION share/crails/front)
install(FILES ${crails_front_js}          DESTINATION share/crails/front)
install(FILES ${scripts_front_templates}  DESTINATION share/crails/guard/templates)
install(FILES ${scripts_front_module}     DESTINATION share/crails/scripts/modules/front)
install(FILES ${guard_front_plugins}      DESTINATION share/crails/guard)
install(TARGETS crails-front${crails-suffix} LIBRARY DESTINATION lib)
