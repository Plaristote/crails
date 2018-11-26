cmake_minimum_required(VERSION 3.0)

project(crails-models)

file(GLOB guard_models_plugins scripts/guard/*.rb)
file(GLOB cppgen_models_plugins scripts/cppgen/*.rb)

install(FILES ${guard_models_plugins}  DESTINATION share/crails/guard)
install(FILES ${cppgen_models_plugins} DESTINATION share/crails/cppgen)
