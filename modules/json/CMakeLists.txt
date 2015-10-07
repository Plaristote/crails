cmake_minimum_required(VERSION 3.0)

project(crails-json)

include_directories(.)

file(GLOB crails_json
     src/json_renderer.cpp)

file(GLOB headers_crails_json
     crails/renderers/json_renderer.hpp)

add_library(crails-json${crails-suffix} SHARED ${crails_json})

install(FILES ${header_crails_json} DESTINATION include/crails/renderers)
install(TARGETS crails-json${crails-suffix} LIBRARY DESTINATION lib)