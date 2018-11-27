cmake_minimum_required(VERSION 3.0)

project(crails-signin)

include_directories(.)

file(GLOB_RECURSE crails_signin         src/*.cpp)
file(GLOB         headers_crails_signin crails/signin/*.hpp)
file(GLOB         scripts_signin_module scripts/modules/signin/*.rb)
file(GLOB         signin_templates      app/config/signin.cpp)

add_library(crails-signin${crails-suffix} SHARED ${crails_signin})
install(FILES ${headers_crails_signin} DESTINATION include/crails/signin)
install(FILES ${scripts_signin_module} DESTINATION share/crails/scripts/modules/signin)
install(FILES ${signin_templates}      DESTINATION share/crails/app_template/signin)
install(TARGETS crails-signin${crails-suffix} LIBRARY DESTINATION lib)
