cmake_minimum_required(VERSION 2.8)

project(crails-image)

include_directories(. ${ImageMagick_Magick++_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAGICKCORE_QUANTUM_DEPTH=16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAGICKCORE_HDRI_ENABLE=0")

file(GLOB         crails_image          src/*.cpp)
file(GLOB         headers_crails_image  crails/*.hpp)
file(GLOB         scripts_image_module  scripts/modules/image/*.rb)
file(GLOB_RECURSE image_app_template    app/*)

add_library(crails-image${crails-suffix} SHARED ${crails_image})
install(FILES    ${headers_crails_image}      DESTINATION include/crails)
install(FILES    ${scripts_image_module}      DESTINATION share/crails/scripts/modules/image)
install(FILES    ${image_app_template}        DESTINATION share/crails/app_template/image)
install(TARGETS  crails-image${crails-suffix} LIBRARY DESTINATION lib)
