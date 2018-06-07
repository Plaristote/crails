cmake_minimum_required(VERSION 3.0)

project(crails-sidekic)

file(GLOB         crails_sidekic         src/*.cpp)
file(GLOB         headers_crails_sidekic crails/*.hpp)
file(GLOB         scripts_sidekic_module scripts/modules/sidekic/*.rb)
file(GLOB_RECURSE sidekic_app_template   app/*)

add_library(crails-sidekic${crails-suffix} SHARED ${crails_sidekic})
install(FILES    ${headers_crails_sidekic}      DESTINATION include/crails)
install(FILES    ${scripts_sidekic_module}      DESTINATION share/crails/scripts/modules/sidekic)
install(FILES    ${sidekic_app_template}        DESTINATION share/crails/app_template/sidekic)
install(TARGETS  crails-sidekic${crails-suffix} LIBRARY DESTINATION lib)

