cmake_minimum_required(VERSION 3.0)

file(GLOB task_files
     sidetasks/*.cpp
     *.cpp
     *.cxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTHREAD_POOL_SIZE=2")

add_executable(sidekic ${task_files})

target_link_libraries(sidekic ${dependencies})

set_target_properties(sidekic PROPERTIES OUTPUT_NAME "task")
