cmake_minimum_required(VERSION 3.16)

project(zen_remote LANGUAGES C CXX VERSION 0.1.0.3)

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

include(FetchContent)
FetchContent_Declare(
  boost_headers_content
  GIT_REPOSITORY https://github.com/zigen-project/boost-headers.git
  GIT_TAG boost-1.80.0
)
FetchContent_MakeAvailable(boost_headers_content)

set(public_inc ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(private_inc ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)
