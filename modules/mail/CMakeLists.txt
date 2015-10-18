cmake_minimum_required(VERSION 3.0)

project(crails-mail)

file(GLOB crails_mail
     src/mailer.cpp
     src/mail_servers.cpp)

file(GLOB headers_crails_mail
     crails/mailer.hpp
     crails/mail_servers.hpp)

file(GLOB crails_mail_templates
     app/config/mailers.json)

add_library(crails-mail${crails-suffix} SHARED ${crails_cache})

install(FILES ${headers_crails_mail} DESTINATION include/crails)
install(FILES ${crails_mail_templates} DESTINATION share/crails/app_template)
install(TARGETS crails-mail${crails-suffix} LIBRARY DESTINATION lib)
