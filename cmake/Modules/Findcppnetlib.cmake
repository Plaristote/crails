set(CPPNETLIB_PATH)
set(CPPNETLIB_FOUND FALSE)

find_path(CPPNETLIB_INCLUDE_DIR boost/network/version.hpp HINTS
  ${CPPNETLIB_ROOT}/include $ENV{CPPNETLIB_ROOT}/include
  ${COMMON_SOURCE_DIR}/cppnetlib ${CMAKE_SOURCE_DIR}/cppnetlib
  /usr/local/include
  /usr/include)

if(CPPNETLIB_INCLUDE_DIR)
  set(CPPNETLIB_PATH "${CPPNETLIB_INCLUDE_DIR}/..")
endif()

if(CPPNETLIB_PATH)
  set(__libraries cppnetlib-client-connections
                  cppnetlib-server-parsers
                  cppnetlib-uri)

  foreach(__library ${__libraries})
    if(TARGET ${__library})
      list(APPEND CPPNETLIB_LIBRARIES ${__library})
      set(CPPNETLIB_FOUND_SUBPROJECT ON)
    else()
      find_library(${__library} NAMES ${__library}
        HINTS ${CPPNETLIB_ROOT} $ENV{CPPNETLIB_ROOT}
        PATHS ${CPPNETLIB_PATH}/lib64 ${CPPNETLIB_PATH}/lib)
      list(APPEND CPPNETLIB_LIBRARIES ${${__library}})
    endif()
  endforeach()
  mark_as_advanced(CPPNETLIB_LIBRARIES)
endif()


if(NOT cppnetlib_FIND_QUIETLY)
  set(_cppnetlib_output 1)
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(cppnetlib DEFAULT_MSG
                                  CPPNETLIB_LIBRARIES
                                  CPPNETLIB_INCLUDE_DIR)

if(CPPNETLIB_FOUND)
  set(CPPNETLIB_INCLUDE_DIRS ${CPPNETLIB_INCLUDE_DIR})
  if(_cppnetlib_output)
    message(STATUS "Found cppnetlib in ${CPPNETLIB_INCLUDE_DIR}:${CPPNETLIB_LIBRARIES}")
  endif()
else()
  set(CPPNETLIB_INCLUDE_DIRS)
  set(CPPNETLIB_INCLUDE_DIR)
  set(CPPNETLIB_LIBRARIES)
  set(CPPNETLIB_FOUND)
endif()
