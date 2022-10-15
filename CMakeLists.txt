cmake_minimum_required(VERSION 3.18)

project(
  zen_remote
  LANGUAGES C CXX
  HOMEPAGE_URL https://github.com/zigen-project/zen-remote
  DESCRIPTION "Library for ZEN to communicate with devices over a network"
  VERSION 0.1.0.6
)

if (EXISTS local.cmake)
  include(local.cmake)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT DEFINED ZEN_REMOTE_TEST_CLIENT)
  set(ZEN_REMOTE_TEST_CLIENT ON)
endif()

if(ZEN_REMOTE_REQUIRED_VERSION VERSION_GREATER ${PROJECT_VERSION})
  message(FATAL_ERROR string(CONCAT
    "ZEN Remote is required to be version ${ZEN_REMOTE_REQUIRED_VERSION}, "
    "but is ${PROJECT_VERSION}"
  ))
endif()

if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
  file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()

set(zen_remote_public_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(zen_remote_private_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(zen_remote_boost_headers ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/boost-headers/include/)

add_subdirectory(protos)
add_subdirectory(src)

install(DIRECTORY ${zen_remote_public_headers} DESTINATION include)
