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

file(GLOB headers_utils include/Boots/Utils/*.hpp)

add_library(Boots SHARED ${sourceFiles})

install(FILES ${headers_utils} DESTINATION include/Boots/Utils)
install(TARGETS Boots LIBRARY DESTINATION lib)
