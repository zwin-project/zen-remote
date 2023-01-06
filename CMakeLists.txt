cmake_minimum_required(VERSION 3.18)

project(
  zen_remote
  LANGUAGES C CXX
  HOMEPAGE_URL https://github.com/zigen-project/zen-remote
  DESCRIPTION "Library for ZEN to communicate with devices over a network"
  VERSION 0.1.0.27
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (EXISTS local.cmake)
  include(local.cmake)
endif()

function(set_default variable default)
  if (NOT DEFINED ${variable} OR ${variable} STREQUAL "")
    set(${variable} ${default} PARENT_SCOPE)
  endif()
endfunction()

set_default(CMAKE_BUILD_TYPE Debug)
set_default(ZEN_REMOTE_CLIENT ON)
set_default(ZEN_REMOTE_SERVER ON)
set_default(ZEN_REMOTE_GRAPHICS_API OpenGL)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")

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

include(GNUInstallDirs)

add_subdirectory(3rd-party/glm)
add_subdirectory(protos)
add_subdirectory(src)
