cmake_minimum_required(VERSION 3.0)

project(Boots)

if(UNIX)
  if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wno-deprecated-declarations -pedantic")
  elseif(APPLE)
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  endif()
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "-D_SCL_SECURE_NO_WARNINGS")
endif()

include_directories(include)

file(GLOB_RECURSE sourceFiles
     src/*.cpp)

file(GLOB_RECURSE testingFiles
     testing/*.cpp)

file(GLOB_RECURSE exempleServerFiles
     exemple/shared/*.cpp
     exemple/server/*.cpp)

file(GLOB_RECURSE exempleClientFiles
     exemple/shared/*.cpp
     exemple/client/*.cpp)

file(GLOB headers_network include/Boots/Network/*.hpp)
file(GLOB headers_sync    include/Boots/Sync/*.hpp)
file(GLOB headers_utils   include/Boots/Utils/*.hpp)
file(GLOB headers_system  include/Boots/System/*.hpp)

add_library(Boots       SHARED ${sourceFiles})
#add_library(StaticBoots STATIC ${sourceFiles})
add_executable(boots-tests ${testingFiles})
#add_executable(sample-server ${exempleServerFiles})
#add_executable(sample-client ${exempleClientFiles})

if(UNIX)
  install(FILES   ${headers_network} DESTINATION include/Boots/Network)
  install(FILES   ${headers_sync}    DESTINATION include/Boots/Sync)
  install(FILES   ${headers_utils}   DESTINATION include/Boots/Utils)
  install(FILES   ${headers_system}  DESTINATION include/Boots/System)
  install(TARGETS Boots LIBRARY DESTINATION lib)
  target_link_libraries(boots-tests    Boots pthread dl)
#  target_link_libraries(sample_server Boots pthread dl)
#  target_link_libraries(sample_client Boots pthread dl)
else()
  target_link_libraries(Boots       Ws2_32)
#  target_link_libraries(StaticBoots Ws2_32)
  target_link_libraries(boots-tests Boots pthread Ws2_32)
endif()
