cmake_minimum_required(VERSION 3.0)

project(crails-sql)

include_directories(.)

file(GLOB crails_sql         src/*.cpp)
file(GLOB headers_crails_sql crails/sql/*.hpp)

add_library(crails-sql${crails-suffix} SHARED ${crails_sql})
install(FILES    ${headers_crails_sql}      DESTINATION include/crails/sql)
install(TARGETS  crails-sql${crails-suffix} LIBRARY DESTINATION lib)
