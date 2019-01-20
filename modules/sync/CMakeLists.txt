cmake_minimum_required(VERSION 3.0)

project(crails-sync)

include_directories(. ../odb)

file(GLOB crails_sync src/*.cpp)

file(GLOB headers_crails_sync crails/sync/*.hpp)
file(GLOB headers_crails_odb  crails/odb/*.hpp)

file(GLOB crails_sync_templates app/config/sync.cpp)

file(GLOB crails_sync_scripts scripts/modules/sync/*.rb)

add_library(crails-sync${crails-suffix} SHARED ${crails_sync})

install(FILES ${headers_crails_sync}   DESTINATION include/crails/sync)
install(FILES ${headers_crails_odb}    DESTINATION include/crails/odb)
install(FILES ${crails_sync_templates} DESTINATION share/crails/app_template/sync)
install(FILES ${crails_sync_scripts}   DESTINATION share/crails/scripts/modules/sync)
install(TARGETS crails-sync${crails-suffix} LIBRARY DESTINATION lib)
