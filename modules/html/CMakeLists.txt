cmake_minimum_required(VERSION 3.0)

project(crails-html)

include_directories(.)

file(GLOB_RECURSE crails_html            src/*.cpp)
file(GLOB         guard_html_plugins     scripts/guard/*.rb)
file(GLOB         scripts_html_templates scripts/guard/templates/*.erb)
file(GLOB         scripts_html_module    scripts/modules/html/*.rb)
file(GLOB         html_templates         app/lib/exception.ecpp)

add_library(crails-html${crails-suffix} SHARED ${crails_html})

install(DIRECTORY crails DESTINATION include)
install(FILES ${scripts_html_templates} DESTINATION share/crails/guard/templates)
install(FILES ${scripts_html_module}    DESTINATION share/crails/scripts/modules/html)
install(FILES ${html_templates}         DESTINATION share/crails/app_template/html)
install(FILES ${guard_html_plugins}     DESTINATION share/crails/guard)
install(TARGETS crails-html${crails-suffix} LIBRARY DESTINATION lib)
