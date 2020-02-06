cmake_minimum_required(VERSION 3.0)

project(crails-cache)

file(GLOB         crails_cache           src/memcached.cpp)
file(GLOB         headers_crails_cache   crails/memcached.hpp)
file(GLOB         crails_cache_templates app/config/memcached.cpp)
file(GLOB         crails_cache_scripts   scripts/modules/cache/*.rb)
file(GLOB         lib_scripts            scripts/lib/*.rb)
file(GLOB_RECURSE cache_app_template     app/*)

add_library(crails-cache${crails-suffix} SHARED ${crails_cache})

install(FILES ${headers_crails_cache}   DESTINATION include/crails)
install(FILES ${crails_cache_templates} DESTINATION share/crails/app_template/cache)
install(FILES ${crails_cache_scripts}   DESTINATION share/crails/scripts/modules/cache)
install(FILES ${lib_scripts}            DESTINATION share/crails/scripts/lib)
install(FILES ${cache_app_template}     DESTINATION share/crails/app_template/cache)
install(TARGETS crails-cache${crails-suffix} LIBRARY DESTINATION lib)
