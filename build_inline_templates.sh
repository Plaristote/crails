#!/bin/bash -ex
ECPP=ecpp/ecpp
PROJECT_TPL=crails/new/templates
RENDERERS_TPL=crails/renderers
SCAFFOLDS_TPL=crails/scaffolds/templates
##
## AppTemplate
##
mkdir -p $PROJECT_TPL/build
$ECPP -c "" \
  -n ProjectCmakelistsTxt \
  -i $PROJECT_TPL/CMakeLists.txt.ecpp \
  >  $PROJECT_TPL/build/CMakeLists.txt.cpp
$ECPP -c "" \
  -n ProjectAppMainCpp \
  -i $PROJECT_TPL/app/main.cpp.ecpp \
  >  $PROJECT_TPL/build/main.cpp.cpp
$ECPP -c "" \
  -n ProjectAppRoutesCpp \
  -i $PROJECT_TPL/app/routes.cpp.ecpp \
  >  $PROJECT_TPL/build/routes.cpp.cpp
$ECPP -c "" \
  -n ProjectAppViewsExceptionHtml \
  -i $PROJECT_TPL/app/views/exception.html.ecpp \
  >  $PROJECT_TPL/build/exception.html.cpp
$ECPP -c "" \
  -n ProjectConfigEnvironmentCpp \
  -i $PROJECT_TPL/config/environment.cpp.ecpp \
  >  $PROJECT_TPL/build/environment.cpp.cpp
$ECPP -c "" \
  -n ProjectConfigDatabasesCpp \
  -i $PROJECT_TPL/config/databases.cpp.ecpp \
  >  $PROJECT_TPL/build/databases.cpp.cpp
$ECPP -c "" \
  -n ProjectConfigLoggerCpp \
  -i $PROJECT_TPL/config/logger.cpp.ecpp \
  >  $PROJECT_TPL/build/logger.cpp.cpp
$ECPP -c "" \
  -n ProjectConfigRenderersCpp \
  -i $PROJECT_TPL/config/renderers.cpp.ecpp \
  >  $PROJECT_TPL/build/renderers.cpp.cpp
$ECPP -c "" \
  -n ProjectConfigRequestPipeCpp \
  -i $PROJECT_TPL/config/request_pipe.cpp.ecpp \
  >  $PROJECT_TPL/build/request_pipe.cpp.cpp
$ECPP -c "" \
  -n ProjectConfigSaltCpp \
  -i $PROJECT_TPL/config/salt.cpp.ecpp \
  >  $PROJECT_TPL/build/salt.cpp.cpp
$ECPP -c "" \
  -n ProjectConfigSessionStoreCpp \
  -i $PROJECT_TPL/config/session_store.cpp.ecpp \
  >  $PROJECT_TPL/build/session_store.cpp
$ECPP -c "" \
  -n ProjectConfigSslCpp \
  -i $PROJECT_TPL/config/ssl.cpp.ecpp \
  >  $PROJECT_TPL/build/ssl.cpp
$ECPP -c "" \
  -n ProjectSpecMainCpp \
  -i $PROJECT_TPL/spec/main.cpp.ecpp \
  >  $PROJECT_TPL/build/spec.cpp

##
## Renderers
##
mkdir -p $RENDERERS_TPL/build
$ECPP -c "" \
  -n RendererCpp \
  -i $RENDERERS_TPL/renderer.cpp.ecpp \
  >  $RENDERERS_TPL/build/renderer.cpp

##
## Scaffolds
##
mkdir -p $SCAFFOLDS_TPL/build
$ECPP -c "" \
  -n ScaffoldsControllerCpp \
  -i $SCAFFOLDS_TPL/controller.cpp.ecpp \
  >  $SCAFFOLDS_TPL/build/controller.cpp.cpp
$ECPP -c "" \
  -n ScaffoldsControllerHpp \
  -i $SCAFFOLDS_TPL/controller.hpp.ecpp \
  >  $SCAFFOLDS_TPL/build/controller.hpp.cpp
