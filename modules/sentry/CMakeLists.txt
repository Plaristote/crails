cmake_minimum_required(VERSION 3.0)

project(crails-sentry)

include_directories(.)

file(GLOB crails_sentry src/sentry.cpp)
file(GLOB scripts_sentry_module scripts/modules/sentry/*.rb)
file(GLOB_RECURSE sentry_app_template app/*)

add_library(crails-sentry${crails-suffix} SHARED ${crails_sentry})

install(DIRECTORY crails              DESTINATION include)
install(FILES ${scripts_sentry_module} DESTINATION share/crails/scripts/modules/sentry)
install(FILES ${sentry_app_template}  DESTINATION share/crails/app_template/sentry)
install(TARGETS crails-sentry${crails-suffix} LIBRARY DESTINATION lib)
