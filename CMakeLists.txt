cmake_minimum_required(VERSION 3.16)

project(zen_remote_display_system LANGUAGES C CXX VERSION 0.1.0.1)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (required_version ${REQUIRED_ZEN_REMOTE_DISPLAY_SYSTEM_VERSION})

if(required_version AND (required_version VERSION_GREATER PROJECT_VERSION))
  message(
    FATAL_ERROR 
    "ZEN Remote Display System is required to be version ${required_version}, \
but is ${PROJECT_VERSION}"
    )
endif()

if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
  file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()

add_compile_options(-Wall -Winvalid-pch -Wextra -Wpedantic -Werror)

add_subdirectory(client)
add_subdirectory(server)
