file(GLOB_RECURSE CLIENT_SOURCE CONFIGURE_DEPENDS ./*.cc ./service/*.cc)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ../core/*.cc)

if (${ZEN_REMOTE_GRAPHICS_API} STREQUAL GLESv3)
  find_library(zen_remote_graphcs_lib NAMES GLESv3 REQUIRED)
  set(zen_remote_graphics_api_option "-DZEN_REMOTE_USE_GRAPHICS_API_GLES_V32")
elseif(${ZEN_REMOTE_GRAPHICS_API} STREQUAL OpenGL)
  find_library(zen_remote_graphcs_lib NAMES GLEW REQUIRED)
  set(zen_remote_graphics_api_option "-DZEN_REMOTE_USE_GRAPHICS_API_OPENGL")
endif()

add_library(
  zen-remote-client SHARED
  ${CLIENT_SOURCE}
  ${CORE_SOURCE}
)

target_include_directories(
  zen-remote-client
  PUBLIC ${zen_remote_public_headers}
  PRIVATE ${zen_remote_private_headers}
  PRIVATE ${zen_remote_boost_headers}
)

target_link_libraries(
  zen-remote-client
  PRIVATE
    zen_remote_grpc_proto
    glm::glm
    ${zen_remote_graphcs_lib}
)

target_precompile_headers(zen-remote-client PRIVATE ../pch/pch.h)

target_compile_options(
  zen-remote-client
  PRIVATE
    ${zen_remote_graphics_api_option}
    -Wall -Winvalid-pch -Wextra -Wpedantic -Werror
    -Wno-gnu-zero-variadic-macro-arguments
)

target_compile_definitions(
  zen-remote-client
  PRIVATE
    ZEN_REMOTE_CLIENT  
)

add_library(zen_remote::client ALIAS zen-remote-client)

install(TARGETS zen-remote-client)

configure_file(zen-remote-client.pc.in zen-remote-client.pc @ONLY) 

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/zen-remote-client.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
  DIRECTORY ${zen_remote_public_headers} 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  REGEX "zen-remote/server" EXCLUDE
)
