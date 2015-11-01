cmake_minimum_required(VERSION 3.0)

file(GLOB task_files
     *.cpp)

add_executable(task ${task_files})

target_link_libraries(task ${dependencies})
