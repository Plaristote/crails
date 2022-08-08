cmake_minimum_required(VERSION 3.0)

project(crails-proxy)

include_directories(.)

file(GLOB crails_proxy            src/*.cpp)
file(GLOB headers_crails_proxy    crails/request_parsers/proxy.hpp)
file(GLOB template_crails_proxy   app/config/proxy.cpp)
file(GLOB scripts_crails_proxy    scripts/modules/proxy/*.rb)

add_library(crails-proxy${crails-suffix} SHARED ${crails_proxy})

install(FILES ${headers_crails_proxy}    DESTINATION include/crails/request_parsers)
install(FILES ${template_crails_proxy}   DESTINATION share/crails/app_template/proxy)
install(FILES ${scripts_crails_proxy}    DESTINATION share/crails/scripts/modules/proxy)
install(TARGETS crails-proxy${crails-suffix} LIBRARY DESTINATION lib)
