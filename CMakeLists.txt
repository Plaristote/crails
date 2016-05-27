cmake_minimum_required(VERSION 2.8)

project(crails_project)

option(USE_MEMCACHED   "Memcached"           ON)
option(USE_MONGODB     "MongoDB"             ON)
option(USE_ODB         "ODB ORM"             ON)
option(DEVELOPER_MODE  "Developer mode"      ON)

cmake_policy(SET CMP0042 OLD)

##
## Compiler Options
##
set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -Wno-deprecated-declarations -pedantic")

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
find_package(Boost COMPONENTS thread filesystem program_options system random REQUIRED)
find_package(ImageMagick COMPONENTS Magick++)

include_directories(. include Boots/include /usr/local/include ${CPPNETLIB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ImageMagick_INCLUDE_DIRS})

##
## Source files
##
file(GLOB crails_core
     src/*.cpp
     src/request_handlers/*.cpp
     src/request_parsers/*.cpp
     src/utils/*.cpp)

file(GLOB crails_sql
     src/sql/*.cpp)

file(GLOB crails_tests
     src/tests/*.cpp)

##
## Targets
##
if(DEVELOPER_MODE)
  set(crails-suffix -debug)
endif()

add_library(crails-core${crails-suffix}      SHARED ${crails_core})
add_library(crails-tests${crails-suffix}     SHARED ${crails_tests})

file(GLOB          headers_crails     crails/*.h crails/*.hpp)
file(GLOB          headers_crails_cki crails/session_store/*.hpp)
file(GLOB          headers_crails_rhs crails/request_handlers/*.hpp)
file(GLOB          headers_crails_uti crails/utils/*.hpp)
file(GLOB          headers_crails_tst crails/tests/*.hpp)
file(GLOB          scripts            scripts/*.rb)
file(GLOB          scripts_lib        scripts/lib/*.rb)
file(GLOB          scripts_templates  scripts/guard/templates/*.erb)
file(GLOB_RECURSE  app_template       app_template/app/*)
file(GLOB_RECURSE  task_template      app_template/task/tasks/*)
file(GLOB          guard_plugins      scripts/guard/*.rb)

install(PROGRAMS   scripts/crails         DESTINATION bin)
instalL(FILES      ${app_template}        DESTINATION share/crails/app_template)
install(FILES      ${task_template}       DESTINATION share/crails/app_template/task)
install(FILES      ${scripts}             DESTINATION share/crails/scripts)
install(FILES      ${scripts_lib}         DESTINATION share/crails/scripts/lib)
install(FILES      ${scripts_templates}   DESTINATION share/crails/guard/templates)
install(FILES      ${guard_plugins}       DESTINATION share/crails/guard)
install(FILES      ${headers_crails}      DESTINATION include/crails)
install(FILES      ${headers_crails_cki}  DESTINATION include/crails/session_store)
install(FILES      ${headers_crails_rhs}  DESTINATION include/crails/request_handlers)
install(FILES      ${headers_crails_uti}  DESTINATION include/crails/utils)
install(FILES      ${headers_crails_tst}  DESTINATION include/crails/tests)
install(TARGETS    crails-core${crails-suffix}    LIBRARY DESTINATION lib)
install(TARGETS    crails-tests${crails-suffix}   LIBRARY DESTINATION lib)

add_subdirectory(modules/docker)
add_subdirectory(modules/html)
add_subdirectory(modules/json)
add_subdirectory(modules/mail)
add_subdirectory(modules/proxy)

if (${ImageMagick_Magick++_FOUND})
  add_subdirectory(modules/image)
endif()

if (USE_MEMCACHED)
  add_subdirectory(modules/cache)
endif()

if (USE_MONGODB)
#  add_subdirectory(modules/mongodb)
endif()

if (USE_ODB)
  add_subdirectory(modules/odb)
endif()
