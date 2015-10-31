cmake_minimum_required(VERSION 3.0)

project(crails-odb)

include_directories(.)

file(GLOB         crails_odb            src/*.cpp)
file(GLOB         headers_crails_odb    crails/odb/*.hpp)
file(GLOB         guard_odb_plugin      scripts/guard/*.rb)
file(GLOB         scripts_sql_module    scripts/modules/odb/*.rb)
file(GLOB_RECURSE sql_app_template      app/*)

add_library(crails-odb${crails-suffix} SHARED ${crails_odb})
install(FILES    ${headers_crails_odb}      DESTINATION include/crails/odb)
install(FILES    ${guard_odb_plugin}        DESTINATION share/crails/guard)
install(FILES    ${scripts_odb_module}      DESTINATION share/crails/scripts/modules/odb)
install(FILES    ${odb_app_template}        DESTINATION share/crails/app_template/odb)
install(TARGETS  crails-odb${crails-suffix} LIBRARY DESTINATION lib)
