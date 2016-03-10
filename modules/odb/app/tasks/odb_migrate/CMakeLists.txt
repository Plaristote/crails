cmake_minimum_required(VERSION 3.0)

file(GLOB task_files
     *.cpp
     *.cxx)

add_executable(odb_migrate ${task_files})

target_link_libraries(odb_migrate ${dependencies})

set_target_properties(odb_migrate PROPERTIES OUTPUT_NAME "task")
