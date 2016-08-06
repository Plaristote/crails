cmake_minimum_required(VERSION 3.0)

project(crails-mongodb)

include_directories(.)

file(GLOB crails_mongodb             src/*.cpp)
file(GLOB crails_mongodb_session     src/session_store/*.cpp)
file(GLOB guard_mongodb_plugins      scripts/guard/*.rb)
file(GLOB scripts_mongodb_templates  scripts/guard/templates/*.erb)
file(GLOB scripts_mongodb_module     scripts/modules/mongodb/*.rb)
file(GLOB mongodb_app_template       app/config/db.json app/config/session_store.cpp)

add_library(crails-mongodb${crails-suffix} SHARED ${crails_mongodb})
add_library(crails-mongodb-session${crails-suffix} SHARED ${crails_mongodb_session})

install(DIRECTORY crails                       DESTINATION include)
install(FILES    ${scripts_mongodb_templates}  DESTINATION share/crails/guard/templates)
install(FILES    ${guard_mongodb_plugins}      DESTINATION share/crails/guard)
install(FILES    ${scripts_mongodb_module}     DESTINATION share/crails/scripts/modules/mongodb)
install(FILES    ${mongodb_app_template}       DESTINATION share/crails/app_template/mongodb)
install(TARGETS  crails-mongodb${crails-suffix} LIBRARY DESTINATION lib)
install(TARGETS  crails-mongodb-session${crails-suffix} LIBRARY DESTINATION lib)
