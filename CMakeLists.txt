cmake_minimum_required(VERSION 2.8)

project(crails_project)

# Define:
# - ASYNC_SERVER / SYNC_SERVER	to use the async or the sync server
# - SERVER_DEBUG		to enable non-production features (view compiler, displaying exceptions, ...)
# - USE_SEGVCATCH		if you're going to use the segvcatch library
# - USE_MONGODB_SESSION_STORE	to use MongoDB as a session store instead of the cookies
set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wno-deprecated-declarations -pedantic -DASYNC_SERVER -DSERVER_DEBUG -DUSE_MONGODB_SESSION_STORE")

include_directories(include /usr/local/include .)

file(GLOB crails_core
     src/*.cpp)

file(GLOB crails_sql
     src/sql/*.cpp)

file(GLOB crails_mongodb
     src/mongodb/*.cpp)

add_library(crails-core    SHARED ${crails_core})
add_library(crails-sql     SHARED ${crails_sql})
add_library(crails-mongodb SHARED ${crails_mongodb})

file(GLOB         headers_crails     crails/*.h crails/*.hpp)
file(GLOB         headers_crails_sql crails/sql/*.hpp)
file(GLOB         headers_crails_mdb crails/mongodb/*.hpp)
file(GLOB         headers_crails_cki crails/session_store/*.hpp)
file(GLOB         scripts            scripts/*.rb)
file(GLOB         scripts_generate   scripts/generate/*.rb)
file(GLOB_RECURSE app_template       app_tpl/*)

install(PROGRAMS scripts/crails     DESTINATION bin)
instalL(FILES    ${app_template}    DESTINATION share/crails)
install(FILES    ${scripts}         DESTINATION share/crails/scripts)
install(FILES    ${script_generate} DESTINATION share/crails/scripts/generate)
install(FILES    ${headers_crails}     DESTINATION include/crails)
install(FILES    ${headers_crails_sql} DESTINATION include/crails/sql)
install(FILES    ${headers_crails_mdb} DESTINATION include/crails/mongodb)
install(FILES    ${headers_crails_cki} DESTINATION include/crails/session_store)
install(TARGETS  crails-core    LIBRARY DESTINATION lib)
install(TARGETS  crails-mongodb LIBRARY DESTINATION lib)
install(TARGETS  crails-sql     LIBRARY DESTINATION lib)

