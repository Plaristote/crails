cmake_minimum_required(VERSION 3.0)

project(crails-sql)

include_directories(.)

file(GLOB         crails_sql            src/*.cpp)
file(GLOB         headers_crails_sql    crails/sql/*.hpp)
file(GLOB         guard_sql_plugin      scripts/guard/*.rb)
file(GLOB         scripts_sql_templates scripts/guard/templates/*.erb)
file(GLOB         scripts_sql_module    scripts/modules/sql/*.rb)
file(GLOB_RECURSE sql_app_template      app/*)

add_library(crails-sql${crails-suffix} SHARED ${crails_sql})
install(FILES    ${headers_crails_sql}      DESTINATION include/crails/sql)
install(FILES    ${guard_sql_plugin}        DESTINATION share/crails/guard)
install(FILES    ${scripts_sql_templates}   DESTINATION share/crails/guard/templates)
install(FILES    ${scripts_sql_module}      DESTINATION share/crails/scripts/modules/sql)
install(FILES    ${sql_app_template}        DESTINATION share/crails/app_template/sql)
install(TARGETS  crails-sql${crails-suffix} LIBRARY DESTINATION lib)
