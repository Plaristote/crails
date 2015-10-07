cmake_minimum_required(VERSION 2.8)

add_subdirectory(Boots)

project(crails_project)

option(USE_MULTITHREAD "Asynchronous Server" ON)
option(USE_MONGODB     "MongoDB"             ON)
option(USE_SOCI        "Soci"                OFF)
option(DEVELOPER_MODE  "Developer mode"      ON)

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

include_directories(. include Boots/include /usr/local/include ${CPPNETLIB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

##
## Source files
##
file(GLOB crails_core
     src/*.cpp
     src/request_handlers/*.cpp
     src/request_parsers/*.cpp)

file(GLOB crails_sql
     src/sql/*.cpp)

file(GLOB crails_mongodb
     src/mongodb/*.cpp)
     
file(GLOB crails_mongodb_session
     src/session_store/mongodb/*.cpp)

file(GLOB crails_html src/renderers/html_renderer.cpp)
file(GLOB crails_json src/renderers/json_renderer.cpp)

file(GLOB crails_tests
     src/tests/*.cpp)

##
## Targets
##
if(DEVELOPER_MODE)
  if(USE_MULTITHREAD)
    set(crails-suffix -multithread-debug)
  else()
    set(crails-suffix -debug)
  endif()
elseif(USE_MULTITHREAD)
  set(crails-suffix -multithread)
endif()

add_library(crails-core${crails-suffix}      SHARED ${crails_core})
add_library(crails-html${crails-suffix}      SHARED ${crails_html})
add_library(crails-json${crails-suffix}      SHARED ${crails_json})
add_library(crails-tests${crails-suffix}     SHARED ${crails_tests})

if (USE_SOCI)
  add_library(crails-sql${crails-suffix}     SHARED ${crails_sql})
endif()

if (USE_MONGODB)
  add_library(crails-mongodb${crails-suffix} SHARED ${crails_mongodb})
  add_library(crails-mongodb-session${crails-suffix} SHARED ${crails_mongodb_session})
endif()

file(GLOB          headers_crails     crails/*.h crails/*.hpp)
file(GLOB          headers_crails_sql crails/sql/*.hpp)
file(GLOB          headers_crails_mdb crails/mongodb/*.hpp)
file(GLOB          headers_crails_cki crails/session_store/*.hpp)
file(GLOB          headers_crails_rhs crails/request_handlers/*.hpp)
file(GLOB          headers_crails_rdr crails/renderers/*.hpp)
file(GLOB          headers_crails_tst crails/tests/*.hpp)
file(GLOB          scripts            scripts/*.rb)
file(GLOB          scripts_templates  scripts/guard/templates/*.erb)
file(GLOB_RECURSE  app_template       app_tpl/*)
file(GLOB          guard_plugins      scripts/guard/*.rb)

install(PROGRAMS   scripts/crails         DESTINATION bin)
instalL(FILES      ${app_template}        DESTINATION share/crails/app_template)
install(FILES      ${scripts}             DESTINATION share/crails/scripts)
install(FILES      ${scripts_templates}   DESTINATION share/crails/guard/templates)
install(FILES      ${guard_plugins}       DESTINATION share/crails/guard)
install(FILES      ${headers_crails}      DESTINATION include/crails)
install(FILES      ${headers_crails_cki}  DESTINATION include/crails/session_store)
install(FILES      ${headers_crails_rhs}  DESTINATION include/crails/request_handlers)
install(FILES      ${headers_crails_rdr}  DESTINATION include/crails/renderers)
install(FILES      ${headers_crails_tst}  DESTINATION include/crails/tests)
install(TARGETS    crails-core${crails-suffix}    LIBRARY DESTINATION lib)
install(TARGETS    crails-html${crails-suffix}    LIBRARY DESTINATION lib)
install(TARGETS    crails-json${crails-suffix}    LIBRARY DESTINATION lib)
install(TARGETS    crails-tests${crails-suffix}   LIBRARY DESTINATION lib)

if (USE_SOCI)
  install(FILES    ${headers_crails_sql}  DESTINATION include/crails/sql)
  install(TARGETS  crails-sql${crails-suffix}     LIBRARY DESTINATION lib)
endif()
if (USE_MONGODB)
  install(FILES    ${headers_crails_mdb}  DESTINATION include/crails/mongodb)
  install(TARGETS  crails-mongodb${crails-suffix} LIBRARY DESTINATION lib)
  install(TARGETS  crails-mongodb-session${crails-suffix} LIBRARY DESTINATION lib)
endif()

