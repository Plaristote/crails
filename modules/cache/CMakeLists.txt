cmake_minimum_required(VERSION 3.0)

project(crails-cache)

file(GLOB crails_cache
     src/memcached.cpp)

file(GLOB headers_crails_cache
     crails/memcached.hpp)

file(GLOB crails_cache_templates
     app/config/memcached.cpp)

file(GLOB crails_cache_scripts
     scripts/modules/cache/*.rb)

add_library(crails-cache${crails-suffix} SHARED ${crails_cache})

install(FILES ${headers_crails_cache}   DESTINATION include/crails)
install(FILES ${crails_cache_templates} DESTINATION share/crails/app_template/cache)
install(FILES ${crails_cache_scripts}   DESTINATION share/crails/scripts/modules/cache)
install(TARGETS crails-cache${crails-suffix} LIBRARY DESTINATION lib)
