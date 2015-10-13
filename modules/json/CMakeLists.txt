cmake_minimum_required(VERSION 3.0)

project(crails-json)

include_directories(.)

file(GLOB crails_json            src/json_renderer.cpp)
file(GLOB headers_crails_json    crails/renderers/json_renderer.hpp)
file(GLOB guard_json_plugins     scripts/guard/*.rb)
file(GLOB scripts_json_templates scripts/guard/templates/*.erb)

add_library(crails-json${crails-suffix} SHARED ${crails_json})

install(FILES ${headers_crails_json}    DESTINATION include/crails/renderers)
install(FILES ${scripts_json_templates} DESTINATION share/crails/guard/templates)
install(FILES ${guard_json_plugins}     DESTINATION share/crails/guard)
install(TARGETS crails-json${crails-suffix} LIBRARY DESTINATION lib)
