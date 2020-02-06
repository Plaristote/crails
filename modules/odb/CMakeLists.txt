cmake_minimum_required(VERSION 3.0)

project(crails-odb)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

#find_package(ODB REQUIRED libodb OPTIONAL_COMPONENTS mysql pgsql sqlite oracle)

if (${ODB_mysql_FOUND})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DODB_WITH_MYSQL")
endif()

if (${ODB_sqlite_FOUND})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DODB_WITH_SQLITE")
endif()

if (${ODB_pgsql_FOUND})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DODB_WITH_PGSQL")
endif()

if (${ODB_oracle_FOUND})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DODB_WITH_ORACLE")
endif()

include_directories(. ${ODB_INCLUDE_DIRS})

file(GLOB_RECURSE crails_odb                src/*.cpp)
file(GLOB         headers_crails_odb        crails/odb/*.hpp)
file(GLOB         headers_crails_odb_model  crails/odb/model/*.hpp)
file(GLOB         headers_crails_odb_traits crails/odb/traits/*.hpp)
file(GLOB         guard_odb_plugin          scripts/guard/*.rb)
file(GLOB         scripts_odb_module        scripts/modules/odb/*.rb)
file(GLOB_RECURSE odb_app_template          app/*)
file(GLOB         lib_scripts               scripts/lib/*.rb)

add_library(crails-odb${crails-suffix} SHARED ${crails_odb})
install(FILES    ${headers_crails_odb}        DESTINATION include/crails/odb)
install(FILES    ${headers_crails_odb_model}  DESTINATION include/crails/odb/model)
install(FILES    ${headers_crails_odb_traits} DESTINATION include/crails/odb/traits)
install(FILES    ${guard_odb_plugin}          DESTINATION share/crails/guard)
install(FILES    ${scripts_odb_module}        DESTINATION share/crails/scripts/modules/odb)
install(FILES    ${odb_app_template}          DESTINATION share/crails/app_template/odb)
install(FILES    ${lib_scripts}               DESTINATION share/crails/scripts/lib)
install(TARGETS  crails-odb${crails-suffix} LIBRARY DESTINATION lib)
