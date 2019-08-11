cmake_minimum_required(VERSION 3.0)

project(crails-models)

file(GLOB headers_crails_models crails/models/*.hpp)
file(GLOB scripts_models_module scripts/modules/models/*.rb)
file(GLOB guard_models_plugins  scripts/guard/*.rb)
file(GLOB cppgen_models_plugins scripts/cppgen/*.rb)

install(FILES ${headers_crails_models} DESTINATION include/crails/models)
install(FILES ${scripts_models_module} DESTINATION share/crails/scripts/modules/models)
install(FILES ${guard_models_plugins}  DESTINATION share/crails/guard)
install(FILES ${cppgen_models_plugins} DESTINATION share/crails/cppgen)
