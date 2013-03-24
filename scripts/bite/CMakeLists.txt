cmake_minimum_required(VERSION 2.8)

project(bite)




set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wno-deprecated-declarations -pedantic -DASYNC_SERVER")

include_directories(include /usr/local/include .)

file(GLOB crails_core
     src/*.cpp)

file(GLOB crails_app
     app/controllers/*.cpp
     app/models/*.cpp
     app/routes.cpp
     main.cpp)

file(GLOB crails_sql
     src/sql/*.cpp)

file(GLOB crails_mongodb
     src/mongodb/*.cpp)

add_library(crails-core    SHARED ${crails_core})
add_library(crails-sql     SHARED ${crails_sql})
add_library(crails-mongodb SHARED ${crails_mongodb})
add_executable(crails-app ${crails_app})


target_link_libraries(crails-app
                      crails-core
                      boost_thread boost_filesystem boost_program_options boost_system
                      pthread dl
                      Boots
                      cppnetlib-client-connections cppnetlib-server-parsers cppnetlib-uri
                      
                      
                      )