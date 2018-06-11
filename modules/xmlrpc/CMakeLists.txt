cmake_minimum_required(VERSION 3.0)

project(crails-xmlrpc)

file(GLOB crails_xmlrpc
     src/*.cpp)

file(GLOB headers_crails_xmlrpc
     crails/xmlrpc/*.hpp)

file(GLOB crails_xmlrpc_scripts
     scripts/modules/xmlrpc/*.rb)

add_library(crails-xmlrpc${crails-suffix} SHARED ${crails_xmlrpc})

install(FILES ${headers_crails_xmlrpc}   DESTINATION include/crails/xmlrpc)
install(FILES ${crails_xmlrpc_scripts}   DESTINATION share/crails/scripts/modules/xmlrpc)
install(TARGETS crails-xmlrpc${crails-suffix} LIBRARY DESTINATION lib)
