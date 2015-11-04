cmake_minimum_required(VERSION 3.0)

project(crails-image)

include_directories(.)

file(GLOB         crails_image          src/*.cpp)
file(GLOB         headers_crails_image  crails/*.hpp)
file(GLOB         scripts_image_module  scripts/modules/image/*.rb)
file(GLOB_RECURSE sql_app_template      app/*)

add_library(crails-image${crails-suffix} SHARED ${crails_image})
install(FILES    ${headers_crails_image}    DESTINATION include/crails)
install(FILES    ${scripts_image_module}    DESTINATION share/crails/scripts/modules/image)
install(FILES    ${image_app_template}      DESTINATION share/crails/app_template/image)
install(TARGETS  crails-sql${crails-suffix} LIBRARY DESTINATION lib)
