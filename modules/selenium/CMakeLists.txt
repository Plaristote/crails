cmake_minimum_required(VERSION 3.0)

project(crails-selenium)

include_directories(.)

file(GLOB crails_selenium            src/*.cpp)
file(GLOB headers_crails_selenium    crails/tests/*.hpp)
file(GLOB scripts_crails_selenium    scripts/modules/selenium/*.rb)

add_library(crails-selenium${crails-suffix} SHARED ${crails_selenium})

install(FILES ${headers_crails_selenium} DESTINATION include/crails/tests)
install(FILES ${scripts_crails_selenium} DESTINATION share/crails/scripts/modules/selenium)
install(TARGETS crails-selenium${crails-suffix} LIBRARY DESTINATION lib)
