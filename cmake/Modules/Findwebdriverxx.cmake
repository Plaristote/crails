set(WEBDRIVERXX_PATH)
set(WEBDRIVERXX_FOUND FALSE)

find_path(WEBDRIVERXX_INCLUDE_DIR webdriverxx/webdriverxx.h HINTS
  ${WEBDRIVERXX_ROOT}/include $ENV${WEBDRIVERCXX_ROOT}/include
  ${COMMON_SOURCE_DIR}/webdriverxx ${CMAKE_SOURCE_DIR}/webdriverxx
  /usr/local/include
  /usr/include)

set(WEBDRIVERXX_LIBRARIES "")

if(WEBDRIVERXX_INCLUDE_DIR)
  set(WEBDRIVERXX_PATH "${WEBDRIVER_INCLUDE_DIR}/..")
endif()

if(NOT webdriverxx_FIND_QUIETLY)
  set(_webdriverxx_output 1)
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(webdriverxx DEFAULT_MSG
                                  WEBDRIVERXX_INCLUDE_DIR)

if(WEBDRIVERXX_FOUND)
  set(WEBDRIVERXX_INCLUDE_DIRS ${WEBDRIVERCXX_INCLUDE_DIR})
  if(_webdriverxx_output)
    message(STATUS "Found webdriverxx in ${WEBDRIVERXX_INCLUDE_DIR}")
  endif()
else()
  set(WEBDRIVERXX_INCLUDE_DIRS)
  set(WEBDRIVERXX_INCLUDE_DIR)
  set(WEBDRIVERXX_LIBRARIES)
  set(WEBDRIVERXX_FOUND)
endif()
