cmake_minimum_required(VERSION 3.0)

project(crails-crud)

include_directories(.)

file(GLOB crails_crud src/*.cpp)
file(GLOB headers_crails_crud crails/crud/*.hpp)
file(GLOB headers_odb_crud crails/odb/crud/*.hpp)
file(GLOB scripts_crud_module scripts/modules/crud/*.rb)

add_library(crails-crud${crails-suffix} SHARED ${crails_crud})

install(FILES ${headers_crails_crud} DESTINATION include/crails/crud)
install(FILES ${headers_odb_crud} DESTINATION include/crails/odb/crud)
install(FILES ${scripts_crud_module} DESTINATION share/crails/scripts/modules/crud)
install(TARGETS crails-crud${crails-suffix} LIBRARY DESTINATION lib)
