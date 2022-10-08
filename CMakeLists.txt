cmake_minimum_required(VERSION 3.16)

project(
  zen_remote
  LANGUAGES C CXX
  HOMEPAGE_URL https://github.com/zigen-project/zen-remote
  DESCRIPTION "Library for ZEN to communicate with devices over a network"
  VERSION 0.1.0.4
)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (required_version ${REQUIRED_ZEN_REMOTE_VERSION})

if(required_version AND (required_version VERSION_GREATER PROJECT_VERSION))
  message(
    FATAL_ERROR 
    "ZEN Remote is required to be version ${required_version}, \
but is ${PROJECT_VERSION}"
    )
endif()

if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
  file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()

add_compile_options(-Wall -Winvalid-pch -Wextra -Wpedantic -Werror)

set(public_inc ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(private_inc ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(boost_inc ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/boost-headers/include/)

add_subdirectory(src)

install(DIRECTORY ${public_inc} DESTINATION include)
