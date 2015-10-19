cmake_minimum_required(VERSION 3.0)

project(crails-mail)

include_directories(.)

file(GLOB crails_mail
     src/mailer.cpp
     src/mail_servers.cpp)

file(GLOB headers_crails_mail
     crails/mailer.hpp
     crails/mail_servers.hpp)

file(GLOB crails_mail_templates
     app/config/mailers.json)

file(GLOB crails_mail_scripts
     scripts/modules/mail/*.rb)

add_library(crails-mail${crails-suffix} SHARED ${crails_mail})

install(FILES ${headers_crails_mail}   DESTINATION include/crails)
install(FILES ${crails_mail_templates} DESTINATION share/crails/app_template)
install(FILES ${crails_mail_scripts}   DESTINATION share/crails/scripts/modules/mail)
install(TARGETS crails-mail${crails-suffix} LIBRARY DESTINATION lib)
