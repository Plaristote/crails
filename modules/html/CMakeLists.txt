cmake_minimum_required(VERSION 3.0)

project(crails-html)

file(GLOB crails_html            src/html_renderer.cpp)
file(GLOB headers_crails_html    crails/renderers/html_renderer.hpp)
file(GLOB guard_html_plugins     scripts/guard/*.rb)
file(GLOB scripts_html_templates scripts/guard/templates/*.erb)

add_library(crails-html${crails-suffix} SHARED ${crails_html})

install(FILES ${header_html_json}       DESTINATION include/crails/renderers)
install(FILES ${scripts_html_templates} DESTINATION share/crails/guard/templates)
install(FILES ${guard_html_plugins}     DESTINATION share/crails/guard)
install(TARGETS crails-html${crails-suffix} LIBRARY DESTINATION lib)
