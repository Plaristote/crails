cmake_minimum_required(VERSION 2.8)

project(crails_project)

option(USE_MULTITHREAD "Asynchronous Server" ON)
option(USE_MONGODB     "MongoDB"             ON)
option(USE_SOCI        "Soci"                OFF)
option(DEVELOPER_MODE  "Developer mode"      OFF)

cmake_policy(SET CMP0042 OLD)

##
## Compiler Options
##
set(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated-declarations -pedantic")

if (USE_MULTITHREAD)
  if (APPLE)
    message(FATAL_ERROR "Asynchronous server is not supported on your platform (APPLE)")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASYNC_SERVER")
endif()

if (DEVELOPER_MODE)
  message("-- Building development server")
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSERVER_DEBUG")
else()
  message("-- Building production server")
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -Wno-return-type-c-linkage")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

##
## Dependencies
##
find_package(cppnetlib REQUIRED)
find_package(Boost COMPONENTS thread filesystem program_options system REQUIRED)

include_directories(. include /usr/local/include ${CPPNETLIB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

##
## Source files
##
file(GLOB crails_core
     src/*.cpp)

file(GLOB crails_sql
     src/sql/*.cpp)

file(GLOB crails_mongodb
     src/mongodb/*.cpp)

##
## Targets
##
if (DEVELOPER_MODE)
  set(crails_core_name    crails-core-debug)
  set(crails_sql_name     crails-sql-debug)
  set(crails_mongodb_name crails-mongodb-debug)
else()
  set(crails_core_name    crails-core)
  set(crails_sql_name     crails-sql)
  set(crails_mongodb_name crails-mongodb)
endif()

add_library(${crails_core_name}      SHARED ${crails_core})

if (USE_SOCI)
  add_library(${crails_sql_name}     SHARED ${crails_sql})
endif()

if (USE_MONGODB)
  add_library(${crails_mongodb_name} SHARED ${crails_mongodb})
endif()

file(GLOB         headers_crails     crails/*.h crails/*.hpp)
file(GLOB         headers_crails_sql crails/sql/*.hpp)
file(GLOB         headers_crails_mdb crails/mongodb/*.hpp)
file(GLOB         headers_crails_cki crails/session_store/*.hpp)
file(GLOB         scripts            scripts/*.rb)
file(GLOB         scripts_generate   scripts/generate/*.rb)
file(GLOB         scripts_templates  scripts/templates/*.erb)
file(GLOB_RECURSE app_template       app_tpl/*)

install(PROGRAMS scripts/crails         DESTINATION bin)
instalL(FILES    ${app_template}        DESTINATION share/crails)
install(FILES    ${scripts}             DESTINATION share/crails/scripts)
install(FILES    ${scripts_generate}    DESTINATION share/crails/scripts/generate)
install(FILES    ${scripts_templates}   DESTINATION share/crails/scripts/templates)
install(FILES    ${headers_crails}      DESTINATION include/crails)
install(FILES    ${headers_crails_cki}  DESTINATION include/crails/session_store)
install(TARGETS  ${crails_core_name}    LIBRARY DESTINATION lib)
if (USE_SOCI)
  install(FILES    ${headers_crails_sql}  DESTINATION include/crails/sql)
  install(TARGETS  ${crails_sql_name}     LIBRARY DESTINATION lib)
endif()
if (USE_MONGODB)
  install(FILES    ${headers_crails_mdb}  DESTINATION include/crails/mongodb)
  install(TARGETS  ${crails_mongodb_name} LIBRARY DESTINATION lib)
endif()

