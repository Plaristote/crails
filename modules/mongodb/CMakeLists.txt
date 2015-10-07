cmake_minimum_required(VERSION 3.0)

project(crails-mongodb)

include_directories(.)

file(GLOB crails_mongodb             src/*.cpp)
file(GLOB crails_mongodb_session     src/session_store/*.cpp)
file(GLOB headers_crails_mongodb     crails/mongodb/*.hpp)
file(GLOB headers_crails_cki_mongodb crails/session_store/*.hpp)
file(GLOB guard_mongodb_plugins      scripts/guard/*.rb)
file(GLOB scripts_mongodb_templates  scripts/guard/templates/*.erb)

add_library(crails-mongodb${crails-suffix} SHARED ${crails_mongodb})
add_library(crails-mongodb-session${crails-suffix} SHARED ${crails_mongodb_session})

install(FILES    ${headers_crails_mongodb}     DESTINATION include/crails/mongodb)
install(FILES    ${headers_crails_cki_mongodb} DESTINATION include/crails/session_store)
install(FILES    ${scripts_mongodb_templates}  DESTINATION share/crails/guard/templates)
install(FILES    ${guard_mongodb_plugins}      DESTINATION share/crails/guard)
install(TARGETS  crails-mongodb${crails-suffix} LIBRARY DESTINATION lib)
install(TARGETS  crails-mongodb-session${crails-suffix} LIBRARY DESTINATION lib)
