cmake_minimum_required(VERSION 3.0)

project(crails-attachment)

include_directories(.)

file(GLOB crails_attachment            src/*.cpp)
file(GLOB headers_crails_attachment    crails/*.hpp)
file(GLOB scripts_crails_attachment    scripts/modules/attachment/*.rb)
file(GLOB_RECURSE attachmant_app_template app/*)

add_library(crails-attachment${crails-suffix} SHARED ${crails_attachment})
install(FILES ${headers_crails_attachment}    DESTINATION include/crails)
install(FILES ${attachment_app_template}   DESTINATION share/crails/app_template/attachment)
install(FILES ${scripts_crails_attachment}    DESTINATION share/crails/scripts/modules/attachment)
install(TARGETS crails-attachment${crails-suffix} LIBRARY DESTINATION lib)

